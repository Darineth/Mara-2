#ifndef MARASERVERTHREAD_H
#define MARASERVERTHREAD_H

#include <QHash>
#include <QList>
#include <QMultiHash>
#include <QPointer>
#include <QThread>
#include <QWaitCondition>

#include "MChannel.h"
#include "MLogLevel.h"
#include "MPacketHandler.h"
#include "MRandom.h"
#include "MSettings.h"

#include "MaraConnectionHandler.h"

class MaraServerThread : public QThread
{
	Q_OBJECT;

	public:
		MaraServerThread(Mara::MSettings *pSettings, QObject *pParent = 0);
		~MaraServerThread();

		void init();

		void run();

	signals:
		void retryPacket(Mara::MPacket *pPacket);

	private slots:
		void handleIncomingConnection(MaraClientHandler *pHandler, Mara::MTcpSocket *pSocket);
		void handleDisconnectingConnection(MaraClientHandler *pHandler, Mara::MTcpSocket *pSocket);

	private:
		void registerPacketTypes();

		Mara::MUser *addUser(MaraClientHandler *pHandler, const QString &pName, const QFont &pFont, const QColor &pColor);
		Mara::MChannel *getChannel(const QString &pName);

		bool userInChannel(MaraClientHandler *pHandler, Mara::MUser::Token pUserToken, Mara::MChannel::Token pChannelToken);

		// Packet Handling
		void unhandledPacket(Mara::MPacket *pPacket);
		void unknownPacket(Mara::MPacket *pPacket);

		void handleTestPacket(Mara::MPacket *pPacket);
		void handleClientVersionCheck(Mara::MPacket *pPacket);
		void handleUserLogin(Mara::MPacket *pPacket);
		void handleUserUpdate(Mara::MPacket *pPacket);
		void handleUserDisconnect(Mara::MPacket *pPacket);
		void handleJoinChannel(Mara::MPacket *pPacket);
		void handleLeaveChannel(Mara::MPacket *pPacket);
		void handleChat(Mara::MPacket* pPacket);
		void handleEmote(Mara::MPacket* pPacket);
		void handleAway(Mara::MPacket* pPacket);
		void handleMessage(Mara::MPacket* pPacket);
		void handlePing(Mara::MPacket* pPacket);
		void handleServerCommand(Mara::MPacket* pPacket);
		void handleUnknownUser(Mara::MPacket* pPacket);

		// Connection Handler
		MaraConnectionHandler *_connectionHandler;
		QPointer<Mara::MPacketHandler> _packetHandler;

		Mara::MSettings *_settings;

		Mara::MersenneRNG _rng;

		//QMutex _socketMutex;
		//QWaitCondition _socketWait;

		QHash<Mara::MTcpSocket*, MaraClientHandler*> _socketToHandler;
		QHash<Mara::MUser::Token, MaraClientHandler*> _tokenToHandler;
		QHash<QString, MaraClientHandler*> _userTokenToHandler;
		QHash<QString, MaraClientHandler*> _nameToHandler;
		QHash<Mara::MUser::Token, Mara::MUser*> _users;

		QHash<QString, Mara::MChannel::Token> _nameToChannel;
		QHash<Mara::MChannel::Token, Mara::MChannel*> _tokenToChannel;
		QMultiHash<Mara::MUser::Token, Mara::MChannel*> _userToChannels;
};

#endif
