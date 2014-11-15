#ifndef MARACLIENTHANDLER_H
#define MARACLIENTHANDLER_H

#include <QPointer>
#include <QTcpSocket>

#include "MUser.h"
#include "MTcpSocket.h"
#include "MPacketHandler.h"

class MaraClientHandler : public QObject //: public QThread
{
	Q_OBJECT;

	enum UserConnectionStatus
	{
		ClientVersion,
		UserLogin,
		Denied,
		Accepted
	};

	public:
		MaraClientHandler(int pSocketDescriptor, Mara::MPacketHandler *pPacketHandler, QObject *pParent = 0);
		~MaraClientHandler();

		void start();

		const QString& userToken() const;
		void setUserToken(const QString &pToken);

		Mara::MUser* user();
		void associateUser(Mara::MUser *pUser);

		const UserConnectionStatus status();

		void sendPacket(const Mara::MPacket *pPacket);

		const QString clientAddress() const;

		void run();

		void disconnect();

		void acceptVersion();

		bool userDisconnected() const;
		void setUserDisconnected(bool pUserDisconnected);

	signals:
		void statusChanged(MaraClientHandler::UserConnectionStatus pStatus);
		void signalPacket(const Mara::MPacket *pPacket);
		void connected(MaraClientHandler *pHandler, Mara::MTcpSocket *pSocket);
		void disconnected(MaraClientHandler *pHandler, Mara::MTcpSocket *pSocket);
		void closeSocket();

	private slots:
		void socketDisconnected();

	private:
		void setStatus(UserConnectionStatus pStatus);

		bool _inEventLoop;
		int _socketDescriptor;
		Mara::MPacketHandler *_packetHandler;
		QPointer<Mara::MTcpSocket> _socket;
		UserConnectionStatus _status;
		QString _userToken;
		Mara::MUser *_user;
		bool _userDisconnected;

		QMutex _socketMutex;
};

#endif
