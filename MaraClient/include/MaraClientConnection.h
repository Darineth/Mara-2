#ifndef MARACLIENTCONNECTION_H
#define MARACLIENTCONNECTION_H

#include <QHostAddress>
#include <QMetaType>
#include <QPointer>
#include <QString>
#include <QTimer>

#include "MaraClientDll.h"
#include "MPacket.h"
#include "MPacketHandler.h"
#include "MTcpSocket.h"
#include "MaraUtilityDefines.h"

namespace MaraClient
{
	enum ConnectionStatus
	{
		Connection_Inactive,
		Connection_Connecting,
		Connection_Ready,
		Connection_Error
	};

	class __MARACLIENT__CLASS_DECL ClientConnection : public QThread
	{
		Q_OBJECT;

		public:
			ClientConnection(Mara::MPacketHandler *pPacketHandler, QObject *pParent = 0);
			~ClientConnection();

			ConnectionStatus status();
			QString serverAddress();

			void connect(const QString &pServerAddress, quint16 pServerPort);

			bool connected();

			const QString &socketError();

			static const QString statusToString(ConnectionStatus pStatus);

		public slots:
			void disconnect();
			void sendPacket(const Mara::MPacket* pPacket);

		signals:
			void statusChanged(MaraClient::ConnectionStatus pStatus);
			void signalSendPacket(const Mara::MPacket* pPacket);

		private slots:
			void socketDisconnected();
			void socketConnected();
			void checkConnection();
			void socketError(QAbstractSocket::SocketError pSocketError);
			void sendPacketPrivate(const Mara::MPacket* pPacket);

		private:
			void run();
			void setStatus(ConnectionStatus pStatus);

			ConnectionStatus _status;
			QString _socketError;

			QString _serverAddress;
			quint16 _serverPort;

			Mara::MPacketHandler *_packetHandler;
			QPointer<Mara::MTcpSocket> _socket;

			QPointer<QTimer> _connectTimer;

			bool _socketActive;
			bool _userDisconnected;
	};
};

#endif
