#ifndef MARACLIENT_H
#define MARACLIENT_H

#include <QTimer>
#include <QString>
#include <QPointer>

#include "MaraClientDll.h"

#include "MUser.h"
#include "MPacket.h"
#include "MLogLevel.h"
#include "MPacketHandler.h"
#include "MaraStandardPackets.h"
#include "MaraClientConnection.h"
#include "MaraUtilityDefines.h"

namespace MaraClient
{
	enum ClientStatus
	{
		Client_Inactive,
		Client_Connecting,
		Client_WaitPing,
		Client_VersionCheck,
		Client_UserLogin,
		Client_Active,
		Client_Denied,
		Client_LostConnection,
		Client_Kicked
	};

	class __MARACLIENT__CLASS_DECL Client : public QObject
	{
		Q_OBJECT;

		public:
			typedef QHash<Mara::MUser::Token, Mara::MUser*>::iterator UserIterator;
			typedef QHash<Mara::MChannel::Token, Mara::MChannel*>::iterator ChannelIterator;

			Client(QObject *pParent = 0);
			~Client();

			void connect(const QString &pServerAddress, quint16 pServerPort);

			void joinChannel(const QString &pChannel);
			void leaveChannel(Mara::MChannel::Token pToken);

			Mara::MUser& user();
			const Mara::MUser& user() const;

			void setToken(const QString &pToken);
			const QString& token() const;

			void setServerToken(const Mara::MUser::Token pServerToken);
			Mara::MUser::Token serverToken() const;

			void setAppVersion(quint32 pAppVersion);

			const ClientConnection* connection() const;
			ClientConnection* connection();

			QHash<Mara::MUser::Token, Mara::MUser*>& users();
			const QHash<Mara::MUser::Token, Mara::MUser*> users() const;

			QHash<Mara::MChannel::Token, Mara::MChannel*>& channels();

			bool canConnect();
			bool isConnected();

			Mara::MChannel* tokenToChannel(Mara::MChannel::Token pToken);

			static const QString statusToString(ClientStatus pStatus);

		public slots:
			void disconnect();
			void sendPacket(const Mara::MPacket *pPacket);
			void sendChat(Mara::MChannel *pChannel, const QString &pText);
			void sendEmote(Mara::MChannel *pChannel, const QString &pText);
			void sendAway(const QString &pText);
			void sendMessage(Mara::MUser *pToUser, const QString &pText);
			void sendServerCommand(const QString &pCommand, const QString &pArgs);
			void sendUserUpdate();
			void sendPing();

		signals:
			void statusChanged(MaraClient::ClientStatus pStatus);
			void connecting();
			void connected();
			void motd(QString pMotd);
			void disconnecting();
			void disconnected();
			void userConnected(Mara::MUser *pUser);
			void userDisconnected(Mara::MUser *pUser);
			void userReconnected(Mara::MUser *pUser);
			void userUpdated(Mara::MUser *pUser);
			void userChangedName(QString pOldName, QString pNewName);
			void joinedChannel(Mara::MChannel* pChannel);
			void leftChannel(Mara::MChannel* pChannel);
			void userJoinedChannel(Mara::MUser *pUser, Mara::MChannel *pChannel);
			void userLeftChannel(Mara::MUser *pUser, Mara::MChannel *pChannel);
			void channelChat(Mara::MUser *pUser, Mara::MChannel *pChannel, const QString &pText);
			void channelEmote(Mara::MUser *pUser, Mara::MChannel *pChannel, const QString &pText);
			void userAway(Mara::MUser *pUser);
			void sentMessage(Mara::MUser *pUser, const QString &pText);
			void userMessage(Mara::MUser *pUser, const QString &pText);
			void pingReceived();
			void kicked();
			void updateRequired();
			void serverMessage(const QString &pText);

		private slots:
			void connectionStatusChanged(MaraClient::ConnectionStatus pStatus);

		private:
			void setStatus(MaraClient::ClientStatus pStatus);
			void clientDisconnected();
			bool notifyNotConnected();
			void requestUnknownUser(Mara::MUser::Token pToken);

			Mara::MPacketHandler _packetHandler;
			ClientConnection _connection;

			MaraClient::ClientStatus _status;

			Mara::MUser _user;
			QString _token;

			quint32 _appVersion;

			QHash<Mara::MUser::Token, Mara::MUser*> _users;
			QHash<Mara::MChannel::Token, Mara::MChannel*> _channels;
			QMultiHash<Mara::MUser*, Mara::MChannel*> _userToChannels;

			// Packet Handlers
			void unknownPacket(Mara::MPacket *pPacket);
			void unhandledPacket(Mara::MPacket *pPacket);
			void handleResponsePacket(Mara::MPacket *pPacket);
			void userConnectPacket(Mara::MPacket *pPacket);
			void handleUserUpdate(Mara::MPacket *pPacket);
			void userDisconnectPacket(Mara::MPacket *pPacket);
			void joinChannelPacket(Mara::MPacket *pPacket);
			void leaveChannelPacket(Mara::MPacket *pPacket);
			void handleChat(Mara::MPacket *pPacket);
			void handleEmote(Mara::MPacket *pPacket);
			void handleAway(Mara::MPacket *pPacket);
			void handleKick(Mara::MPacket *pPacket);
			void handleMessage(Mara::MPacket *pPacket);
			void handlePing(Mara::MPacket *pPacket);
	};
};

#endif
