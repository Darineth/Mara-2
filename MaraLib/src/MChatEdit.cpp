#include "MChatEdit.h"

const int MChatEdit::_maxHistoryRecords = 50;

MChatEdit::MChatEdit(QWidget *pParent) : QTextBrowser(pParent),
		_autoResize(true),
		_minimumSize(0),
		_maxLength(0),
		_history(),
		_historyIterator(_history.constEnd()),
		_lastEntered("")

{
	installEventFilter(this);
	connect(this, SIGNAL(textChanged()), this, SLOT(checkChangedText()));
}

MChatEdit::~MChatEdit() {}

bool MChatEdit::autoResize() const
{
	return _autoResize;
}

void MChatEdit::setAutoResize(bool pAutoResize)
{
	_autoResize = pAutoResize;
}

void MChatEdit::setSource(const QUrl &name)
{
	Q_UNUSED(name);
	//QDesktopServices::openUrl(name);
}

void MChatEdit::setFont(const QFont &pFont)
{
	QTextBrowser::setFont(pFont);
	document()->setDefaultFont(pFont);
	QTextDocument test;
	test.setDefaultFont(font());
	_minimumSize = test.size().height() + 4;
}

void MChatEdit::mousePressEvent(QMouseEvent *e)
{
	if(_tripleClickTimer.isActive() && (e->pos() - _tripleClick).manhattanLength() < QApplication::startDragDistance())
	{
		QTextCursor newCursor(document());
		newCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
		newCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
		setTextCursor(newCursor);
		_tripleClickTimer.stop();
	}
	else
	{
		QTextBrowser::mousePressEvent(e);
	}
}

void MChatEdit::timerEvent(QTimerEvent *e)
{
	if(e->timerId() == _tripleClickTimer.timerId())
	{
		_tripleClickTimer.stop();
	}
}

void MChatEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
	_tripleClickTimer.start(QApplication::doubleClickInterval(), this);
	_tripleClick = e->pos();
	QTextBrowser::mouseDoubleClickEvent(e);
}

QVariant MChatEdit::loadResource(int type, const QUrl & name)
{
	Q_UNUSED(type);
	Q_UNUSED(name);
	return QVariant();
}

bool MChatEdit::eventFilter(QObject *pObject, QEvent *pEvent)
{
	if(pObject == this && pEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent *ke = static_cast<QKeyEvent*>(pEvent);

		switch(ke->key())
		{
			case Qt::Key_Return:
			case Qt::Key_Enter:
				if(ke->modifiers() == Qt::NoModifier || ke->modifiers() == Qt::ShiftModifier)
				{
					QString text = toPlainText().left(_maxLength);
					if(text.length() > 0)
					{
						clear();
						appendHistory(text);
						emit textEntered(text);
					}
					ke->accept();
					return true;
				}
				else if(ke->modifiers() == Qt::ControlModifier)
				{
					insertPlainText("\n");
					return true;
				}
				break;
			case Qt::Key_Escape:
				clear();
				ke->accept();
				return true;
				break;
			case Qt::Key_Up:
				{
					QTextCursor c = textCursor();
					c.movePosition(QTextCursor::StartOfLine);

					if(ke->modifiers() == Qt::AltModifier || c.atStart())
					{
						if(_historyIterator != _history.constBegin())
						{
							if(_historyIterator == _history.constEnd())
							{
								_lastEntered = toPlainText();
							}
							--_historyIterator;

							document()->setPlainText(*_historyIterator);
						}
						ke->accept();
						return true;
					}
				}
				break;
			case Qt::Key_Down:
				QTextCursor c = textCursor();
				c.movePosition(QTextCursor::EndOfLine);

				if(ke->modifiers() == Qt::AltModifier || (c.atEnd() && (_historyIterator != _history.constEnd())))
				{
					if(_historyIterator != _history.constEnd())
					{
						++_historyIterator;
						if(_historyIterator == _history.constEnd())
						{
							document()->setPlainText(_lastEntered);
						}
						else
						{
							document()->setPlainText(*_historyIterator);
						}
					}
					ke->accept();
					return true;
				}
				break;
		}
	}
	
	return QTextBrowser::eventFilter(pObject, pEvent);
}

void MChatEdit::checkChangedText()
{
	if(_autoResize) updateSize();
	if(_maxLength) checkTextLength();
}

void MChatEdit::updateSize()
{
	int size = document()->size().height() + 4;
	if(size < _minimumSize) size = _minimumSize;
	setMaximumHeight(size);
}

bool MChatEdit::canInsertFromMimeData(const QMimeData *pSource) const
{
	return pSource->hasText() || pSource->hasUrls();
}

void MChatEdit::insertFromMimeData(const QMimeData *pSource)
{
	if(pSource->hasUrls())
	{
		QString allUrls = "";
		QList<QUrl> urls(pSource->urls());
		for(QList<QUrl>::const_iterator ii = urls.begin(); ii != urls.end(); ++ii)
		{
			if(allUrls.size() > 0)
				allUrls += (" " + (*ii).toString());
			else
				allUrls = (*ii).toString();
		}
		insertPlainText(allUrls);
	}
	else
	{
		insertPlainText(pSource->text());
	}
}

void MChatEdit::appendHistory(const QString &pText)
{
	_history.append(pText);
	_historyIterator = _history.constEnd();
	_lastEntered = "";

	if(_history.size() > _maxHistoryRecords) _history.removeFirst();
}

int MChatEdit::maxLength() const
{
	return _maxLength;
}

void MChatEdit::setMaxLength(int pMaxLength)
{
	_maxLength = pMaxLength;
}

void MChatEdit::checkTextLength()
{
	if(toPlainText().size() > _maxLength)
	{
		int pos = textCursor().position();
		if(pos > _maxLength) pos = _maxLength;
		setPlainText(toPlainText().left(_maxLength));
		QTextCursor cursor = textCursor();
		cursor.setPosition(pos);
		setTextCursor(cursor);
	}
}
