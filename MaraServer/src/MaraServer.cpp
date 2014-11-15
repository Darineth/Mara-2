#include "MaraServer.h"
#include "MaraServerSettings.h"

#include "MaraStandardPackets.h"

#include "MLogger.h"

MaraServerApplication* MaraServerApplication::_sMaraApp;

MaraServerApplication::MaraServerApplication(int &argc, char **argv) : QApplication(argc, argv),
	_initialized(false),
	_settings(new Mara::MSettings("./MaraServer.conf", this)),
	_server(_settings),
	_users(),
	_clients(),
	_window(new MaraServerWindow),
	_trayIcon(0)
{
	setObjectName("MaraServerApplication");
	qInstallMsgHandler(qtMessageHandler);
	qRegisterMetaType<Mara::LogLevel::Level>("Mara::LogLevel::Level");

	connect(Mara::MLogger::logger(), SIGNAL(logged(Mara::LogLevel::Level, const QString&)),
			this, SLOT(handleLog(Mara::LogLevel::Level, const QString&)), 
			Qt::QueuedConnection);

	_window->show();

	connect(_window->qaExit, SIGNAL(triggered()), this, SLOT(quit()));

	//connect(&_server, SIGNAL(log(Mara::LogLevel::Level, const QString&)),
	//		this, SLOT(handleLog(Mara::LogLevel::Level, const QString&)), 
	//		Qt::DirectConnection);

	connect(&_server, SIGNAL(finished()), this, SLOT(quit()));

	try
	{
		initializeSettings();
		_server.init();
		_server.start();
		_initialized = true;

		if(QSystemTrayIcon::isSystemTrayAvailable())
		{
			_trayIcon = new QSystemTrayIcon(QIcon(":/images/MaraServer.ico"), this);
			connect(_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

			// TODO: Don't show here, wait until window is minimized.
			_trayIcon->show();
		}
	}
	catch(Mara::MException *ex)
	{
		handleLog(Mara::LogLevel::Error, QString("Error initializing server application: %1").arg(ex->toString()));
		delete ex;
	}
}

MaraServerApplication::~MaraServerApplication()
{
	_server.quit();
	if(_trayIcon)
	{
		_trayIcon->hide();
		delete _trayIcon;
	}

	delete _window;
	_window = 0;

	_settings->sync();
	delete _settings;
}

void MaraServerApplication::initializeSettings()
{
	_settings->setDefault(MaraServerSettings::Server::MOTD, QString("No MOTD Set"));
	_settings->setDefault(MaraServerSettings::Server::Name, QString("Mara 2 Server"));
	_settings->setDefault(MaraServerSettings::Server::Port, (quint16)5051);
}

void qtMessageHandler(QtMsgType type, const char *msg)
{
	MaraServerApplication::handleQtMessage(type, msg);
}

void MaraServerApplication::handleQtMessage(QtMsgType type, const char *msg)
{
	switch(type)
	{
		case QtDebugMsg:
			DEBUG_ONLY(std::cout << "[DEBUG]    " << msg << std::endl << std::flush);
			//DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, msg));
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

bool MaraServerApplication::notify(QObject *pReceiver, QEvent *pEvent)
{
	try
	{
		return QApplication::notify(pReceiver, pEvent);
	}
	catch(Mara::MException *ex)
	{
		QMessageBox::critical(0, "Event Handler Exception", ex->toString());
		//std::cout << "[EVENT HANDLER EXCEPTION] " << qPrintable(ex->toString());
		handleLog(Mara::LogLevel::Error, "EXCEPTION CAUGHT FROM EVENT HANDLER: " + ex->toString());
		throw;
		//delete ex;
		return false;
	}
}

void MaraServerApplication::handleLog(Mara::LogLevel::Level pLevel, const QString &pText)
{
	//std::cout << "[MARA LOG] " << qPrintable(pText) << std::endl << std::flush;
	switch(pLevel)
	{
		case Mara::LogLevel::Error:
			if(_window) _window->log(pLevel, "ERROR: " + pText);
			break;
		default:
			if(_window) _window->log(pLevel, pText);
			break;
	}
}

void MaraServerApplication::trayIconActivated(QSystemTrayIcon::ActivationReason pReason)
{
	Q_UNUSED(pReason);
	// TODO: Show the window!
	if(_window)
	{
		_window->restore();
		_window->setFocus();
		_window->activateWindow();
		_window->raise();
	}
}
