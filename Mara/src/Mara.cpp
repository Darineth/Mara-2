#include "Mara.h"
#include "MUser.h"
#include "MRandom.h"

#include "MaraClient.h"
#include "MaraStaticData.h"
#include "MLogger.h"

#include <QImageReader>
#include <QPixmap>
#include <QProcess>
#include <QSplashScreen>

// Static Variable Declarations
MaraApplication* MaraApplication::_sMaraApp = 0;

MaraApplication::MaraApplication(int &argc, char **argv) : QApplication(argc, argv), _debugLog(1000)
{
	_guiInitialized = false;

	QStringList paths;
	paths << QCoreApplication::applicationDirPath() + "/plugins";
	QCoreApplication::setLibraryPaths(paths);

	//QList<QByteArray> formats = QImageReader::supportedImageFormats();

	//for(QList<QByteArray>::iterator ii = formats.begin(); ii != formats.end(); ++ii)
	//{
	//	std::cout << (*ii).constData() << std::endl;
	//}

	_window = new MaraWindow(0);
	_client = new MaraClient::Client(0);

	qInstallMsgHandler(qtMessageHandler);
	qRegisterMetaType<Mara::LogLevel::Level>("Mara::LogLevel::Level");
	qRegisterMetaType<MaraClient::ClientStatus>("MaraClient::ClientStatus");
	qRegisterMetaType<MaraClient::ConnectionStatus>("MaraClient::ConnectionStatus");

	//QPixmap splashImage(":/images/Mara.png");
	//QSplashScreen *splash = new QSplashScreen(splashImage);
	//splash->setAttribute(Qt::WA_DeleteOnClose);
	//splash->show();
	//QTimer::singleShot(5000, splash, SLOT(close()));

	QObject::connect(Mara::MLogger::logger(), SIGNAL(logged(Mara::LogLevel::Level, const QString&)),
					 this, SLOT(logSlot(Mara::LogLevel::Level, const QString&)), 
					 Qt::QueuedConnection);
	QObject::connect(_client, SIGNAL(statusChanged(MaraClient::ClientStatus)), 
					 this, SLOT(logClientStatus(MaraClient::ClientStatus)),
					 Qt::QueuedConnection);
	QObject::connect(_client, SIGNAL(statusChanged(MaraClient::ClientStatus)), 
					 this, SLOT(clientStatusChanged(MaraClient::ClientStatus)),
					 Qt::QueuedConnection);
	QObject::connect(_client->connection(), SIGNAL(statusChanged(MaraClient::ConnectionStatus)), 
					 this, SLOT(logConnectionStatus(MaraClient::ConnectionStatus)),
					 Qt::QueuedConnection);
	//processEvents();
	initializeSettings();
	//processEvents();

	QObject::connect(this, SIGNAL(settingsChanged()), this, SLOT(reloadSettings()));
	QObject::connect(this, SIGNAL(settingsChanged()), _window, SLOT(reloadSettings()));
	QObject::connect(this, SIGNAL(updateStyleSheets()), _window, SIGNAL(updateStyleSheets()));

	QObject::connect(_window->qaExit, SIGNAL(triggered()), this, SLOT(quit()));
	QObject::connect(_window->qaSettings, SIGNAL(triggered()), this, SLOT(showSettings()));
	QObject::connect(_window->qaUpdate, SIGNAL(triggered()), this, SLOT(update()));

	QObject::connect(_window->qaConnect, SIGNAL(triggered()), this, SLOT(connect()), Qt::QueuedConnection);
	QObject::connect(_window->qaDisconnect, SIGNAL(triggered()), this, SLOT(stopReconnect()), Qt::QueuedConnection);
	QObject::connect(_window->qaDisconnect, SIGNAL(triggered()), _client, SLOT(disconnect()), Qt::QueuedConnection);

	QObject::connect(_window, SIGNAL(channelTextEntered(Mara::MChannel*, QString)), this, SLOT(channelTextEntered(Mara::MChannel*, QString)));
	QObject::connect(_window, SIGNAL(messageTextEntered(Mara::MUser*, QString)), this, SLOT(messageTextEntered(Mara::MUser*, QString)));

	QObject::connect(_client, SIGNAL(connected()), this, SLOT(clientConnected()));
	QObject::connect(_client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));

	QObject::connect(_client, SIGNAL(userConnected(Mara::MUser*)), this, SLOT(userConnected(Mara::MUser*)));
	QObject::connect(_client, SIGNAL(userDisconnected(Mara::MUser*)), this, SLOT(userDisconnected(Mara::MUser*)));
	QObject::connect(_client, SIGNAL(userReconnected(Mara::MUser*)), this, SLOT(userReconnected(Mara::MUser*)));
	QObject::connect(_client, SIGNAL(joinedChannel(Mara::MChannel*)), this, SLOT(joinedChannel(Mara::MChannel*)));
	QObject::connect(_client, SIGNAL(leftChannel(Mara::MChannel*)), this, SLOT(leftChannel(Mara::MChannel*)));
	QObject::connect(_client, SIGNAL(userJoinedChannel(Mara::MUser*, Mara::MChannel*)), this, SLOT(userJoinedChannel(Mara::MUser*, Mara::MChannel*)));
	QObject::connect(_client, SIGNAL(userLeftChannel(Mara::MUser*, Mara::MChannel*)), this, SLOT(userLeftChannel(Mara::MUser*, Mara::MChannel*)));
	QObject::connect(_client, SIGNAL(userUpdated(Mara::MUser*)), this, SLOT(userUpdated(Mara::MUser*)));
	QObject::connect(_client, SIGNAL(userChangedName(QString, QString)), this, SLOT(userChangedName(QString, QString)));
	QObject::connect(_client, SIGNAL(channelChat(Mara::MUser *, Mara::MChannel *, const QString &)), this, SLOT(channelChat(Mara::MUser *, Mara::MChannel *, const QString &)));
	QObject::connect(_client, SIGNAL(channelEmote(Mara::MUser *, Mara::MChannel *, const QString &)), this, SLOT(channelEmote(Mara::MUser *, Mara::MChannel *, const QString &)));
	QObject::connect(_client, SIGNAL(userAway(Mara::MUser *)), this, SLOT(userAway(Mara::MUser *)));
	QObject::connect(_client, SIGNAL(sentMessage(Mara::MUser *, const QString &)), this, SLOT(sentMessage(Mara::MUser *, const QString &)));
	QObject::connect(_client, SIGNAL(userMessage(Mara::MUser *, const QString &)), this, SLOT(userMessage(Mara::MUser *, const QString &)));
	QObject::connect(_client, SIGNAL(pingReceived()), this, SLOT(pingReceived()));
	QObject::connect(_client, SIGNAL(motd(QString)), this, SLOT(motd(QString)));
	QObject::connect(_client, SIGNAL(serverMessage(QString)), this, SLOT(serverMessage(QString)));
	QObject::connect(_client, SIGNAL(updateRequired()), this, SLOT(updateRequired()));

	QObject::connect(this, SIGNAL(appendHtml(MaraChatDisplay*, QString)), this, SLOT(appendHtmlSlot(MaraChatDisplay*, QString)), Qt::QueuedConnection);
	QObject::connect(&_reconnectTimer, SIGNAL(timeout()), this, SLOT(connect()));
	_reconnectTimer.setInterval(10000);
	_reconnectTimer.setSingleShot(true);

	QObject::connect(&_heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));

	_commandMap.insert("me", &MaraApplication::emote);
	_commandMap.insert("em", &MaraApplication::emote);
	_commandMap.insert("away", &MaraApplication::away);
	_commandMap.insert("name", &MaraApplication::name);
	_commandMap.insert("join", &MaraApplication::join);
	_commandMap.insert("leave", &MaraApplication::leave);
	_commandMap.insert("msg", &MaraApplication::message);
	_commandMap.insert("style", &MaraApplication::style);

	MaraStaticData::init(_client);

#ifndef __DEBUG__
	// Remove the Debug menu when in Release mode.
	_window->qmbMenuBar->removeAction(_window->qmDebug->menuAction());
#else
	//QObject::connect(_window->qaDebugChatWidget, SIGNAL(triggered()), this, SLOT(debug()));
	//QObject::connect(_window->qaDebugNetTest, SIGNAL(triggered()), this, SLOT(testNetwork()));
#endif

	reloadSettings();
	//processEvents();

	MaraStaticData::buildStyleSheet();
	//processEvents();

	_window->show();
	//splash.finish(_window);
	//processEvents();

	//QList<QByteArray> formats = QImageReader::supportedImageFormats();
	//QString formatStr = "";
	//for(int ii = 0; ii < formats.size(); ++ii)
	//{
	//	formatStr += formats.at(ii).constData();
	//	formatStr += " ";
	//}

	//Mara::MLogger::log(Mara::LogLevel::Debug, formatStr);

	_guiInitialized = true;

	QStringList args = arguments();

	for(QStringList::iterator ii = args.begin(); ii != args.end(); ++ii)
	{
		if((*ii).toLower() == "autoconnect")
		{
			QTimer::singleShot(2000, this, SLOT(connect()));
		}
	}
}

MaraApplication::~MaraApplication()
{
	QFile debugLog("maraclient_debug.log");

	if(debugLog.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		_debugLog.writeTo(&debugLog);
	}

	qInstallMsgHandler(0);
	_sMaraApp = 0;
	if(_client->connection()->connected()) _client->disconnect();
	MaraClientSettings::settings().sync();
	delete _client;
	delete _window;
}

void qtMessageHandler(QtMsgType type, const char *msg)
{
	MaraApplication::handleQtMessage(type, msg);
}

bool MaraApplication::notify(QObject *pReceiver, QEvent *pEvent)
{
	try
	{
		return QApplication::notify(pReceiver, pEvent);
	}
	catch(Mara::MException *ex)
	{
		logSlot(Mara::LogLevel::Error, "EXCEPTION CAUGHT FROM EVENT HANDLER: " + ex->toString());
		delete ex;
		return false;
	}
}

void MaraApplication::handleQtMessage(QtMsgType type, const char *msg)
{
	//switch(type)
	//{
	//	case QtDebugMsg:
	//		DEBUG_ONLY(std::cout << msg << std::endl);
	//		break;
	//	case QtWarningMsg:
	//		/*if(_sMaraApp && _sMaraApp->thread() == QThread::currentThread())
	//			QMessageBox::warning(_sMaraApp->_window, "Warning", msg);
	//		else*/
	//			std::cout << "[QtWARNING]" << msg << std::endl;
	//		//break;
	//	case QtCriticalMsg: 
	//		if(_sMaraApp && _sMaraApp->thread() == QThread::currentThread())
	//			QMessageBox::critical(_sMaraApp->_window, "Critical", msg);
	//		else
	//			std::cout << "[QtCRITICAL]" << msg << std::endl;
	//		break;
	//	case QtFatalMsg:
	//		std::cerr << msg << std::endl;
	//		throw new Mara::MException("FATAL QT ERROR", QString(msg));
	//		break;
	//};

	switch(type)
	{
		case QtDebugMsg:
			DEBUG_ONLY(std::cout << "[DEBUG]    " << msg << std::endl << std::flush);
			//_debugLog.log(msg);
			Mara::MLogger::log(Mara::LogLevel::Debug, msg);
			break;
		case QtWarningMsg:
			std::cout << "[WARNING]  " << msg << std::endl << std::flush;
			Mara::MLogger::log(Mara::LogLevel::Event, msg);
			//QMessageBox::warning(_sMaraApp->_window, "Warning", msg);
			break;
		case QtCriticalMsg: 
			std::cout << "[CRITICAL] " << msg << std::endl << std::flush;
			Mara::MLogger::log(Mara::LogLevel::Error, msg);
			//QMessageBox::critical(_sMaraApp->_window, "Critical", msg);
			break;
		case QtFatalMsg:
			std::cout << "[FATAL]    " << msg << std::endl << std::flush;
			throw new Mara::MException("FATAL QT ERROR", QString(msg));
			break;
	};
}

void MaraApplication::showSettings()
{
	MaraSettingsDialog msdDialog(_window);
	if(msdDialog.exec() == QDialog::Accepted)
	{
		msdDialog.saveSettings();
		emit settingsChanged();
		MaraStaticData::buildStyleSheet();
		emit updateStyleSheets();
	}
}

void MaraApplication::reloadSettings()
{
	MaraStaticData::reloadStaticSettings();

	_fileLogger.setFilenameTemplate(MaraClientSettings::settings().load<QString>(MaraClientSettings::Chat::LogFile));
	_fileLogging = MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::LogChat);

	_client->setToken(MaraClientSettings::settings().load<QString>(MaraClientSettings::User::Token));
	_client->user().setName(MaraClientSettings::settings().load<QString>(MaraClientSettings::User::Name));
	_client->user().setFont(MaraClientSettings::settings().load<QFont>(MaraClientSettings::Font::User));
	_client->user().setColor(MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::User));

	_heartbeatTimer.setInterval(MaraClientSettings::settings().load<int>(MaraClientSettings::Connection::HeartbeatTimeout));
	if(MaraClientSettings::settings().load<bool>(MaraClientSettings::Connection::EnableHeartbeat) && _client->isConnected())
	{
		_heartbeatTimer.start();
		_client->sendPing();
	}
	else
	{
		_heartbeatTimer.stop();
	}

	// Send user update!
	if(_client->isConnected()) _client->sendUserUpdate();
}

void MaraApplication::initializeSettings()
{
	Mara::MersenneRNG rng(QDateTime::currentDateTime().toTime_t());
	MaraClientSettings::settings().setDefault(MaraClientSettings::User::Name, QString("MaraUser{#ff0000}%1{#}").arg(rng() % 100));

	QString nameError;
	if(!Mara::MUser::validateUserName(MaraClientSettings::settings().load<QString>(MaraClientSettings::User::Name), nameError))
	{
		MaraClientSettings::settings().save(MaraClientSettings::User::Name, QString("MaraUser{#ff0000}%1{#}").arg(rng() % 100));
	}

	MaraClientSettings::settings().setDefault(MaraClientSettings::User::Token, Mara::MUser::generateUserToken());

	if(!Mara::MUser::validateUserToken(MaraClientSettings::settings().load<QString>(MaraClientSettings::User::Token)))
	{
		MaraClientSettings::settings().save(MaraClientSettings::User::Token, Mara::MUser::generateUserToken());
	}

	MaraClientSettings::settings().setDefault(MaraClientSettings::Chat::AllowUserFonts, true);
	MaraClientSettings::settings().setDefault(MaraClientSettings::Chat::ShowJoinLeave, true);
	MaraClientSettings::settings().setDefault(MaraClientSettings::Chat::Timestamps, true);
	MaraClientSettings::settings().setDefault(MaraClientSettings::Chat::TimestampFormat, "[hh:mm:ss]");
	MaraClientSettings::settings().setDefault(MaraClientSettings::Chat::LocalEcho, false);
	MaraClientSettings::settings().setDefault(MaraClientSettings::Chat::LogChat, false);
	MaraClientSettings::settings().setDefault(MaraClientSettings::Chat::LogFile, QString("./logs/%c/Mara_%yyyy-%mm.log"));
	MaraClientSettings::settings().setDefault(MaraClientSettings::Chat::MessageWindows, true);

	MaraClientSettings::settings().setDefault(MaraClientSettings::Image::MaxHeight, "500");
	MaraClientSettings::settings().setDefault(MaraClientSettings::Image::MaxWidth, "100%");

	MaraClientSettings::settings().setDefault(MaraClientSettings::Color::User, QColor(255, 255, 255));
	MaraClientSettings::settings().setDefault(MaraClientSettings::Color::Default, QColor(255, 255, 255));
	MaraClientSettings::settings().setDefault(MaraClientSettings::Color::Message, QColor(255, 255, 0));
	MaraClientSettings::settings().setDefault(MaraClientSettings::Color::System, QColor(0, 255, 0));
	MaraClientSettings::settings().setDefault(MaraClientSettings::Color::Link, QColor(0, 0, 255));
	MaraClientSettings::settings().setDefault(MaraClientSettings::Color::Background, QColor(0, 0, 0));

	QFont defaultFont("Arial", 10);
	MaraClientSettings::settings().setDefault(MaraClientSettings::Font::User, defaultFont);
	MaraClientSettings::settings().setDefault(MaraClientSettings::Font::Default, defaultFont);
	MaraClientSettings::settings().setDefault(MaraClientSettings::Font::Message, defaultFont);
	QFont fixedFont("Courier New", 10);
	MaraClientSettings::settings().setDefault(MaraClientSettings::Font::System, fixedFont);

	MaraClientSettings::settings().setDefault(MaraClientSettings::Chat::UserFontForEditBox, true);

	MaraClientSettings::settings().setDefault(MaraClientSettings::Connection::DataPort, 5005);
	MaraClientSettings::settings().setDefault(MaraClientSettings::Connection::Server, QString("127.0.0.1:5051"));
	MaraClientSettings::settings().setDefault(MaraClientSettings::Connection::BackupServer, QString(""));
	MaraClientSettings::settings().setDefault(MaraClientSettings::Connection::EnableHeartbeat, true);
	MaraClientSettings::settings().setDefault(MaraClientSettings::Connection::HeartbeatTimeout, 30000);

	MaraClientSettings::settings().setDefault(MaraClientSettings::Template::Chat, "<DIV STYLE=\"%USERSTYLE%\">%TIMESTAMP% &lt;%USERNAME%&gt; %TEXT%</DIV>");
	MaraClientSettings::settings().setDefault(MaraClientSettings::Template::Emote, "<DIV STYLE=\"%USERSTYLE%\">%TIMESTAMP% * %USERNAME% %TEXT%</DIV>");
	MaraClientSettings::settings().setDefault(MaraClientSettings::Template::Message, "<DIV STYLE=\"%MESSAGESTYLE%\">%TIMESTAMP% %TOFROM%: [%USERNAME%] %TEXT%</DIV>");
	MaraClientSettings::settings().setDefault(MaraClientSettings::Template::Plain, "<DIV STYLE=\"%SYSTEMSTYLE%\">%TIMESTAMP% %TEXT%</DIV>");

	MaraClientSettings::settings().setDefault(MaraClientSettings::Macro::F1, "Mara 2 Advice: Cry like a baby, you baby.");
}

void MaraApplication::connect()
{
	if(_window->welcomeTabOpen())
	{
		_window->closeWelcomeTab();
	}

	if(!_window->currentChannelWidget())
	{
		_window->getChannelWidget("Chat")->mcwChat->mceChatEntry->setFocus();
		//MaraStaticData::buildStyleSheet();
		//emit updateStyleSheets();
	}

	stopReconnect();

	QString host = MaraClientSettings::settings().load<QString>(MaraClientSettings::Connection::Server);
	QStringList hostParse = host.split(QChar(':'));
	if(hostParse.size() < 2)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Invalid host: %1").arg(host));
		return;
	}
	host = hostParse[0].trimmed();
	quint16 port = hostParse[1].trimmed().toUInt();
	_client->connect(host, port);
}

void MaraApplication::logSlot(Mara::LogLevel::Level pLevel, const QString& pText)
{
	QMutexLocker lock(&_logMutex);

	MaraChannelWidget *mcw = _window->currentChannelWidget();

	if(mcw)
	{
		switch(pLevel)
		{
			case Mara::LogLevel::AppStatus:
			case Mara::LogLevel::Error:
			case Mara::LogLevel::Event:
			case Mara::LogLevel::Memory:
				mcw->appendHtml(MaraStaticData::plainToHtml(pText));
				if(pLevel == Mara::LogLevel::Error)
				{
					MaraMessageWindow *mmw = qobject_cast<MaraMessageWindow*>(QApplication::activeWindow());
					if(mmw)
					{
						mmw->appendHtml(MaraStaticData::plainToHtml(pText));

					}
				}
				if(_fileLogging) _fileLogger.log(pText, mcw->channelName());
				_debugLog.log(pText);
				break;
			case Mara::LogLevel::Debug:
				DEBUG_ONLY(std::cout << "[" << pLevel << "]" << " " << qPrintable(pText) << "\n");
				_debugLog.log(pText);
				break;
		};
	}
	else
	{
		std::cout << "[" << pLevel << "]" << " " << qPrintable(pText) << "\n";
	}
}

void MaraApplication::logConnectionStatus(MaraClient::ConnectionStatus pStatus)
{
	switch(pStatus)
	{
		case MaraClient::Connection_Connecting:
			Mara::MLogger::log(Mara::LogLevel::AppStatus, QString("Connecting to %1...").arg(_client->connection()->serverAddress()));
			break;
		case MaraClient::Connection_Error:
			Mara::MLogger::log(Mara::LogLevel::Error, QString("Connection error: %1.").arg(_client->connection()->socketError()));
			break;
		case MaraClient::Connection_Ready:
			Mara::MLogger::log(Mara::LogLevel::AppStatus, QString("Connected!"));
			break;
		default:
			Mara::MLogger::log(Mara::LogLevel::Debug, QString("Connection Status: %1").arg(MaraClient::ClientConnection::statusToString(pStatus)));
	}
}

void MaraApplication::logClientStatus(MaraClient::ClientStatus pStatus)
{
	Mara::MLogger::log(Mara::LogLevel::Debug, QString("Client Status: %2").arg(MaraClient::Client::statusToString(pStatus)));
}

void MaraApplication::appendHtmlSlot(MaraChatDisplay *pWidget, QString pHtml)
{
	pWidget->appendHtml(pHtml);
}

void MaraApplication::clientConnected()
{
	QList<MaraChannelWidget*> channelWidgets = _window->channelWidgets();

	for(int ii = 0; ii < channelWidgets.size(); ++ii)
	{
		_client->sendPacket(new Mara::StandardPackets::JoinChannelPacket(_client->serverToken(), channelWidgets.at(ii)->channelName()));
	}

	if(MaraClientSettings::settings().load<bool>(MaraClientSettings::Connection::EnableHeartbeat))
	{
		_heartbeatTimer.start();
		_client->sendPing();
	}
}

void MaraApplication::clientDisconnected()
{
	MaraChannelWidget *mcw;
	for(MaraClient::Client::ChannelIterator ii = _client->channels().begin(); ii != _client->channels().end(); ++ii)
	{
		if(mcw = MaraChannelWidget::getChannelWidget(*ii))
		{
			mcw->dissociateChannel();
			mcw->updateUserList();
		}
	}

	MaraMessageWindow *mmw;

	for(MaraClient::Client::UserIterator ii = _client->users().begin(); ii != _client->users().end(); ++ii)
	{
		mmw = _window->getMessageWidget((*ii)->serverToken(), false, false);

		if(mmw)
		{
			mmw->disassociateUser();
		}
	}

	_heartbeatTimer.stop();

	Mara::MLogger::log(Mara::LogLevel::AppStatus, "Disconnected.");
}


void MaraApplication::channelTextEntered(Mara::MChannel *pChannel, QString pText)
{
	if(pText.at(0) == '/')
	{
		if(pText.size() == 1)
		{
			return;
		}
		QStringList argList = pText.mid(1).split(" ", QString::SkipEmptyParts);
		QString args = "";

		if(argList.size() > 1)
		{
			args = pText.mid(argList.at(0).size() + 2);
		}

		if(_commandMap.contains(argList.at(0)))
		{
			(this->*(_commandMap.value(argList.at(0))))(argList, args, pChannel, 0);
		}
		else
		{
			QString log = args.length() > 0 ? QString("Sending command \"%1 - %2\"").arg(argList.at(0)).arg(args) : QString("Sending command \"%1\"").arg(argList.at(0));

			Mara::MLogger::log(Mara::LogLevel::Event, log);

			serverCommand(argList.at(0), args);
		}
	}
	else
	{
		// Send chat!
		if(_client->isConnected() && MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::LocalEcho))
		{
			MaraChannelWidget *mcw;
			if(mcw = MaraChannelWidget::getChannelWidget(pChannel))
			{
				emit appendHtml(mcw, MaraStaticData::chatToHtml(_client->user().name(), pText, _client->user().style()));
				if(_fileLogging) _fileLogger.log("<" + _client->user().simpleName() + "> " + pText, pChannel->name());
			}
		}
		pText = _mpmPluginManager.pluginExecute().preprocessOutgoing(pText);
		_client->sendChat(pChannel, pText);
	}
}

void MaraApplication::messageTextEntered(Mara::MUser *pUser, QString pText)
{
	if(pText.at(0) == '/')
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Commands may not be issued from message windows."));
	}
	else
	{
		// Send Message!
		_client->sendMessage(pUser, pText);
	}
}

void MaraApplication::userConnected(Mara::MUser *pUser)
{
	if(MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::ShowJoinLeave))
		Mara::MLogger::log(Mara::LogLevel::Event, QString("User %1 connected.").arg(pUser->name()));
	//MaraStaticData::buildStyleSheet();
	//emit updateStyleSheets();
}

void MaraApplication::userDisconnected(Mara::MUser *pUser)
{
	if(MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::ShowJoinLeave))
		Mara::MLogger::log(Mara::LogLevel::Event, QString("User %1 disconnected.").arg(pUser->name()));

	MaraMessageWindow *mmw = _window->getMessageWidget(pUser->serverToken(), false, false);

	if(mmw)
	{
		mmw->disassociateUser();
	}

	//MaraStaticData::buildStyleSheet();
	emit updateStyleSheets();
}

void MaraApplication::userReconnected(Mara::MUser *pUser)
{
	if(MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::ShowJoinLeave))
		Mara::MLogger::log(Mara::LogLevel::Event, QString("User %1 reconnected.").arg(pUser->name()));
	//MaraStaticData::buildStyleSheet();
	//emit updateStyleSheets();
}

void MaraApplication::joinedChannel(Mara::MChannel *pChannel)
{
	MaraChannelWidget *mcw = _window->getChannelWidget(pChannel->name());
	//MaraStaticData::buildStyleSheet();
	emit updateStyleSheets();
	mcw->associateChannel(pChannel);

	_window->qtwTabs->setTabText(_window->qtwTabs->indexOf(mcw), mcw->channelName());
}

void MaraApplication::leftChannel(Mara::MChannel *pChannel)
{
	MaraChannelWidget *mcw = _window->getChannelWidget(pChannel->name());

	_window->qtwTabs->removeTab(_window->qtwTabs->indexOf(mcw));

	delete mcw;
}

void MaraApplication::userJoinedChannel(Mara::MUser *pUser, Mara::MChannel *pChannel)
{
	MaraChannelWidget *mcw;
	if(mcw = MaraChannelWidget::getChannelWidget(pChannel))
	{
		if(MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::ShowJoinLeave))
			Mara::MLogger::log(Mara::LogLevel::Event, QString("User %1 joined channel %2.").arg(pUser->name()).arg(pChannel->name()));
		mcw->updateUserList();
	}
	else
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Unable to locate widget for channel %1.").arg(pChannel->name()));
	}
}

void MaraApplication::userLeftChannel(Mara::MUser *pUser, Mara::MChannel *pChannel)
{
	MaraChannelWidget *mcw;
	if(mcw = MaraChannelWidget::getChannelWidget(pChannel))
	{
		if(MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::ShowJoinLeave))
			Mara::MLogger::log(Mara::LogLevel::Event, QString("User %1 left channel %2.").arg(pUser->name()).arg(pChannel->name()));
		mcw->updateUserList();
	}
	else
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Unable to locate widget for channel %1.").arg(pChannel->name()));
	}
}

void MaraApplication::userUpdated(Mara::MUser *pUser)
{
	Q_UNUSED(pUser);
	//MaraStaticData::buildStyleSheet();
	emit updateStyleSheets();
}

void MaraApplication::channelChat(Mara::MUser *pUser, Mara::MChannel *pChannel, const QString &pText)
{
	MaraChannelWidget *mcw = MaraChannelWidget::getChannelWidget(pChannel);
	if(!mcw)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Received chat for unassociated channel %1.").arg(pChannel->name()));
	}

	if(pUser->serverToken() != _client->serverToken() || !MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::LocalEcho))
	{
		emit appendHtml(mcw, MaraStaticData::chatToHtml(pUser->name(), pText, pUser->style()));
		if(_fileLogging) _fileLogger.log("<" + pUser->simpleName() + "> " + pText, pChannel->name());
	}
}

void MaraApplication::channelEmote(Mara::MUser *pUser, Mara::MChannel *pChannel, const QString &pText)
{
	MaraChannelWidget *mcw = MaraChannelWidget::getChannelWidget(pChannel);
	if(!mcw)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Received chat for unassociated channel %1.").arg(pChannel->name()));
	}

	if(pUser->serverToken() != _client->serverToken() || !MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::LocalEcho))
	{
		emit appendHtml(mcw, MaraStaticData::emoteToHtml(pUser->name(), pText, pUser->style()));
		if(_fileLogging) _fileLogger.log("* " + pUser->simpleName() + " " + pText, pChannel->name());
	}
}

void MaraApplication::userAway(Mara::MUser *pUser)
{
	if(_client->channels().size() == 0)
	{
		MaraChannelWidget *mcw = _window->currentChannelWidget();
		if(mcw)
		{
			if(pUser->away() != "")
			{
				emit appendHtml(mcw, MaraStaticData::emoteToHtml(pUser->name(), QString("is away (%1).").arg(pUser->away()), pUser->style()));
				if(_fileLogging) _fileLogger.log("* " + pUser->simpleName() + " (" + pUser->away() + ")", mcw->channelName());
			}
			else
			{
				emit appendHtml(mcw, MaraStaticData::emoteToHtml(pUser->name(), "is back.", pUser->style()));
				if(_fileLogging) _fileLogger.log("* " + pUser->simpleName() + " is back.", mcw->channelName());
			}
		}
	}
	else
	{
		for(MaraClient::Client::ChannelIterator ii = _client->channels().begin(); ii != _client->channels().end(); ++ii)
		{
			MaraChannelWidget *mcw = MaraChannelWidget::getChannelWidget(*ii);
			if(mcw)
			{
				if(pUser->away() != "")
				{
					emit appendHtml(mcw, MaraStaticData::emoteToHtml(pUser->name(), QString("is away (%1).").arg(pUser->away()), pUser->style()));
					if(_fileLogging) _fileLogger.log("* " + pUser->simpleName() + " (" + pUser->away() + ")", (*ii)->name());
				}
				else
				{
					emit appendHtml(mcw, MaraStaticData::emoteToHtml(pUser->name(), "is back.", pUser->style()));
					if(_fileLogging) _fileLogger.log("* " + pUser->simpleName() + " is back.", (*ii)->name());
				}
			}
		}
	}
}

void MaraApplication::sentMessage(Mara::MUser *pUser, const QString &pText)
{
	MaraChatDisplay	*messageWindow = _window->getMessageWidget(pUser->serverToken(), false, false);

	if(messageWindow)
	{
		emit appendHtml(messageWindow, MaraStaticData::chatToHtml(_client->user().name(), pText, _client->user().style()));
		if(_fileLogging) _fileLogger.log("<" + pUser->simpleName() + "> " + pText, pUser->simpleName());
	}
	else
	{
		messageWindow = _window->currentChannelWidget();
		emit appendHtml(messageWindow, MaraStaticData::messageToHtml(pUser->name(), pText, true, _client->user().style()));
		if(_fileLogging) _fileLogger.log("[" + pUser->simpleName() + "] " + pText, pUser->simpleName());
		MaraStaticData::setReplyUser(pUser);
	}
}

void MaraApplication::userMessage(Mara::MUser *pUser, const QString &pText)
{
	MaraMessageWindow *messageWindow = _window->getMessageWidget(pUser->serverToken(), true, false);

	if(messageWindow)
	{
		if(messageWindow != qobject_cast<MaraMessageWindow*>(QApplication::activeWindow()))
		{
			messageWindow->getAttention();
		}
		emit appendHtml(messageWindow, MaraStaticData::chatToHtml(pUser->name(), pText, pUser->style()));
		if(_fileLogging) _fileLogger.log("<" + pUser->simpleName() + "> " + pText, pUser->simpleName());
	}
	else
	{
		MaraChannelWidget *mcw = _window->currentChannelWidget();
		emit appendHtml(mcw, MaraStaticData::messageToHtml(pUser->name(), pText, false, pUser->style()));
		if(_fileLogging) _fileLogger.log("[" + pUser->simpleName() + "] " + pText, pUser->simpleName());
		MaraStaticData::setReplyUser(pUser);
	}
}

void MaraApplication::userChangedName(QString pOldName, QString pNewName)
{
	Mara::MLogger::log(Mara::LogLevel::Event, QString("User %1 changed name to %2.").arg(pOldName).arg(pNewName));
}

void MaraApplication::clientStatusChanged(MaraClient::ClientStatus pStatus)
{
	if(pStatus == MaraClient::Client_LostConnection)
	{
		Mara::MLogger::log(Mara::LogLevel::AppStatus, "Lost connection.  Reconnecting in 10 seconds...");
		reconnect();
	}
	else if(pStatus == MaraClient::Client_Kicked)
	{
		Mara::MLogger::log(Mara::LogLevel::AppStatus, "Not attempting reconnection due to kick.");
	}
}

void MaraApplication::reconnect()
{
	_reconnectTimer.start();
}

void MaraApplication::stopReconnect()
{
	if(_reconnectTimer.isActive())
	{
		Mara::MLogger::log(Mara::LogLevel::Event, "Aborting automatic reconnection attempts.");
		_reconnectTimer.stop();
	}	
}

///////////////////////
// Command Functions //
///////////////////////
void MaraApplication::emote(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser)
{
	if(pChannel == 0)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Attempted to send emote with no channel."));
		return;
	}

	_client->sendEmote(pChannel, pArgString);

	if(_client->isConnected() && MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::LocalEcho))
	{
		MaraChannelWidget *mcw;
		if(mcw = MaraChannelWidget::getChannelWidget(pChannel))
		{
			emit appendHtml(mcw, MaraStaticData::emoteToHtml(_client->user().name(), pArgString, _client->user().style()));
			if(_fileLogging) _fileLogger.log("* " + _client->user().simpleName() + " " + pArgString, _client->user().simpleName());
		}
	}

	Q_UNUSED(pArgs);
	Q_UNUSED(pUser);
}

void MaraApplication::away(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser)
{
	_client->sendAway(pArgString);
	Q_UNUSED(pArgs);
	Q_UNUSED(pChannel);
	Q_UNUSED(pUser);
}

void MaraApplication::name(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser)
{
	QString error = "";
	if(!Mara::MUser::validateUserName(pArgString, error))
	{
		Mara::MLogger::log(Mara::LogLevel::Error, "Error changing name: " + error);
		return;
	}

	MaraClientSettings::settings().save(MaraClientSettings::User::Name, pArgString);
	_client->user().setName(MaraClientSettings::settings().load<QString>(MaraClientSettings::User::Name));

	_client->sendUserUpdate();
	Q_UNUSED(pArgs);
	Q_UNUSED(pChannel);
	Q_UNUSED(pUser);
}

void MaraApplication::join(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser)
{
	_client->joinChannel(pArgString);
	Q_UNUSED(pArgs);
	Q_UNUSED(pChannel);
	Q_UNUSED(pUser);
}

void MaraApplication::leave(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser)
{
	if(pChannel->name() == "Chat")
	{
		Mara::MLogger::log(Mara::LogLevel::Error, "You may not leave the default Chat channel.");
		return;
	}

	_client->leaveChannel(pChannel->token());
	Q_UNUSED(pArgs);
	Q_UNUSED(pArgString);
	Q_UNUSED(pUser);
}

void MaraApplication::message(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser)
{
	for(MaraClient::Client::UserIterator ii = _client->users().begin(); ii != _client->users().end(); ++ii)
	{
		if(pArgString.startsWith((*ii)->simpleName() + " ", Qt::CaseInsensitive))
		{
			_client->sendMessage(*ii, pArgString.mid((*ii)->simpleName().length() + 1));
			return;
		}
	}

	Mara::MLogger::log(Mara::LogLevel::Error, "Unknown user:\n" + pArgString);

	Q_UNUSED(pArgs);
	Q_UNUSED(pChannel);
	Q_UNUSED(pUser);
}

void MaraApplication::style(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser)
{
	MaraChannelWidget *mcw = _window->currentChannelWidget();

	if(mcw)
	{
		emit appendHtml(mcw, MaraStaticData::plainToHtml(MaraStaticData::styleSheet(), false));
	}

	Q_UNUSED(pArgs);
	Q_UNUSED(pArgString);
	Q_UNUSED(pChannel);
	Q_UNUSED(pUser);
}

void MaraApplication::serverCommand(const QString& pCommand, const QString& pArgString)
{
	_client->sendServerCommand(pCommand, pArgString);
}

void MaraApplication::heartbeat()
{
	if(!_client->isConnected())
	{
		_heartbeatTimer.stop();
		return;
	}

	if(!_pingReceived)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, "Lost server heartbeat.");
		_client->disconnect();
		reconnect();
	}
	else
	{
		_pingReceived = false;
		_client->sendPing();
	}
}

void MaraApplication::pingReceived()
{
	//Mara::MLogger::log(Mara::LogLevel::Debug, "Pinged!");
	_pingReceived = true;
}

void MaraApplication::motd(QString pMotd)
{
	MaraChannelWidget *mcw = _window->currentChannelWidget();

	if(mcw)
	{
		emit appendHtml(mcw, MaraStaticData::plainToHtml(pMotd, false));
	}
}

void MaraApplication::serverMessage(QString pMessage)
{
	MaraChannelWidget *mcw = _window->currentChannelWidget();

	if(mcw)
	{
		Mara::MLogger::log(Mara::LogLevel::Event, pMessage);
	}
}

void MaraApplication::update()
{
#ifdef Q_OS_WIN32
	QProcess::startDetached("MaraUpdater.exe autorestart");
	exit();
#endif
}

void MaraApplication::updateRequired()
{
	Mara::MLogger::log(Mara::LogLevel::AppStatus, "Update is required.  Launching updater in 10 seconds.");

	QTimer::singleShot(10000, this, SLOT(update()));
}
