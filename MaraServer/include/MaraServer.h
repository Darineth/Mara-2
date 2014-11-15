#ifndef MARASERVER_H
#define MARASERVER_H

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTcpServer>
#include <QTimer>
#include <QSystemTrayIcon>

#include <iostream>

#include "MLogLevel.h"
#include "MSettings.h"
#include "MException.h"
#include "MPacketFactory.h"
#include "MPacketHandler.h"

#include "MaraServerThread.h"
#include "MaraServerWindow.h"
#include "MaraClientHandler.h"
#include "MaraConnectionHandler.h"

class MaraServerApplication : private QApplication
{
	Q_OBJECT;

	public:
		static void init(int &argc, char **argv)
		{
			if(!_sMaraApp) _sMaraApp = new MaraServerApplication(argc, argv);
		}

		static void destroy()
		{
			if(_sMaraApp)
			{
				delete _sMaraApp;
				_sMaraApp = 0;
			}
		}

		static int execute()
		{
			if(!_sMaraApp->_initialized)
			{
				std::cerr << "Application did not initialize successfully.  Exiting." << std::endl;
				return -2;
			}

			try
			{
				return QApplication::exec();
			}
			catch(Mara::MException *ex)
			{
				//QMessageBox::critical(_sMaraApp->_window, "Unhandled Exception", ex->toString());
				qDebug() << "Unhandled Exception: " << ex->toString();
				delete ex;
				return -1;
			}
		}

		static MaraServerApplication& app()
		{
			return *_sMaraApp;
		}

		static Mara::MSettings& settings()
		{
			return *(_sMaraApp->_settings);
		}

		static void handleQtMessage(QtMsgType type, const char *msg);

		bool notify(QObject *pReceiver, QEvent *pEvent);

	public slots:
		void handleLog(Mara::LogLevel::Level pLevel, const QString &pText);

	private slots:
		void trayIconActivated(QSystemTrayIcon::ActivationReason pReason);

	private:
		MaraServerApplication();
		MaraServerApplication(int &argc, char **argv);
		virtual ~MaraServerApplication();
		MaraServerApplication& operator =(const MaraServerApplication &app);

		void initializeSettings();

		friend void qtMessageHandler(QtMsgType type, const char *msg);

		// Singleton
		static MaraServerApplication *_sMaraApp;
		bool _initialized;

		// Settings
		Mara::MSettings *_settings;

		MaraServerThread _server;

		QHash<unsigned int, Mara::MUser*> _users;
		QHash<unsigned int, MaraClientHandler*> _clients;

		MaraServerWindow *_window;
		QSystemTrayIcon *_trayIcon;
};

void qtMessageHandler(QtMsgType type, const char *msg);

#endif
