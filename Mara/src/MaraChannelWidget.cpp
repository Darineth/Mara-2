#include "MaraChannelWidget.h"

#include "MaraStaticData.h"

QHash<Mara::MChannel*, MaraChannelWidget*> MaraChannelWidget::_channelToWidget;

MaraChannelWidget::MaraChannelWidget(QWidget *pParent) : QWidget(pParent),
	_channel(0),
	_channelName("")
{
	setupUi(this);

	qlwUserList->setAutoFillBackground(true);

	QByteArray splitterState = MaraClientSettings::settings().load(MaraClientSettings::UI::SplitterSizes, QByteArray());

	if(!splitterState.isEmpty())
		qsplSplitter->restoreState(splitterState);

	connect(qsplSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(splitterMoved(int, int)));
	connect(mcwChat, SIGNAL(textEntered(QString)), this, SLOT(chatWidgetTextEntered(QString)));
	connect(qlwUserList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(userDoubleClicked(QListWidgetItem*)));

	mcwChat->mceChatEntry->installEventFilter(this);
	qlwUserList->installEventFilter(this);

	reloadSettings();
}

MaraChannelWidget::~MaraChannelWidget()
{
	dissociateChannel();
}

void MaraChannelWidget::reloadSettings()
{
	mcwChat->reloadSettings();

	QPalette palette;
	palette.setColor(QPalette::Base, MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Background));
	palette.setColor(QPalette::Background, MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Background));
	palette.setColor(QPalette::Foreground, MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Default));
	qlwUserList->setPalette(palette);

	qlbUserCount->setPalette(palette);
}

void MaraChannelWidget::associateChannel(Mara::MChannel *pChannel)
{
	dissociateChannel();
	_channel = pChannel;
	_channelToWidget.insert(_channel, this);
	_channelName = _channel->name();
}

void MaraChannelWidget::dissociateChannel()
{
	if(_channel)
	{
		if(_channelToWidget.contains(_channel))
		{
			_channelToWidget.remove(_channel);
		}
		_channel = 0;
	}
}

void MaraChannelWidget::appendText(const Mara::MTextStyle &pStyle, const QString &pText, bool pAllowTimestamp)
{
	mcwChat->appendText(pStyle, pText, pAllowTimestamp);
}

void MaraChannelWidget::splitterMoved(int pPos, int pIndex)
{
	Q_UNUSED(pPos);
	Q_UNUSED(pIndex);
	MaraClientSettings::settings().save(MaraClientSettings::UI::SplitterSizes, qsplSplitter->saveState());
}

const QString& MaraChannelWidget::channelName() const
{
	return _channelName;
}

void MaraChannelWidget::setChannelName(const QString &pChannelName)
{
	_channelName = pChannelName;
}

MaraChannelWidget* MaraChannelWidget::getChannelWidget(Mara::MChannel* pChannel)
{
	return _channelToWidget.value(pChannel, 0);
}

void MaraChannelWidget::chatWidgetTextEntered(QString pText)
{
	emit textEntered(_channel, pText);
}

void MaraChannelWidget::setStyleSheet()
{
	mcwChat->setStyleSheet();
	updateUserList();
}

void MaraChannelWidget::updateUserList()
{
	qlwUserList->clear();
	if(_channel)
	{
		QColor backgroundColor = MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Background);

		for(Mara::MChannel::UserIterator ii = _channel->users().begin(); ii != _channel->users().end(); ++ii)
		{
			const Mara::MUser *user = (*ii);
			QListWidgetItem *newClientItem = new QListWidgetItem(user->simpleName());
			newClientItem->setFont(user->font());

			QColor itemColor = user->color();

			if((abs(itemColor.red() - backgroundColor.red()) +
				abs(itemColor.green() - backgroundColor.green()) +
				abs(itemColor.blue() - backgroundColor.blue())) < 70)
			{
				itemColor = MaraStaticData::styleDefault().color();
			}

			newClientItem->setForeground(QBrush(itemColor));
			qlwUserList->addItem(newClientItem);
		}
		qlbUserCount->setText(QString("Users: %1").arg(_channel->users().size()));
	}
	else
	{
		qlbUserCount->setText("Not connected.");
	}
}

void MaraChannelWidget::userDoubleClicked(QListWidgetItem *pItem)
{
	if(QApplication::keyboardModifiers() == Qt::NoModifier)
		mcwChat->mceChatEntry->insertPlainText(pItem->text());
	else if(QApplication::keyboardModifiers() == Qt::ShiftModifier)
		emit openMessageWindow(pItem->text());
}

void MaraChannelWidget::appendHtml(const QString &pHtml)
{
	mcwChat->appendHtml(pHtml);
}

bool MaraChannelWidget::eventFilter(QObject *pObject, QEvent *pEvent)
{
	if(pObject == mcwChat->mceChatEntry)
	{
		if(pEvent->type() == QEvent::KeyPress)
		{
			QKeyEvent *ke = static_cast<QKeyEvent*>(pEvent);

			if(ke->key() == Qt::Key_Tab && ke->modifiers() == Qt::NoModifier)
			{
				if(mcwChat->mceChatEntry->toPlainText().length() == 0 && MaraStaticData::replyUser() != 0)
				{
					// Reply!
					mcwChat->mceChatEntry->insertPlainText("/msg " + MaraStaticData::replyUser()->simpleName() + " ");
					return true;
				}
			}
		}
	}
	else if(pObject == qlwUserList)
	{
		if(pEvent->type() == QEvent::KeyPress)
		{
			QKeyEvent *ke = static_cast<QKeyEvent*>(pEvent);
			if(ke->key() == Qt::Key_Escape)
			{
				qlwUserList->clearSelection();
				return true;
			}
		}
	}
	return false;
}

void MaraChannelWidget::toggleFreeze()
{
	mcwChat->toggleFreeze();
}

void MaraChannelWidget::clear()
{
	mcwChat->clear();
}