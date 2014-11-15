#include "MaraClientHandler.h"

#include <QMetaType>

#include "MLogger.h"

MaraClientHandler::MaraClientHandler(int pSocketDescriptor, Mara::MPacketHandler *pPacketHandler, QObject *pParent) : QObject(pParent),
		_inEventLoop(false),
		_socketDescriptor(pSocketDescriptor),
		_packetHandler(pPacketHandler),
		_socket(0),
		_status(MaraClientHandler::ClientVersion),
		_userToken(),
		_user(0),
		_userDisconnected(false),
		_socketMutex()
{
	qRegisterMetaType<MaraClientHandler::UserConnectionStatus>("MaraClientHandler::UserConnectionStatus");

	_socket = new Mara::MTcpSocket(_socketDescriptor, this);
	Mara::MLogger::log(Mara::LogLevel::Event, QString("Incoming connection from %1.").arg(clientAddress()));

	connect(_socket, SIGNAL(disconnected()),
			this, SLOT(socketDisconnected()),
			Qt::QueuedConnection);

	connect(_socket, SIGNAL(packetReceived(Mara::MPacket*)),
			_packetHandler, SLOT(handlePacket(Mara::MPacket*)),
			Qt::QueuedConnection);

	//connect(this, SIGNAL(signalPacket(const Mara::MPacket*)),
	//		_socket, SLOT(sendPacket(const Mara::MPacket*)));

	connect(this, SIGNAL(closeSocket()), _socket, SLOT(close()), Qt::DirectConnection);
}

MaraClientHandler::~MaraClientHandler()
{
	disconnect();
	//if(isRunning())
	//{
	//	disconnect();
	//	exit();
	//	wait(5000);
	//}
}

void MaraClientHandler::start()
{
	emit connected(this, _socket);

	setStatus(ClientVersion);
}

const QString& MaraClientHandler::userToken() const
{
	return _userToken;
}

void MaraClientHandler::setUserToken(const QString &pToken)
{
	_userToken = pToken;
}

Mara::MUser* MaraClientHandler::user()
{
	return _user;
}

void MaraClientHandler::associateUser(Mara::MUser *pUser)
{
	_user = pUser;
}

const MaraClientHandler::UserConnectionStatus MaraClientHandler::status()
{
	return _status;
}

void MaraClientHandler::setStatus(UserConnectionStatus pStatus)
{
	_status = pStatus;
	emit statusChanged(_status);
}

const QString MaraClientHandler::clientAddress() const
{
	if(_socket)
		return _socket->peerAddress().toString() + ":" + QString::number(_socket->peerPort());
	else
		return "0.0.0.0:0";
}

void MaraClientHandler::sendPacket(const Mara::MPacket *pPacket)
{
	QMutexLocker locker(&_socketMutex);
	if(_socket)	_socket->sendPacket(pPacket);
	//emit signalPacket(pPacket);
	//if(_inEventLoop)
	//{

	//}
}

//void MaraClientHandler::run()
//{
//	Mara::MTcpSocket socket(_socketDescriptor);
//
//	_socket = &socket;
//	Mara::MLogger::log(Mara::LogLevel::Event, QString("Incoming connection from %1.").arg(clientAddress()));
//
//	connect(_socket, SIGNAL(disconnected()),
//			this, SLOT(socketDisconnected()));
//
//	connect(_socket, SIGNAL(packetReceived(Mara::MPacket*)),
//			_packetHandler, SLOT(handlePacket(Mara::MPacket*)));
//
//	connect(this, SIGNAL(signalPacket(const Mara::MPacket*)),
//			_socket, SLOT(sendPacket(const Mara::MPacket*)));
//
//	connect(this, SIGNAL(closeSocket()), _socket, SLOT(close()), Qt::QueuedConnection);
//
//	emit connected(this, _socket);
//
//	setStatus(ClientVersion);
//
//	_inEventLoop = true;
//	exec();
//	_inEventLoop = false;
//
//	DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, "Exited ClientHandler Exec loop."));
//
//	_socket->close();
//	if(_socket->state() != QAbstractSocket::UnconnectedState && !_socket->waitForDisconnected(5000))
//		_socket->abort();
//}

void MaraClientHandler::disconnect()
{
	QMutexLocker locker(&_socketMutex);
	if(_socket)
	{
		if(_socket->state() != QAbstractSocket::UnconnectedState)
		{
			_socket->close();
		}
		delete _socket;
		_socket = 0;
	}
	//if(_socket) emit closeSocket();
}

void MaraClientHandler::acceptVersion()
{
	if(_status == ClientVersion)
	{
		setStatus(UserLogin);
	}
}


bool MaraClientHandler::userDisconnected() const
{
	return _userDisconnected;
}

void MaraClientHandler::setUserDisconnected(bool pUserDisconnected)
{
	_userDisconnected = pUserDisconnected;
}

void MaraClientHandler::socketDisconnected()
{
	//if(_inEventLoop) exit();
	emit disconnected(this, _socket);
}
