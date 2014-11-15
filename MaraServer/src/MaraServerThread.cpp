#include "MaraServerThread.h"

#include <QFile>
#include <QDateTime>

#include "MaraServerSettings.h"

#include "MLogger.h"

namespace MaraPackets = Mara::StandardPackets;

MaraServerThread::MaraServerThread(Mara::MSettings *pSettings, QObject *pParent) : QThread(pParent),
		_connectionHandler(0),
		_packetHandler(0),
		_settings(pSettings),
		_rng(QDateTime::currentDateTime().toTime_t()),
		_socketToHandler(),
		_tokenToHandler(),
		_userTokenToHandler(),
		_nameToHandler(),
		_users()
{
	setObjectName("MaraServerThread");
}

MaraServerThread::~MaraServerThread()
{
	if(isRunning())
	{
		exit();
		wait(5000);
	}
}

void MaraServerThread::init()
{
	registerPacketTypes();
}

void MaraServerThread::registerPacketTypes()
{
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::TestPacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::ClientVersionPacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::UserLoginPacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::UserUpdatePacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::UserDisconnectPacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::JoinChannelPacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::LeaveChannelPacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::ChatPacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::EmotePacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::AwayPacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::MessagePacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::PingPacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::ServerCommandPacket());
	Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::UnknownUserPacket());
}

void MaraServerThread::run()
{
	try
	{
		Mara::MLogger::log(Mara::LogLevel::AppStatus, QString("Starting server.  Mara Library Version %1.").arg(Mara::mLibraryVersion()));
		Mara::MPacketHandler packetHandler(new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::unhandledPacket));
		_packetHandler = &packetHandler;
		_packetHandler->registerHandler(Mara::StandardPacketTypes::UNKNOWN,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::unknownPacket));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::TEST,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleTestPacket));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::CLIENTVERSION,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleClientVersionCheck));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::USERLOGIN,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleUserLogin));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::USERUPDATE,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleUserUpdate));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::JOINCHANNEL,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleJoinChannel));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::LEAVECHANNEL,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleLeaveChannel));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::CHAT,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleChat));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::EMOTE,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleEmote));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::AWAY,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleAway));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::MESSAGE,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleMessage));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::PING,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handlePing));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::SERVERCOMMAND,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleServerCommand));
		_packetHandler->registerHandler(Mara::StandardPacketTypes::UNKNOWNUSER,
										new Mara::MHandlerCallback<MaraServerThread>(this, &MaraServerThread::handleUnknownUser));

		connect(this, SIGNAL(retryPacket(Mara::MPacket*)), _packetHandler, SLOT(handlePacket(Mara::MPacket*)));

		MaraConnectionHandler connectionHandler(_settings, _packetHandler);
		_connectionHandler = &connectionHandler;

		//QThread *tch = connectionHandler.thread();
		//QThread *tmine = this->thread();
		//QThread *current = QThread::currentThread();

		connect(_connectionHandler, SIGNAL(incomingConnection(MaraClientHandler*, Mara::MTcpSocket*)),
				this, SLOT(handleIncomingConnection(MaraClientHandler*, Mara::MTcpSocket*)), Qt::DirectConnection);
		connect(_connectionHandler, SIGNAL(closingConnection(MaraClientHandler*, Mara::MTcpSocket*)),
				this, SLOT(handleDisconnectingConnection(MaraClientHandler*, Mara::MTcpSocket*)), Qt::DirectConnection);

		Mara::MLogger::log(Mara::LogLevel::AppStatus, "Entering connection handling loop.");
		exec();
		Mara::MLogger::log(Mara::LogLevel::AppStatus, "Exited connection handling loop.");

		_connectionHandler = 0;
		_packetHandler = 0;
	}
	catch(Mara::MException *ex)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Error in MaraServerThread:\n%1").arg(ex->toString()));
		delete ex;
	}
}

void MaraServerThread::handleIncomingConnection(MaraClientHandler *pHandler, Mara::MTcpSocket *pSocket)
{
	//QThread *thread = QThread::currentThread();
	// TODO: Check for IP ban?
	//_socketMutex.lock();
	_socketToHandler.insert(pSocket, pHandler);
	pHandler->sendPacket(new MaraPackets::PingPacket(0, 0, 0, 0));
	//_socketWait.wakeAll();
	//_socketMutex.unlock();
}

void MaraServerThread::handleDisconnectingConnection(MaraClientHandler *pHandler, Mara::MTcpSocket *pSocket)
{
	DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, "Removing disconnecting client handler."));
	if(_socketToHandler.contains(pSocket))
	{
		_socketToHandler.remove(pSocket);
	}

	if(_userTokenToHandler.contains(pHandler->userToken()))
	{
		_userTokenToHandler.remove(pHandler->userToken());
	}

	//_socketWait.wakeAll();

	if(pHandler->user())
	{
		Mara::MUser *user = pHandler->user();
		_users.remove(user->serverToken());

		Mara::MLogger::log(Mara::LogLevel::Event, QString("Client disconnecting: %1").arg(user->simpleName()));

		pHandler->associateUser(0);

		if(_tokenToHandler.contains(user->serverToken()))
		{
			_tokenToHandler.remove(user->serverToken());
		}

		if(_nameToHandler.contains(user->simpleName()))
		{
			_nameToHandler.remove(user->simpleName());
		}

		Mara::MChannel *channel;
		while(channel = _userToChannels.take(user->serverToken()))
		{
			channel->removeUser(user);
			
			// TODO: send channel leave packet...or let the disconnect packet do it.
			//QList<const Mara::MUser*> channelUsers = channel->users();

			//for(QList<const Mara::MUser*>::const_iterator ii = channelUsers.begin(); ii != channelUsers.end(); ++ii)
			//{
			//	MaraClientHandler *channelHandler = _tokenToHandler.value((*ii)->serverToken());
			//	channelHandler->sendPacket(new MaraPackets::LeaveChannelPacket(user->serverToken(), channel->name(), channel->token()));
			//}
		}

		for(QHash<Mara::MUser::Token, MaraClientHandler*>::iterator ii = _tokenToHandler.begin(); ii != _tokenToHandler.end(); ++ii)
		{
			if(pHandler->userDisconnected())
				(*ii)->sendPacket(new MaraPackets::UserDisconnectPacket(user->serverToken(), "Disconnected"));
			else
				(*ii)->sendPacket(new MaraPackets::UserDisconnectPacket(user->serverToken(), "Dropped"));
		}

		delete user;
	}

	pHandler->deleteLater();
}

Mara::MUser *MaraServerThread::addUser(MaraClientHandler *pHandler, const QString &pName, const QFont &pFont, const QColor &pColor)
{
	Mara::MUser::Token token = _rng();

	// Make sure the token isn't already in use.
	while(_tokenToHandler.contains(token))
	{
		token = _rng();
	}
	_tokenToHandler.insert(token, pHandler);

	Mara::MUser *newUser = new Mara::MUser(pName, pFont, pColor, token);
	_users.insert(token, newUser);

	pHandler->associateUser(newUser);

	_nameToHandler.insert(newUser->simpleName(), pHandler);

	return newUser;
}

Mara::MChannel *MaraServerThread::getChannel(const QString &pName)
{
	QString simpleName = pName.simplified();
	if(_nameToChannel.contains(simpleName))
	{
		return _tokenToChannel.value(_nameToChannel.value(simpleName));
	}
	else
	{
		Mara::MChannel::Token token = _rng();
		while(_tokenToChannel.contains(token))
		{
			token = _rng();
		}

		Mara::MChannel *newChannel = new Mara::MChannel(simpleName, token);

		_nameToChannel.insert(simpleName, token);
		_tokenToChannel.insert(token, newChannel);

		return newChannel;
	}
}


bool MaraServerThread::userInChannel(MaraClientHandler *pHandler, Mara::MUser::Token pUserToken, Mara::MChannel::Token pChannelToken)
{
	Mara::MChannel *channel;
	if(!pHandler->user())
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User-less handler attempted to chat."));
		pHandler->sendPacket(new MaraPackets::KickPacket(0, "Unconnected user."));
		pHandler->disconnect();
		return false;
	}
	else if(pHandler->user()->serverToken() != pUserToken)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to chat with incorrect server token."));
		pHandler->sendPacket(new MaraPackets::KickPacket(pUserToken, "Incorrect server token."));
		pHandler->disconnect();
		return false;
	}
	else if(!(channel = _tokenToChannel.value(pChannelToken)))
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to chat with nonexistent channel."));
		pHandler->sendPacket(new MaraPackets::KickPacket(pUserToken, "Incorrect channel token."));
		pHandler->disconnect();
		return false;
	}
	else if(!_userToChannels.contains(pUserToken, channel))
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to chat without being a member of the target channel."));
		pHandler->sendPacket(new MaraPackets::KickPacket(pUserToken, "You are not on that channel."));
		pHandler->disconnect();
		return false;
	}

	return true;
}

void MaraServerThread::unhandledPacket(Mara::MPacket *pPacket)
{
	Mara::MLogger::log(Mara::LogLevel::Error, QString("Unhandled packet type %1: %2").arg(pPacket->type()).arg(pPacket->typeName()));
	delete pPacket;
}

void MaraServerThread::handleTestPacket(Mara::MPacket *pPacket)
{
	Mara::StandardPackets::TestPacket *packet = static_cast<Mara::StandardPackets::TestPacket*>(pPacket);
	Mara::MLogger::log(Mara::LogLevel::Debug, QString("Test Packet: %1").arg(packet->_text));

	pPacket->source()->sendPacket(new Mara::StandardPackets::TestPacket(packet->_text.toLower()));

	delete packet;
}

void MaraServerThread::unknownPacket(Mara::MPacket *pPacket)
{
	Mara::StandardPackets::UnknownPacket *packet = static_cast<Mara::StandardPackets::UnknownPacket*>(pPacket);

	Mara::MLogger::log(Mara::LogLevel::Error, QString("Unknown packet type %1 received.").arg(packet->_originalType));

	QFile packetDump("UnknownPacket_" + QString::number(packet->_originalType) + "_" + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzzz"));
	packetDump.open(QIODevice::WriteOnly);
	packetDump.write(packet->_rawData);
	packetDump.close();

	delete packet;
}

void MaraServerThread::handleClientVersionCheck(Mara::MPacket *pPacket)
{
	QPointer<Mara::MTcpSocket> socket = pPacket->source();
	Mara::StandardPackets::ClientVersionPacket *packet = static_cast<Mara::StandardPackets::ClientVersionPacket*>(pPacket);

	MaraClientHandler *handler = 0;
		
	//_socketMutex.lock();
	do 
	{
		if(socket)
			handler = _socketToHandler.value(socket, 0);
		if(!handler)
		{
			//_socketWait.wait(&_socketMutex, 5000);
		}
	}
	while(socket && !handler);
	//_socketMutex.unlock();

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Version check received for client not in socket to handler registry."));
	}
	else if(packet->_maraVersion == Mara::mLibraryVersion())
	{
		Mara::MLogger::log(Mara::LogLevel::Event, QString("Client attempting connection with version %1.%2.").arg(packet->_maraVersion).arg(packet->_clientVersion));
		handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::CLIENTVERSION, 1, ""));
	}
	else
	{
		Mara::MLogger::log(Mara::LogLevel::Event, QString("Client attempted connection with incorrect library version %1.").arg(packet->_maraVersion));
		handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::CLIENTVERSION,
														    0,
														    QString("Invalid library version %1.  Current version is %2.").arg(packet->_maraVersion).arg(Mara::mLibraryVersion())));
		handler->disconnect();
	}

	delete packet;
}

void MaraServerThread::handleUserLogin(Mara::MPacket *pPacket)
{
	Mara::StandardPackets::UserLoginPacket *packet = static_cast<Mara::StandardPackets::UserLoginPacket*>(pPacket);
	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);
	//if(!handler) throw new Mara::MException(__FUNCTION_NAME__, QString("Version check received for client not in socket to handler registry: %1.").arg((int)(void*)pPacket->source()));

	QString validationError;

	if(!Mara::MUser::validateUserToken(packet->_userToken))
	{
		// Deny login for bad token
		handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::USERLOGIN,
															0,
															QString("Invalid user token: %1.").arg(packet->_userToken)));
		handler->disconnect();
	}
	else if(!Mara::MUser::validateUserName(packet->_userName, validationError))
	{
		// Deny login for bad name
		handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::USERLOGIN,
															0,
															QString("Invalid user name: \"%1\" - %2.").arg(packet->_userName).arg(validationError)));
		handler->disconnect();
	}
	else
	{
		//quint64 token = qHash(packet->_userToken);

		if(_userTokenToHandler.contains(packet->_userToken))
		{
			// Connecting from existing client.
			Mara::MLogger::log(Mara::LogLevel::Event, QString("Reconnecting client: %1").arg(packet->_userName));

			handler->setUserToken(packet->_userToken);

			MaraClientHandler *oldHandler = _userTokenToHandler.take(packet->_userToken);
			_userTokenToHandler.insert(packet->_userToken, handler);

			Mara::MUser *reconnectedUser = oldHandler->user();
			oldHandler->associateUser(0);
			oldHandler->setUserToken("");

			if(reconnectedUser)
			{
				oldHandler->sendPacket(new MaraPackets::KickPacket(reconnectedUser->serverToken(), "Reconnected from another client."));

				_tokenToHandler[reconnectedUser->serverToken()] = handler;
				_nameToHandler.remove(reconnectedUser->simpleName());
				reconnectedUser->setName(packet->_userName);
				reconnectedUser->setFont(packet->_font);
				reconnectedUser->setColor(packet->_color);
				_nameToHandler.insert(reconnectedUser->simpleName(), handler);
				handler->associateUser(reconnectedUser);

				// TODO: Deal with the user being reconnected @ channels
				Mara::MChannel *channel;
				while(channel = _userToChannels.take(reconnectedUser->serverToken()))
				{
					channel->removeUser(reconnectedUser);
				}
			}
			else
			{
				reconnectedUser = addUser(handler, packet->_userName, packet->_font, packet->_color);
			}
			oldHandler->deleteLater();

			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::USERLOGIN, reconnectedUser->serverToken(), _settings->load<QString>(MaraServerSettings::Server::MOTD)));

			DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, "Sending reconnected client to existing clients."));

			for(QHash<Mara::MUser::Token, MaraClientHandler*>::iterator ii = _tokenToHandler.begin(); ii != _tokenToHandler.end(); ++ii)
			{
				if((*ii) != handler)
					(*ii)->sendPacket(new MaraPackets::UserConnectPacket(reconnectedUser->name(), reconnectedUser->serverToken(), reconnectedUser->font(), reconnectedUser->color(), reconnectedUser->away(), true));
			}

			DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, "Sending clients to reconnected client."));

			// Client should be in active status after this, start sending existing clients to it.
			for(QHash<Mara::MUser::Token, Mara::MUser*>::iterator ii = _users.begin(); ii != _users.end(); ++ii)
			{
				Mara::MUser* user = (*ii);
				handler->sendPacket(new MaraPackets::UserConnectPacket(user->name(), user->serverToken(), user->font(), user->color(), user->away()));
			}
		}
		else if(_nameToHandler.contains(Mara::MUser::buildSimpleName(packet->_userName)))
		{
			// Username is taken and this is not the same person, deny.
			Mara::MLogger::log(Mara::LogLevel::Event, QString("Client attempted login with duplicate name: %1 (%2).").arg(packet->_userName).arg(Mara::MUser::buildSimpleName(packet->_userName)));
			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::USERLOGIN,
																0,
																QString("Simplified user name already in use: %1.").arg(Mara::MUser::buildSimpleName(packet->_userName))));
			handler->disconnect();
		}
		else
		{
			// No duplicate username.

			Mara::MLogger::log(Mara::LogLevel::Event, QString("New client: %1").arg(packet->_userName));

			handler->setUserToken(packet->_userToken);
			_userTokenToHandler.insert(packet->_userToken, handler);

			Mara::MUser *newUser = addUser(handler, packet->_userName, packet->_font, packet->_color);

			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::USERLOGIN, newUser->serverToken(), _settings->load<QString>(MaraServerSettings::Server::MOTD)));

			DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, "Sending new client to existing clients."));

			for(QHash<Mara::MUser::Token, MaraClientHandler*>::iterator ii = _tokenToHandler.begin(); ii != _tokenToHandler.end(); ++ii)
			{
				if((*ii) != handler)
					(*ii)->sendPacket(new MaraPackets::UserConnectPacket(newUser->name(), newUser->serverToken(), newUser->font(), newUser->color(), newUser->away()));
			}

			DEBUG_ONLY(Mara::MLogger::log(Mara::LogLevel::Debug, "Sending clients to new client."));

			// Client should be in active status after this, start sending existing clients to it.
			for(QHash<Mara::MUser::Token, Mara::MUser*>::iterator ii = _users.begin(); ii != _users.end(); ++ii)
			{
				Mara::MUser* user = (*ii);
				handler->sendPacket(new MaraPackets::UserConnectPacket(user->name(), user->serverToken(), user->font(), user->color(), user->away()));
			}
		}
	}

	delete packet;
}

void MaraServerThread::handleUserUpdate(Mara::MPacket *pPacket)
{
	Mara::StandardPackets::UserUpdatePacket *packet = static_cast<Mara::StandardPackets::UserUpdatePacket*>(pPacket);

	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Handler-less socket attempted to send update."));
		delete pPacket;
		return;
	}

	if(!handler->user())
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User-less handler attempted to update."));
		handler->sendPacket(new MaraPackets::KickPacket(0, "Unconnected user."));
		handler->disconnect();
	}
	else if(handler->user()->serverToken() != packet->_userToken)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to update with incorrect server token."));
		handler->sendPacket(new MaraPackets::KickPacket(packet->_userToken, "Incorrect server token."));
		handler->disconnect();
	}

	Mara::MUser *user = handler->user();
	QString newSimpleName = Mara::MUser::buildSimpleName(packet->_userName);

	if((packet->_userName != user->name()) && _nameToHandler.contains(newSimpleName) && (_nameToHandler.value(newSimpleName) != handler))
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User %1 attempted to change name to an in-use name.").arg(user->simpleName()));
		handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::USERUPDATE, 0, QString("Simple name %1 already in use.").arg(newSimpleName)));
	}
	else
	{
		if(user->name() != packet->_userName)
		{
			_nameToHandler.remove(user->simpleName());
			_nameToHandler.insert(newSimpleName, handler);
		}

		user->setName(packet->_userName);
		user->setFont(packet->_font);
		user->setColor(packet->_color);

		//for(QHash<Mara::MUser::Token, Mara::MUser*>::iterator ii = _users.begin(); ii != _users.end(); ++ii)
		for(QHash<Mara::MUser::Token, MaraClientHandler*>::iterator ii = _tokenToHandler.begin(); ii != _tokenToHandler.end(); ++ii)
		{
			(*ii)->sendPacket(new MaraPackets::UserUpdatePacket(packet->_userName, packet->_userToken, packet->_font, packet->_color));
		}
	}

	delete packet;
}

void MaraServerThread::handleUserDisconnect(Mara::MPacket *pPacket)
{
	Mara::StandardPackets::UserDisconnectPacket *packet = static_cast<Mara::StandardPackets::UserDisconnectPacket*>(pPacket);
	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Handler-less socket attempted to disconnect."));
		delete pPacket;
		return;
	}

	handler->setUserDisconnected(true);
	handler->disconnect();

	delete packet;
}

void MaraServerThread::handleJoinChannel(Mara::MPacket *pPacket)
{
	Mara::StandardPackets::JoinChannelPacket *packet = static_cast<Mara::StandardPackets::JoinChannelPacket*>(pPacket);
	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Handler-less socket attempted to join channel."));
		delete pPacket;
		return;
	}

	if(!handler->user())
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User-less handler attempted to join channel %1.").arg(packet->_channel));
		handler->sendPacket(new MaraPackets::KickPacket(0, "Unconnected user."));
		handler->disconnect();
	}
	else if(handler->user()->serverToken() != packet->_userToken)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to join channel with incorrect server token."));
		handler->sendPacket(new MaraPackets::KickPacket(packet->_userToken, "Incorrect server token."));
		handler->disconnect();
	}
	else if(packet->_channel.size() > 30)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to join channel with too long name."));
		handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::JOINCHANNEL, 0, QString("Channel name is too long.")));
	}
	else
	{
		Mara::MChannel *channel = getChannel(packet->_channel);
		Mara::MUser *user = handler->user();

		if(channel->userInChannel(user))
		{
			Mara::MLogger::log(Mara::LogLevel::Debug, QString("User %1 attempted to join channel they are already in: %2.").arg(user->simpleName()).arg(channel->name()));
			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::JOINCHANNEL, 0, QString("You are already in channel %1.").arg(channel->name())));
		}
		else
		{
			Mara::MLogger::log(Mara::LogLevel::Event, QString("User %1 joining channel %2.").arg(user->simpleName()).arg(channel->name()));

			_userToChannels.insert(user->serverToken(), channel);
			channel->addUser(user);

			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::JOINCHANNEL, channel->token(), channel->name()));

			for(Mara::MChannel::UserIterator ii = channel->users().begin(); ii != channel->users().end(); ++ii)
			{
				MaraClientHandler *channelHandler = _tokenToHandler.value((*ii)->serverToken());
				if(channelHandler != handler)
					channelHandler->sendPacket(new MaraPackets::JoinChannelPacket(user->serverToken(), channel->name(), channel->token()));

				handler->sendPacket(new MaraPackets::JoinChannelPacket((*ii)->serverToken(), channel->name(), channel->token()));
			}
		}
	}

	delete packet;
}

void MaraServerThread::handleLeaveChannel(Mara::MPacket *pPacket)
{
	Mara::StandardPackets::LeaveChannelPacket *packet = static_cast<Mara::StandardPackets::LeaveChannelPacket*>(pPacket);
	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Handler-less socket attempted to Leave channel %1.").arg(packet->_channelToken));
		delete pPacket;
		return;
	}

	Mara::MUser *user = handler->user();

	if(!user)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User-less handler attempted to Leave channel %1.").arg(packet->_channelToken));
		handler->sendPacket(new MaraPackets::KickPacket(0, "Unconnected user."));
		handler->disconnect();
	}
	else if(user->serverToken() != packet->_userToken)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to Leave channel with incorrect server token."));
		handler->sendPacket(new MaraPackets::KickPacket(packet->_userToken, "Incorrect server token."));
		handler->disconnect();
	}
	else
	{
		Mara::MChannel *channel = _tokenToChannel.value(packet->_channelToken);

		if(!channel)
		{
			Mara::MLogger::log(Mara::LogLevel::Debug, QString("User %1 attempted to leave an unknown channel.").arg(user->simpleName()));
			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::LEAVECHANNEL, 0, QString("Channel not found.")));
		}
		else if(!channel->userInChannel(user))
		{
			Mara::MLogger::log(Mara::LogLevel::Debug, QString("User %1 attempted to Leave channel they are not in: %2.").arg(user->simpleName()).arg(channel->name()));
			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::LEAVECHANNEL, 0, QString("You are not in channel %1.").arg(channel->name())));
		}
		else
		{
			Mara::MLogger::log(Mara::LogLevel::Event, QString("User %1 leaving channel %2.").arg(user->simpleName()).arg(channel->name()));
			
			for(Mara::MChannel::UserIterator ii = channel->users().begin(); ii != channel->users().end(); ++ii)
			{
				MaraClientHandler *channelHandler = _tokenToHandler.value((*ii)->serverToken());
				if(channelHandler != handler)
				{
					channelHandler->sendPacket(new MaraPackets::LeaveChannelPacket(user->serverToken(), channel->token()));
				}
			}

			_userToChannels.remove(user->serverToken(), channel);
			channel->removeUser(user);

			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::LEAVECHANNEL, channel->token(), channel->name()));
		}
	}

	delete packet;
}

void MaraServerThread::handleChat(Mara::MPacket* pPacket)
{
	Mara::StandardPackets::ChatPacket *packet = static_cast<Mara::StandardPackets::ChatPacket*>(pPacket);
	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Handler-less socket attempted to chat."));
		delete pPacket;
		return;
	}

	Mara::MChannel *channel = _tokenToChannel.value(packet->_channelToken);

	if(userInChannel(handler, packet->_userToken, packet->_channelToken))
	{
		//for(QHash<Mara::MUser::Token, MaraClientHandler*>::iterator ii = _tokenToHandler.begin(); ii != _tokenToHandler.end(); ++ii)
		//{
		//	(*ii)->sendPacket(new MaraPackets::ChatPacket(packet->_userToken, packet->_channelToken, packet->_text));
		//}

		for(Mara::MChannel::UserIterator ii = channel->users().begin(); ii != channel->users().end(); ++ii)
		{
			_tokenToHandler.value((*ii)->serverToken())->sendPacket(new MaraPackets::ChatPacket(packet->_userToken, packet->_channelToken, packet->_text));
		}
	}

	delete packet;
}

void MaraServerThread::handleEmote(Mara::MPacket* pPacket)
{
	Mara::StandardPackets::EmotePacket *packet = static_cast<Mara::StandardPackets::EmotePacket*>(pPacket);
	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Handler-less socket attempted to send emote."));
		delete pPacket;
		return;
	}

	Mara::MChannel *channel = _tokenToChannel.value(packet->_channelToken);

	if(userInChannel(handler, packet->_userToken, packet->_channelToken))
	{
		//for(QHash<Mara::MUser::Token, MaraClientHandler*>::iterator ii = _tokenToHandler.begin(); ii != _tokenToHandler.end(); ++ii)
		//{
		//	(*ii)->sendPacket(new MaraPackets::ChatPacket(packet->_userToken, packet->_channelToken, packet->_text));
		//}

		for(Mara::MChannel::UserIterator ii = channel->users().begin(); ii != channel->users().end(); ++ii)
		{
			_tokenToHandler.value((*ii)->serverToken())->sendPacket(new MaraPackets::EmotePacket(packet->_userToken, packet->_channelToken, packet->_text));
		}
	}

	delete packet;
}

void MaraServerThread::handleAway(Mara::MPacket* pPacket)
{
	Mara::StandardPackets::AwayPacket *packet = static_cast<Mara::StandardPackets::AwayPacket*>(pPacket);
	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Handler-less socket attempted to send away."));
		delete pPacket;
		return;
	}

	Mara::MUser *user = handler->user();

	if(!user)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User-less handler attempted to send /away."));
		handler->sendPacket(new MaraPackets::KickPacket(0, "Unconnected user."));
		handler->disconnect();
	}
	else if(user->serverToken() != packet->_userToken)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to send /away with incorrect server token."));
		handler->sendPacket(new MaraPackets::KickPacket(packet->_userToken, "Incorrect server token."));
		handler->disconnect();
	}
	else
	{
		if(packet->_text.simplified() == "")
		{
			user->setAway("");
		}
		else
		{
			user->setAway(packet->_text);
		}

		for(QHash<Mara::MUser::Token, MaraClientHandler*>::iterator ii = _tokenToHandler.begin(); ii != _tokenToHandler.end(); ++ii)
		{
			(*ii)->sendPacket(new MaraPackets::AwayPacket(packet->_userToken, user->away()));
		}
	}

	delete packet;
}

void MaraServerThread::handleMessage(Mara::MPacket* pPacket)
{
	Mara::StandardPackets::MessagePacket *packet = static_cast<Mara::StandardPackets::MessagePacket*>(pPacket);
	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Handler-less socket attempted to send message."));
		delete pPacket;
		return;
	}

	Mara::MUser *user = handler->user();

	if(!user)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User-less handler attempted to send message."));
		handler->sendPacket(new MaraPackets::KickPacket(0, "Unconnected user."));
		handler->disconnect();
	}
	else if(user->serverToken() != packet->_userToken)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to send message with incorrect server token."));
		handler->sendPacket(new MaraPackets::KickPacket(packet->_userToken, "Incorrect server token."));
		handler->disconnect();
	}
	else
	{
		if(_tokenToHandler.contains(packet->_toUserToken))
		{
			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::MESSAGE, packet->_toUserToken, packet->_text));

			_tokenToHandler.value(packet->_toUserToken)->sendPacket(new MaraPackets::MessagePacket(packet->_userToken, packet->_toUserToken, packet->_text));
		}
		else
		{
			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::MESSAGE, 0, "Unknown user."));
		}
	}

	delete packet;
}

void MaraServerThread::handlePing(Mara::MPacket* pPacket)
{
	Mara::StandardPackets::PingPacket *packet = static_cast<Mara::StandardPackets::PingPacket*>(pPacket);
	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Handler-less socket attempted to send ping."));
		delete pPacket;
		return;
	}

	Mara::MUser *user = handler->user();

	if(!user)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User-less handler attempted to send ping."));
		handler->sendPacket(new MaraPackets::KickPacket(0, "Unconnected user."));
		handler->disconnect();
	}
	else if(user->serverToken() != packet->_userToken)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to send ping with incorrect server token."));
		handler->sendPacket(new MaraPackets::KickPacket(packet->_userToken, "Incorrect server token."));
		handler->disconnect();
	}
	else
	{
		handler->sendPacket(new MaraPackets::PingPacket(packet->_userToken, packet->_pingId, packet->_sentAt, packet->_lastPing));
	}

	delete packet;
}

void MaraServerThread::handleServerCommand(Mara::MPacket* pPacket)
{
	Mara::StandardPackets::ServerCommandPacket *packet = static_cast<Mara::StandardPackets::ServerCommandPacket*>(pPacket);
	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Handler-less socket attempted to send server command."));
		delete pPacket;
		return;
	}

	Mara::MUser *user = handler->user();

	if(!user)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User-less handler attempted to send command."));
		handler->sendPacket(new MaraPackets::KickPacket(0, "Unconnected user."));
		handler->disconnect();
	}
	else if(user->serverToken() != packet->_userToken)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to send ping with incorrect server token."));
		handler->sendPacket(new MaraPackets::KickPacket(packet->_userToken, "Incorrect server token."));
		handler->disconnect();
	}
	else
	{
		//handler->sendPacket(new MaraPackets::PingPacket(packet->_userToken, packet->_pingId, packet->_sentAt, packet->_lastPing));

		QString command = packet->_command.toLower();
		QString args = packet->_args;
		
		if(command == "ping")
		{
			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::SERVERCOMMAND, 1, "Pong!"));
		}
		else if(command == "hi")
		{
			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::SERVERCOMMAND, 1, "Sup?"));
		}
		else if(command == "d")
		{
			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::SERVERCOMMAND, 1, "Hello, Luf."));
		}
		else if(command == "roll")
		{
			static QRegExp rollRegex("^(p?)(-?)(\\d+)?d(\\d+)(?:-(\\d+))?$", Qt::CaseInsensitive);
			// cap 1 = private
			// cap 2 = -
			// cap 3 = X die
			// cap 4 = dX/min
			// cap 5 = max

			static Mara::MersenneRNG rng;

			if(rollRegex.exactMatch(args))
			{
				bool privateRoll = (rollRegex.cap(1) != "");
				bool showDie = (rollRegex.cap(2) != "");
				quint16 numDie = 1;
				if(rollRegex.cap(3) != "") numDie = rollRegex.cap(3).toUShort();
				quint16 sides = rollRegex.cap(4).toUShort();
				quint16 max = 0;
				if(rollRegex.cap(5) != "") max = rollRegex.cap(5).toUShort();

				//mara->net->sendRoll(mara->User.name, numDie, dX, max, showDie, privateRoll);

				if(numDie > 100) { numDie = 100; }

				QString rolling = QString("%1d%2").arg(numDie).arg(sides);

				quint32 result = 0;
				QString results = "";

				for(quint16 ii = 0; ii < numDie; ++ii)
				{
					quint32 roll = 0;
					if(max > 0)
					{
						roll = (rng() % ((max - sides) + 1)) + sides;
					}
					else if(sides > 0)
					{
						roll = (rng() % sides) + 1;
					}
					else
					{
						roll = 0;
					}

					if(showDie)
					{
						if(ii == 0)
						{
							results = QString::number(roll);
						}
						else
						{
							results += (" " + QString::number(roll));
						}
					}
					
					result += roll;
				}

				if(showDie)
				{
					results = QString::number(result) + ": " + results;
				}
				else
				{
					results = QString::number(result);
				}

				if(privateRoll)
				{
					handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::SERVERCOMMAND, 0, QString("You privately rolled: %1 => %2").arg(rolling).arg(results)));
				}
				else
				{
					QString message = QString("%1 rolled: %2 => %3").arg(handler->user()->simpleName()).arg(rolling).arg(results);
					for(QHash<Mara::MUser::Token, MaraClientHandler*>::iterator ii = _tokenToHandler.begin(); ii != _tokenToHandler.end(); ++ii)
					{
						(*ii)->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::SERVERCOMMAND, 0, message));
					}
				}
			}
			else
			{
				handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::SERVERCOMMAND, 0, QString("Format: /roll [p][-][#]d#[-#] - Rolls D&D style dice.  Begin roll with \"p\" to make the results private, \"-\" to show each die.  Ex. -5d10, p2d20").arg(command)));
			}
		}
		else
		{
			handler->sendPacket(new MaraPackets::ResponsePacket(Mara::StandardPacketTypes::SERVERCOMMAND, 0, QString("Unknown Command: %1").arg(command)));
		}
	}

	delete packet;
}

void MaraServerThread::handleUnknownUser(Mara::MPacket* pPacket)
{
	Mara::StandardPackets::UnknownUserPacket *packet = static_cast<Mara::StandardPackets::UnknownUserPacket*>(pPacket);
	MaraClientHandler *handler = _socketToHandler.value(pPacket->source(), 0);

	if(!handler)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Handler-less socket attempted to request unknown users."));
		delete pPacket;
		return;
	}

	Mara::MUser *user = handler->user();

	if(!user)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User-less handler attempted to request unknown user."));
		handler->sendPacket(new MaraPackets::KickPacket(0, "Unconnected user."));
		handler->disconnect();
	}
	else if(user->serverToken() != packet->_userToken)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("User attempted to send ping with incorrect server token."));
		handler->sendPacket(new MaraPackets::KickPacket(packet->_userToken, "Incorrect server token."));
		handler->disconnect();
	}
	else
	{
		Mara::MUser *unknown = this->_users.value(packet->_unknownToken);
		if(!unknown)
		{
			Mara::MLogger::log(Mara::LogLevel::Error, QString("%1 requested unknown unknown user.").arg(user->simpleName()));
		}
		else
		{
			Mara::MLogger::log(Mara::LogLevel::Event, QString("Sending unknown user to %1.").arg(user->simpleName()));
			handler->sendPacket(new MaraPackets::UserConnectPacket(unknown->name(), unknown->serverToken(), unknown->font(), unknown->color(), unknown->away()));

			QList<Mara::MChannel*> channels = _userToChannels.values(unknown->serverToken());
			for(QList<Mara::MChannel*>::iterator ii = channels.begin(); ii != channels.end(); ++ii)
			{
				Mara::MChannel* channel = (*ii);
				if(channel->userInChannel(user->serverToken()))
				{
					handler->sendPacket(new MaraPackets::JoinChannelPacket(unknown->serverToken(), channel->name(), channel->token()));
				}
			}
		}
	}

	delete packet;
}
