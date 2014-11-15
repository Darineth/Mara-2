#ifndef MARA_H
#define MARA_H

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QMutex>
#include <QString>

#include <iostream>

#include "MaraChannel.h"
#include "MaraChatWidget.h"
#include "MaraClient.h"
#include "MaraClientSettings.h"
#include "MaraSettingsDialog.h"
#include "MaraWindow.h"
#include "MException.h"
#include "MFileLogger.h"
#include "MLogLevel.h"
#include "MSettings.h"
#include "MaraPluginManager.h"
#include "MLogger.h"

class MaraApplication : public QApplication
{
	Q_OBJECT;

	public:
		static void init(int &argc, char **argv)
		{
			if(!_sMaraApp) _sMaraApp = new MaraApplication(argc, argv);
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
			try
			{
				return QApplication::exec();
			}
			catch(Mara::MException *ex)
			{
				QMessageBox::critical(_sMaraApp->_window, "Unhandled Exception", ex->toString());
				delete ex;
				return -1;
			}
		}

	public slots:
		void showSettings();
		void reloadSettings();
		void connect();
		void logSlot(Mara::LogLevel::Level pLevel, const QString& pText);
		void logConnectionStatus(MaraClient::ConnectionStatus pStatus);
		void logClientStatus(MaraClient::ClientStatus pStatus);
		void clientStatusChanged(MaraClient::ClientStatus pStatus);

		void channelTextEntered(Mara::MChannel *pChannel, QString pText);
		void messageTextEntered(Mara::MUser *pUser, QString pText);

		void clientConnected();
		void clientDisconnected();
		void motd(QString pMotd);
		void serverMessage(QString pMessage);
		//void kicked();

		void updateRequired();
		void update();

	signals:
		void settingsChanged();
		void updateStyleSheets();
		void appendHtml(MaraChatDisplay *pWidget, QString pHtml);

	private slots:
		void appendHtmlSlot(MaraChatDisplay *pWidget, QString pHtml);
		void stopReconnect();

		void userConnected(Mara::MUser *pUser);
		void userDisconnected(Mara::MUser *pUser);
		void userReconnected(Mara::MUser *pUser);
		void joinedChannel(Mara::MChannel *pChannel);
		void leftChannel(Mara::MChannel *pChannel);
		void userUpdated(Mara::MUser *pUser);
		void userChangedName(QString pOldName, QString pNewName);
		void userJoinedChannel(Mara::MUser *pUser, Mara::MChannel *pChannel);
		void userLeftChannel(Mara::MUser *pUser, Mara::MChannel *pChannel);
		void channelChat(Mara::MUser *pUser, Mara::MChannel *pChannel, const QString &pText);
		void channelEmote(Mara::MUser *pUser, Mara::MChannel *pChannel, const QString &pText);
		void userAway(Mara::MUser *pUser);
		void sentMessage(Mara::MUser *pUser, const QString &pText);
		void userMessage(Mara::MUser *pUser, const QString &pText);
		void pingReceived();

		void heartbeat();

	private:
		// Singleton
		static MaraApplication *_sMaraApp;

		// UI
		MaraWindow *_window;

		MaraClient::Client *_client;

		bool _guiInitialized;

		QMutex _logMutex;

		Mara::MDebugLogBuffer _debugLog;

		typedef void (MaraApplication::*CommandFunction)(QStringList, const QString&, Mara::MChannel*, Mara::MUser*);

		QMap<QString, CommandFunction> _commandMap;

		QTimer _reconnectTimer;
		QTimer _heartbeatTimer;
		bool _pingReceived;

		bool _fileLogging;
		Mara::MFileLogger _fileLogger;

		Mara::MaraPluginManager _mpmPluginManager;

		MaraApplication();
		MaraApplication(int &argc, char **argv);
		virtual ~MaraApplication();
		MaraApplication& operator =(const MaraApplication &app);

		void initializeSettings();

		friend void qtMessageHandler(QtMsgType type, const char *msg);
		static void handleQtMessage(QtMsgType type, const char *msg);
		bool notify(QObject *pReceiver, QEvent *pEvent);

		void reconnect();

		// COMMAND FUNCTIONS
		void emote(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser);
		void away(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser);
		void name(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser);
		void join(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser);
		void leave(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser);
		void message(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser);
		void style(QStringList pArgs, const QString& pArgString, Mara::MChannel* pChannel, Mara::MUser* pUser);
		void serverCommand(const QString& pCommand, const QString& pArgString);
};

void qtMessageHandler(QtMsgType type, const char *msg);

#endif
