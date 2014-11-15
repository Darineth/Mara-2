#include "MaraConnectionHandler.h"

#include "MLogger.h"

#include "MaraServerSettings.h"
#include "MaraClientHandler.h"


MaraConnectionHandler::MaraConnectionHandler(Mara::MSettings *pSettings, Mara::MPacketHandler *pPacketHandler, QObject *pParent) :
		QObject(pParent),
		_packetHandler(pPacketHandler),
		_server(new Mara::MTcpServer(this))
{
	setObjectName("MaraConnectionHandler");

	if(!_server->listen(QHostAddress::Any, pSettings->load<quint16>(MaraServerSettings::Server::Port)))
	{
		qCritical("Error listening on socket %d: %s", _server->serverPort(), qPrintable(_server->errorString()));
		//throw new Mara::MException(__FUNCTION_NAME__, QString("Error listening on socket %1: %2").arg(_server->serverPort()).arg(_server->errorString()));
		return;
	}

	connect(_server, SIGNAL(newConnection()), this, SLOT(handleConnection()), Qt::QueuedConnection);
	Mara::MLogger::log(Mara::LogLevel::AppStatus, QString("Listening on port %1.").arg(_server->serverPort()));
}

MaraConnectionHandler::~MaraConnectionHandler()
{
	shutdown();
}

//void MaraConnectionHandler::run()
//{
//	Mara::MTcpServer server;
//	ServerHandler handler(this);
//	_server = &server;
//	if(!_server->listen(QHostAddress::Any, _settings->load<quint16>(MaraServerSettings::Server::Port)))
//	{
//		qCritical("Error listening on socket %d: %s", _server->serverPort(), qPrintable(_server->errorString()));
//		//throw new Mara::MException(__FUNCTION_NAME__, QString("Error listening on socket %1: %2").arg(_server->serverPort()).arg(_server->errorString()));
//		return;
//	}
//	connect(_server, SIGNAL(newConnection()), &handler, SLOT(handleConnection()));
//	Mara::MLogger::log(Mara::LogLevel::AppStatus, QString("Listening on port %1.").arg(_server->serverPort()));
//	exec();
//	_server->close();
//	_server = 0;
//}

void MaraConnectionHandler::handleConnection()
{
	//QThread *thread = QThread::currentThread();
	handleIncomingConnection(_server->nextPendingSocket());
}

void MaraConnectionHandler::shutdown()
{
	if(_server)
	{
		disconnect(_server, SIGNAL(newConnection()), this, SLOT(handleConnection()));
		_server->close();
		delete _server;
		_server = 0;
	}
}

void MaraConnectionHandler::handleIncomingConnection(int pSocketDescriptor)
{
	MaraClientHandler *clientHandler = new MaraClientHandler(pSocketDescriptor, _packetHandler, this);
	connect(clientHandler, SIGNAL(connected(MaraClientHandler*, Mara::MTcpSocket*)),
			this, SIGNAL(incomingConnection(MaraClientHandler*, Mara::MTcpSocket*)));
	connect(clientHandler, SIGNAL(disconnected(MaraClientHandler*, Mara::MTcpSocket*)),
			this, SIGNAL(closingConnection(MaraClientHandler*, Mara::MTcpSocket*)));
	clientHandler->start();
}

void MaraConnectionHandler::clientDisconnected()
{
}

////////////////////////////

//ServerHandler::ServerHandler(MaraConnectionHandler *pParent) : QObject(), _parent(pParent)
//{
//}
//
//ServerHandler::~ServerHandler()
//{
//}
//
//void ServerHandler::handleConnection()
//{
//	_parent->handleIncomingConnection(_parent->_server->nextPendingSocket());
//}

