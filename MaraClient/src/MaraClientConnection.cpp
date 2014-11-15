#include "MaraClientConnection.h"

#include "MLogger.h"

namespace MaraClient
{
	ClientConnection::ClientConnection(Mara::MPacketHandler *pPacketHandler, QObject *pParent) : QThread(pParent),
		_status(Connection_Inactive),
		_socketError(),
		_serverAddress(),
		_serverPort(0),
		_packetHandler(pPacketHandler),
		_socket(0),
		_connectTimer(0),
		_socketActive(false),
		_userDisconnected(false)
	{
		qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
		qRegisterMetaType<MaraClient::ConnectionStatus>("MaraClient::ConnectionStatus");

		//QObject::connect(this, SIGNAL(signalSendPacket(const Mara::MPacket*)), 
		//				 this, SLOT(sendPacketPrivate(const Mara::MPacket*)));
	}

	ClientConnection::~ClientConnection()
	{
		if(isRunning())
		{
			exit();
			wait();
		}
	}

	const QString ClientConnection::statusToString(ConnectionStatus pStatus)
	{
		switch(pStatus)
		{
			case Connection_Inactive:
				return tr("Inactive");
			case Connection_Connecting:
				return tr("Connecting");
			case Connection_Ready:
				return tr("Ready");
			case Connection_Error:
				return tr("Error");
		}
		return tr("INVALID_STATUS");
	}

	ConnectionStatus ClientConnection::status()
	{
		return _status;
	}

	QString ClientConnection::serverAddress()
	{
		return QString("%1:%2").arg(_serverAddress).arg(_serverPort);
	}

	void ClientConnection::connect(const QString &pServerAddress, quint16 pServerPort)
	{
		if(isRunning()) { throw new Mara::MException(__FUNCTION_NAME__, "Error: Attempt to connect on connected ClientConnection."); }
		_serverAddress = pServerAddress;
		_serverPort = pServerPort;
		DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, "Starting ClientConnection Thread."));
		start(QThread::HighPriority);
	}

	void ClientConnection::disconnect()
	{
		_userDisconnected = true;
		exit();
	}

	bool ClientConnection::connected()
	{
		return _socketActive;
	}

	void ClientConnection::sendPacket(const Mara::MPacket* pPacket)
	{
		if(!_socketActive) throw new Mara::MException(__FUNCTION_NAME__, "Error: Attempted to send packet to unconnected socket.");

		emit signalSendPacket(pPacket);
	}

	void ClientConnection::sendPacketPrivate(const Mara::MPacket* pPacket)
	{
		if(_socket)
			_socket->sendPacket(pPacket);
		else
			delete pPacket;
	}

	void ClientConnection::run()
	{
		DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, QString("Creating connection socket.")));
		setStatus(MaraClient::Connection_Inactive);
		_userDisconnected = false;

		Mara::MTcpSocket socket;
		_socket = &socket;

		_socketError = "";

		_connectTimer = new QTimer(&socket);

		QObject::connect(_connectTimer, SIGNAL(timeout()), this, SLOT(checkConnection()));
		_connectTimer->setSingleShot(true);

		QObject::connect(_socket, SIGNAL(connected()), this, SLOT(socketConnected()));
		QObject::connect(_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
		QObject::connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
		QObject::connect(this, SIGNAL(signalSendPacket(const Mara::MPacket*)), 
						 _socket, SLOT(sendPacket(const Mara::MPacket*)));
		QObject::connect(_socket, SIGNAL(packetReceived(Mara::MPacket*)),
						 _packetHandler, SLOT(handlePacket(Mara::MPacket*)));

		DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, QString("Connecting to %1:%2.").arg(_serverAddress).arg(_serverPort)));
		setStatus(MaraClient::Connection_Connecting);
		_socket->connectToHost(_serverAddress, _serverPort);

		_connectTimer->start(10000);

		_socketActive = true;
		exec();
		_socketActive = false;
		
		_socket->close();

		_serverAddress = "";
		_serverPort = 0;

		if(_connectTimer && _connectTimer->isActive())
			_connectTimer->stop();

		if(_status != MaraClient::Connection_Error)
			setStatus(MaraClient::Connection_Inactive);
	}

	void ClientConnection::checkConnection()
	{
		DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, "Checking connection status..."));
		if(_status != MaraClient::Connection_Ready)
		{
			if(_socket)
			{
				//Mara::MLogger::log(Mara::LogLevel::Error, QString("Error connecting to server: %1.").arg(_socket->errorString()));
				setStatus(MaraClient::Connection_Error);
				exit();
			}
			else
			{
				DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, "Socket destroyed while waiting for connection."));
				setStatus(MaraClient::Connection_Inactive);
				exit();
			}
		}
	}

	void ClientConnection::socketConnected()
	{
		QTimer::singleShot(0, _connectTimer, SLOT(stop()));
		setStatus(MaraClient::Connection_Ready);
	}

	void ClientConnection::setStatus(ConnectionStatus pStatus)
	{
		if(_status != pStatus)
		{
			_status = pStatus;
			emit statusChanged(_status);
		}
	}

	void ClientConnection::socketDisconnected()
	{
		DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, "Socket disconnected."));
		exit();
	}

	void ClientConnection::socketError(QAbstractSocket::SocketError pSocketError)
	{
		switch(pSocketError)
		{
			case QAbstractSocket::ConnectionRefusedError:
				_socketError = "Connection Refused";
				break;
			case QAbstractSocket::RemoteHostClosedError:
				_socketError = "Remote Host Closed Connection";
				break;
			case QAbstractSocket::HostNotFoundError:
				_socketError = "Host Not Found";
				break;
			case QAbstractSocket::SocketAccessError:
				_socketError = "Error Accessing Socket";
				break;
			case QAbstractSocket::SocketResourceError:
				_socketError = "Socket Resource Error";
				break;
			case QAbstractSocket::SocketTimeoutError:
				_socketError = "Timeout";
				break;
			case QAbstractSocket::DatagramTooLargeError:
				_socketError = "Datagram Too Large";
				break;
			case QAbstractSocket::NetworkError:
				_socketError = "Network Error";
				break;
			case QAbstractSocket::AddressInUseError:
				_socketError = "Address already in use";
				break;
			case QAbstractSocket::SocketAddressNotAvailableError:
				_socketError = "Address Not Available";
				break;
			case QAbstractSocket::UnsupportedSocketOperationError:
				_socketError = "Unsupported Socket Operation";
				break;
			case QAbstractSocket::UnfinishedSocketOperationError:
				_socketError = "Socket Operation Unfinished";
				break;
			case QAbstractSocket::ProxyAuthenticationRequiredError:
				_socketError = "Proxy Authentication Requierd";
				break;
			case QAbstractSocket::SslHandshakeFailedError:
				_socketError = "Failed SSL Handshake";
				break;
			case QAbstractSocket::UnknownSocketError:
				_socketError = "Unknown Error";
				break;
		}

		if(_status == Connection_Connecting && _connectTimer)
		{
			checkConnection();
		}
		else
		{
			setStatus(MaraClient::Connection_Error);
		}
	}

	const QString & ClientConnection::socketError()
	{
		return _socketError;
	}
}
