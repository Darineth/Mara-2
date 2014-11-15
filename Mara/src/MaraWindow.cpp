#include "MaraWindow.h"

#include <QMessageBox>

#include "MaraStaticData.h"

#include "MaraClient.h"

MaraWindow::MaraWindow(QWidget *pParent) : QMainWindow(pParent),
		_welcomeTabOpen(true),
		_channelWidgets(),
		_currentChannelWidget(0)
{
	// Allow Plugins (FLASH!)
	QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);

	setupUi(this);

	setWindowTitle(QString("Mara 2.2.7 - Qt %1").arg(qVersion()));
	setStatusBar(statusBar());
	statusBar()->showMessage("For help, press F1.");

	this->move(MaraClientSettings::settings().load(MaraClientSettings::UI::Location, QPoint(100, 100)));
	this->resize(MaraClientSettings::settings().load(MaraClientSettings::UI::Size, QSize(800, 600)));
	if(MaraClientSettings::settings().load(MaraClientSettings::UI::Maximized, false)) this->showMaximized();
	this->restoreState(MaraClientSettings::settings().load(MaraClientSettings::UI::State, QByteArray()));

	connect(qtwTabs, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
	connect(qaFreeze, SIGNAL(triggered()), this, SLOT(toggleFreeze()));
	connect(qaClear, SIGNAL(triggered()), this, SLOT(clearChat()));

	installEventFilter(this);

	reloadSettings();
}

MaraWindow::~MaraWindow() {}

bool MaraWindow::welcomeTabOpen() const
{
	return _welcomeTabOpen;
}

void MaraWindow::closeWelcomeTab()
{
	if(!_welcomeTabOpen) return;

	_welcomeTabOpen = false;

	qtwTabs->removeTab(qtwTabs->indexOf(qwWelcomeTab));

	delete qwWelcomeTab;
}

void MaraWindow::closeEvent(QCloseEvent *pEvent)
{
	if(QMessageBox::question(this, "Quit", "Are you sure you want to close Mara?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
	{
		MaraClientSettings::settings().save(MaraClientSettings::UI::Location, this->pos());
		MaraClientSettings::settings().save(MaraClientSettings::UI::Size, this->size());
		MaraClientSettings::settings().save(MaraClientSettings::UI::Maximized, this->isMaximized());
		MaraClientSettings::settings().save(MaraClientSettings::UI::State, this->saveState());

		for(QHash<QString, QPointer<MaraMessageWindow> >::iterator ii = _messageWindowsByName.begin(); ii != _messageWindowsByName.end(); ++ii)
		{
			if(*ii)
			{
				(*ii)->close();
			}
		}

		_messageWindows.clear();
		_messageWindowsByName.clear();

		pEvent->accept();
	}
	else
	{
		pEvent->ignore();
	}
}

void MaraWindow::reloadSettings()
{
	QList<QPointer<MaraChannelWidget> > mcws = _channelWidgets.values();
	for(QList<QPointer<MaraChannelWidget> >::iterator ii = mcws.begin(); ii != mcws.end(); ++ii)
	{
		if(*ii) (*ii)->reloadSettings();
	}
}

MaraChannelWidget *MaraWindow::getChannelWidget(const QString &pChannel)
{
	QString name = pChannel.simplified();

	if(_channelWidgets.contains(name))
	{
		if(_channelWidgets.value(name))
			return _channelWidgets.value(name);
		else
			_channelWidgets.remove(name);
	}

	MaraChannelWidget *mcw = new MaraChannelWidget(qtwTabs);

	mcw->setChannelName(name);

	qtwTabs->setCurrentIndex(qtwTabs->addTab(mcw, name));

	_channelWidgets.insert(name, mcw);

	connect(mcw, SIGNAL(textEntered(Mara::MChannel*, QString)), this, SIGNAL(channelTextEntered(Mara::MChannel*, QString)));
	connect(mcw, SIGNAL(openMessageWindow(QString)), this, SLOT(openMessageWindow(QString)));
	connect(this, SIGNAL(updateStyleSheets()), mcw, SLOT(setStyleSheet()));

	mcw->setStyleSheet();
	mcw->mcwChat->mceChatEntry->setFocus();
	mcw->mcwChat->mceChatEntry->installEventFilter(this);
	return mcw;
}

MaraChannelWidget *MaraWindow::currentChannelWidget()
{
	return _currentChannelWidget;
}

MaraMessageWindow *MaraWindow::getMessageWidget(Mara::MUser::Token pUserToken, bool pAllowCreate, bool pForceCreate)
{
	if(_messageWindows.contains(pUserToken))
	{
		if(_messageWindows.value(pUserToken))
			return _messageWindows.value(pUserToken);
		else
			_messageWindows.remove(pUserToken);
	}

	Mara::MUser *user = MaraStaticData::client()->users().value(pUserToken);

	if(user && _messageWindowsByName.contains(user->simpleName()))
	{
		MaraMessageWindow *mmw = _messageWindowsByName.value(user->simpleName());
		if(mmw)
		{
			mmw->associateUser(user);
			return mmw;
		}
		else
		{
			_messageWindowsByName.remove(user->simpleName());
		}
	}

	if(pForceCreate || (pAllowCreate && MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::MessageWindows)))
	{
		MaraMessageWindow *mmw = new MaraMessageWindow();

		_messageWindows.insert(pUserToken, mmw);
		_messageWindowsByName.insert(user->simpleName(), mmw);

		mmw->associateUser(user);

		connect(mmw, SIGNAL(textEntered(Mara::MUser*, QString)), this, SIGNAL(messageTextEntered(Mara::MUser*, QString)));
		connect(this, SIGNAL(updateStyleSheets()), mmw, SLOT(setStyleSheet()));
		connect(mmw, SIGNAL(closed()), this, SLOT(messageWindowClosed()));

		mmw->setStyleSheet();
		mmw->show();

		return mmw;
	}
	else
	{
		return 0;
	}
}

void MaraWindow::currentTabChanged(int index)
{
	if(index > -1)
	{
		_currentChannelWidget = qobject_cast<MaraChannelWidget *>(qtwTabs->widget(index));
	}
	else
	{
		_currentChannelWidget = 0;

	}
}

QList<MaraChannelWidget*> MaraWindow::channelWidgets() const
{
	QList<MaraChannelWidget*> widgets;

	for(int ii = 0; ii < qtwTabs->count(); ++ii)
	{
		MaraChannelWidget *mcw = qobject_cast<MaraChannelWidget*>(qtwTabs->widget(ii));
		if(mcw)
		{
			widgets.append(mcw);
		}
	}

	return widgets;
}

bool MaraWindow::eventFilter(QObject *pObject, QEvent *pEvent)
{
	Q_UNUSED(pObject);
	if(pEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent *ke = static_cast<QKeyEvent*>(pEvent);
		switch(ke->key())
		{
			case Qt::Key_Tab:
				if(ke->modifiers().testFlag(Qt::ControlModifier))
				{
					int tabIndex = qtwTabs->indexOf(currentChannelWidget());
					if(tabIndex < qtwTabs->count() - 1)
						qtwTabs->setCurrentIndex(tabIndex + 1);
					else
						qtwTabs->setCurrentIndex(0);
					return true;
				}
				break;
		}
	}

	return false;
}

void MaraWindow::messageWindowClosed()
{
	MaraMessageWindow *mmw = qobject_cast<MaraMessageWindow*>(sender());

	if(mmw)
	{
		for(QHash<QString, QPointer<MaraMessageWindow> >::iterator ii = _messageWindowsByName.begin(); ii != _messageWindowsByName.end(); ++ii)
		{
			if(*ii == mmw)
			{
				_messageWindowsByName.erase(ii);
				break;
			}
		}

		for(QHash<Mara::MUser::Token, QPointer<MaraMessageWindow> >::iterator ii = _messageWindows.begin(); ii != _messageWindows.end(); ++ii)
		{
			if(*ii == mmw)
			{
				_messageWindows.erase(ii);
				break;
			}
		}
	}
}

void MaraWindow::openMessageWindow(QString pUserName)
{
	for(MaraClient::Client::UserIterator ii = MaraStaticData::client()->users().begin(); ii != MaraStaticData::client()->users().end(); ++ii)
	{
		if((*ii)->simpleName() == pUserName)
		{
			getMessageWidget((*ii)->serverToken(), true, true);
			return;
		}
	}
}

void MaraWindow::toggleFreeze()
{
	if(_currentChannelWidget) _currentChannelWidget->toggleFreeze();
}

void MaraWindow::clearChat()
{
	if(_currentChannelWidget) _currentChannelWidget->clear();
}