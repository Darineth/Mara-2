#include <QDateTime>

#include "MaraChatWidget.h"
#include "MaraStaticData.h"

MaraChatWidget::MaraChatWidget(QWidget *pParent) : QWidget(pParent)
{
	setupUi(this);
	reloadSettings();
	connect(mceChatEntry, SIGNAL(textEntered(QString)), this, SIGNAL(textEntered(QString)));
	mceChatEntry->setFocus(Qt::OtherFocusReason);

	//mtbChatArea->installEventFilter(this);
	mcbChatBrowser->installEventFilter(this);
	mceChatEntry->installEventFilter(this);

	mceChatEntry->setMaxLength(1000);

	mceChatEntry->setFocus();
}

MaraChatWidget::~MaraChatWidget() {}

void MaraChatWidget::reloadSettings()
{
	QPalette palette;
	palette.setColor(QPalette::Base, MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Background));
	palette.setColor(QPalette::Text, MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Default));
	//mtbChatArea->setPalette(palette);
	//mtbChatArea->setFont(MaraClientSettings::settings().load<QFont>(MaraClientSettings::Font::Default));

	if(MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::UserFontForEditBox))
	{
		palette.setColor(QPalette::Text, MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::User));
		mceChatEntry->setPalette(palette);
		mceChatEntry->setFont(MaraClientSettings::settings().load<QFont>(MaraClientSettings::Font::User));
		mceChatEntry->document()->setDefaultFont(MaraClientSettings::settings().load<QFont>(MaraClientSettings::Font::User));
	}
	else
	{
		palette.setColor(QPalette::Text, MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Default));
		mceChatEntry->setPalette(palette);
		mceChatEntry->setFont(MaraClientSettings::settings().load<QFont>(MaraClientSettings::Font::Default));
		mceChatEntry->document()->setDefaultFont(MaraClientSettings::settings().load<QFont>(MaraClientSettings::Font::Default));
	}

	mceChatEntry->updateSize();

	_timestamps = MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::Timestamps);
	_timestampFormat = MaraClientSettings::settings().load<QString>(MaraClientSettings::Chat::TimestampFormat);
}

void MaraChatWidget::appendText(const Mara::MTextStyle &pStyle, const QString &pText, bool pAllowTimestamp)
{
	QString timestamp = "";

	if(pAllowTimestamp && _timestamps)
	{
		timestamp = QDateTime::currentDateTime().toString(_timestampFormat) + " ";
	}

	mcbChatBrowser->appendHtml("<DIV ID=\"" + pStyle.name() + "\">" + timestamp + Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(pText)) + "</DIV>");
}

void MaraChatWidget::appendHtml(const QString &pHtml)
{
	mcbChatBrowser->appendHtml(pHtml);
}

bool MaraChatWidget::eventFilter(QObject *pObject, QEvent *pEvent)
{
	if(pObject == mcbChatBrowser)
	{
		if(pEvent->type() == QEvent::KeyPress)
		{
			QKeyEvent *ke = static_cast<QKeyEvent*>(pEvent);
			//if(ke->key() >= Qt::Key_F1 && ke->key() <= Qt::Key_F12)
			//{
			//	mceChatEntry->insertPlainText(MaraClientSettings::settings().load<QString>(MaraClientSettings::Macro::FArg.arg((ke->key() + 1) - Qt::Key_F1)));
			//	return 
			//}
			if(ke == QKeySequence::Paste)
			{
				if(mceChatEntry->isEnabled())
					mceChatEntry->paste();
				return true;
			}
			else if(ke == QKeySequence::Copy)
			{
				mcbChatBrowser->triggerPageAction(QWebPage::Copy);
				return true;
			}
			else if(ke->modifiers().testFlag(Qt::ControlModifier) ||
					ke->modifiers().testFlag(Qt::AltModifier))
			{
				return false;
			}
			else if(ke->key() != Qt::Key_PageUp &&
					ke->key() != Qt::Key_PageDown)
			{
				if(mceChatEntry->isEnabled())
				{
					mceChatEntry->setFocus(Qt::ActiveWindowFocusReason);
					QApplication::postEvent(mceChatEntry, new QKeyEvent(*ke));
				}
				return true;
			}
		}
	}
	else if(pObject == mceChatEntry)
	{
		if(pEvent->type() == QEvent::KeyPress)
		{
			QKeyEvent *ke = static_cast<QKeyEvent*>(pEvent);

			switch(ke->key())
			{
				case Qt::Key_F1:
				case Qt::Key_F2:
				case Qt::Key_F3:
				case Qt::Key_F4:
				case Qt::Key_F5:
				case Qt::Key_F6:
				case Qt::Key_F7:
				case Qt::Key_F8:
				case Qt::Key_F9:
				case Qt::Key_F10:
				case Qt::Key_F11:
				case Qt::Key_F12:
					if(mceChatEntry->isEnabled())
					{
						mceChatEntry->insertPlainText(MaraClientSettings::settings().load<QString>(MaraClientSettings::Macro::FArg.arg(ke->key() + 1 - Qt::Key_F1)));
					}
					return true;
				case Qt::Key_PageUp:
					mcbChatBrowser->freeze(true);
				case Qt::Key_PageDown:
					mcbChatBrowser->setFocus(Qt::ActiveWindowFocusReason);
					QApplication::postEvent(mcbChatBrowser, new QKeyEvent(*ke));
					return true;
			}
		}
	}

	return false;
}

void MaraChatWidget::setStyleSheet()
{
	//mtbChatArea->document()->setDefaultStyleSheet(MaraStaticData::styleSheet());
	mcbChatBrowser->setStyleSheet(MaraStaticData::styleSheet());
}

void MaraChatWidget::toggleFreeze()
{
	if(mcbChatBrowser->toggleFreeze())
	{
		appendHtml(MaraStaticData::plainToHtml("Freezing chat..."));
	}
	else
	{
		appendHtml(MaraStaticData::plainToHtml("Chat Unfrozen."));
	}
}

void MaraChatWidget::clear()
{
	mcbChatBrowser->clear();
	setStyleSheet();
	appendHtml(MaraStaticData::plainToHtml("Chat Cleared."));
}