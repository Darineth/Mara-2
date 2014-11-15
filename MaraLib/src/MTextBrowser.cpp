#include "MTextBrowser.h"

#include <QScrollBar>
#include <QTextFrame>

MTextBrowser::MTextBrowser( QWidget *pParent /*= 0*/ ) : QTextBrowser(pParent), _freeze(false)
{
	lineEndings.append(0);
	this->setAcceptDrops(false);
}

void MTextBrowser::setSource(const QUrl &name)
{
	//insertHtml("LINKED!");
	// Do nothing, we don't want set source to work.
	QDesktopServices::openUrl(name);
}

void MTextBrowser::mousePressEvent(QMouseEvent *e)
{
	if(tripleClickTimer.isActive() && (e->pos() - tripleClick).manhattanLength() < QApplication::startDragDistance())
	{
		// WE HAVE A TRIPLE CLICK! OMG!!!
		qint32 startIndex = 0;

		for(qint32 i = 1; i < lineEndings.size(); ++i)
		{
			if(textCursor().position() > lineEndings.at(i))
			{
				startIndex = i;
			}
			else
			{
				break;
			}
		}

		qint32 startPos = lineEndings.at(startIndex) + 1;
		qint32 endPos = lineEndings.at(startIndex + 1);
		QTextCursor newCursor = textCursor();
		newCursor.setPosition(startPos, QTextCursor::MoveAnchor);
		newCursor.setPosition(endPos, QTextCursor::KeepAnchor);
		setTextCursor(newCursor);
	}
	else
	{
		QTextBrowser::mousePressEvent(e);
	}
}

void MTextBrowser::timerEvent(QTimerEvent *e)
{
	if(e->timerId() == tripleClickTimer.timerId())
	{
		tripleClickTimer.stop();
	}
}

void MTextBrowser::mouseDoubleClickEvent(QMouseEvent *e)
{
	tripleClickTimer.start(QApplication::doubleClickInterval(), this);
	tripleClick = e->pos();
	QTextBrowser::mouseDoubleClickEvent(e);
}

void MTextBrowser::resizeEvent(QResizeEvent *pEvent)
{
	QTextBrowser::resizeEvent(pEvent);
	scrollToBottom();
}

void MTextBrowser::clear()
{
	lineEndings.clear();
	lineEndings.append(0);
	QTextBrowser::clear();
}

void MTextBrowser::appendHtml(const QString &pHtml)
{
	append(pHtml);
	lineEndings.append(((QTextFrame*)document()->object(0))->lastPosition());
	scrollToBottom();
}

void MTextBrowser::scrollToBottom()
{
	if(!_freeze)
	{
		verticalScrollBar()->setValue(verticalScrollBar()->maximum());
	}
}

QVariant MTextBrowser::loadResource(int type, const QUrl & name)
{
	return QVariant();
	//if(type == QTextDocument::ImageResource && name.scheme().toLower() == "http")
	//{
	//	//qDebug() << "WTF?";
	//	//qDebug() << "IMAGE: " << type << ": " << name; << "\nScheme: " << name.scheme() << "\nHost: " << name.host() << "\nPath: " << name.path();
	//	//server = url.scheme() + "://" + url.host();
	//	//00093       relativeUrl = url.path();
	//	return QVariant();
	//}
	//else
	//{
	//	return QVariant();
	//}
}

bool MTextBrowser::toggleFreeze()
{
	_freeze = !_freeze;

	if(!_freeze) scrollToBottom();
	return _freeze;
}

void MTextBrowser::freeze(bool pFreeze)
{
	_freeze = pFreeze;
}
