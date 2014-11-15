#ifndef MARACONNECTIONHANDLER_H
#define MARACONNECTIONHANDLER_H

#include <QList>
#include <QThread>
#include <QHostAddress>

#include "MLogLevel.h"
#include "MSettings.h"
#include "MTcpServer.h"
#include "MPacketHandler.h"
#include "MaraClientHandler.h"
#include "MaraStandardPackets.h"
#include "MaraStandardPacketTypes.h"

class MaraConnectionHandler : public QObject
{
	Q_OBJECT;
	friend class ServerHandler;

	public:
		MaraConnectionHandler(Mara::MSettings *pSettings, Mara::MPacketHandler *pPacketHandler, QObject *pParent = 0);
		~MaraConnectionHandler();

		//void run();

		void shutdown();

	public slots:

	signals:
		void incomingConnection(MaraClientHandler *pHandler, Mara::MTcpSocket *pSocket);
		void closingConnection(MaraClientHandler *pHandler, Mara::MTcpSocket *pSocket);
		//void error(Mara::MException *pEx);

	private slots:
		void clientDisconnected();
		void handleConnection();

	private:
		void handleIncomingConnection(int pSocketDescriptor);
		
		Mara::MPacketHandler *_packetHandler;
		Mara::MTcpServer *_server;
};

//class ServerHandler : public QObject
//{
//	Q_OBJECT;
//
//	public:
//		ServerHandler(MaraConnectionHandler *pParent);
//		~ServerHandler();
//
//	public slots:
//		void handleConnection();
//
//	signals:
//		void incomingConnection(QTcpServer *pSocket);
//
//	private:
//		MaraConnectionHandler *_parent;
//};


#endif
