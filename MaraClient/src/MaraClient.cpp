#include "MaraClient.h"

#include <QMetaType>

#include "MaraUtilityDefines.h"
#include "MLogger.h"

namespace MaraClient
{
	Client::Client(QObject *pParent) : QObject(pParent), 
		_packetHandler(new Mara::MHandlerCallback<Client>(this, &Client::unhandledPacket)),
		_connection(&_packetHandler, this),
		_status(Client_Inactive),
		_user("UnnamedUser", QFont("Arial", 10), QColor(Qt::white)),
		_token(),
		_appVersion(-1)
	{
		qRegisterMetaType<MaraClient::ClientStatus>("MaraClient::ClientStatus");
		qRegisterMetaType<MaraClient::ConnectionStatus>("MaraClient::ConnectionStatus");

		QObject::connect(&_connection, SIGNAL(statusChanged(MaraClient::ConnectionStatus)),
						 this, SLOT(connectionStatusChanged(MaraClient::ConnectionStatus)));

		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::ResponsePacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::UserConnectPacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::UserUpdatePacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::UserDisconnectPacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::JoinChannelPacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::LeaveChannelPacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::ChatPacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::EmotePacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::AwayPacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::MessagePacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::KickPacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::PingPacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::ServerCommandPacket());
		Mara::MPacketFactory::registerPacketType(new Mara::StandardPackets::UnknownUserPacket());

		_packetHandler.registerHandler(Mara::StandardPacketTypes::UNKNOWN,
									   new Mara::MHandlerCallback<Client>(this, &Client::unknownPacket));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::RESPONSE,
									   new Mara::MHandlerCallback<Client>(this, &Client::handleResponsePacket));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::USERCONNECT,
									   new Mara::MHandlerCallback<Client>(this, &Client::userConnectPacket));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::USERUPDATE,
									   new Mara::MHandlerCallback<Client>(this, &Client::handleUserUpdate));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::USERDISCONNECT,
									   new Mara::MHandlerCallback<Client>(this, &Client::userDisconnectPacket));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::JOINCHANNEL,
									   new Mara::MHandlerCallback<Client>(this, &Client::joinChannelPacket));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::LEAVECHANNEL,
									   new Mara::MHandlerCallback<Client>(this, &Client::leaveChannelPacket));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::CHAT,
									   new Mara::MHandlerCallback<Client>(this, &Client::handleChat));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::EMOTE,
									   new Mara::MHandlerCallback<Client>(this, &Client::handleEmote));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::AWAY,
									   new Mara::MHandlerCallback<Client>(this, &Client::handleAway));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::MESSAGE,
									   new Mara::MHandlerCallback<Client>(this, &Client::handleMessage));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::KICK,
									   new Mara::MHandlerCallback<Client>(this, &Client::handleKick));
		_packetHandler.registerHandler(Mara::StandardPacketTypes::PING,
									   new Mara::MHandlerCallback<Client>(this, &Client::handlePing));
	}

	Client::~Client()
	{
		if(_connection.isRunning())
		{
			_connection.disconnect();
			_connection.wait();
		}
	}

	bool Client::canConnect()
	{
		return (_status == Client_Inactive || _status == Client_LostConnection || _status == Client_Denied || _status == Client_Kicked) && !_connection.isRunning();
	}

	bool Client::isConnected()
	{
		return _status == Client_Active || _status == Client_Connecting;
	}

	Mara::MChannel* Client::tokenToChannel(Mara::MChannel::Token pToken)
	{
		return _channels.value(pToken);
	}

	void Client::connect(const QString &pServerAddress, quint16 pServerPort)
	{
		if(!canConnect())
		{
			Mara::MLogger::log(Mara::LogLevel::Error, QString("Connection active or in progress."));
			return;
		}

		if(_connection.connected())
		{
			Mara::MLogger::log(Mara::LogLevel::Error, QString("Already connected."));
			return;
		}

		emit connecting();
		_connection.connect(pServerAddress, pServerPort);
	}

	void Client::disconnect()
	{
		if(notifyNotConnected()) return;

		emit disconnecting();
		_connection.disconnect();
	}

	void Client::joinChannel(const QString &pChannel)
	{
		if(notifyNotConnected()) return;

		sendPacket(new Mara::StandardPackets::JoinChannelPacket(_user.serverToken(), pChannel));
	}

	void Client::leaveChannel(Mara::MChannel::Token pToken)
	{
		if(notifyNotConnected()) return;

		sendPacket(new Mara::StandardPackets::LeaveChannelPacket(_user.serverToken(), pToken));
	}

	void Client::requestUnknownUser(Mara::MUser::Token pUnknownToken)
	{
		sendPacket(new Mara::StandardPackets::UnknownUserPacket(_user.serverToken(), pUnknownToken));
	}

	const ClientConnection* Client::connection() const
	{
		return &_connection;
	}

	ClientConnection* Client::connection()
	{
		return &_connection;
	}

	void Client::setStatus(MaraClient::ClientStatus pStatus)
	{
		_status = pStatus;
		emit statusChanged(_status);
	}

	Mara::MUser& Client::user()
	{
		return _user;
	}

	const Mara::MUser& Client::user() const
	{
		return _user;
	}

	void Client::setToken(const QString &pToken)
	{
		_token = pToken;
	}

	const QString& Client::token() const
	{
		return _token;
	}

	Mara::MUser::Token Client::serverToken() const
	{
		return _user.serverToken();
	}

	void Client::setAppVersion(quint32 pAppVersion)
	{
		_appVersion = pAppVersion;
	}

	QHash<Mara::MUser::Token, Mara::MUser*>& Client::users()
	{
		return _users;
	}

	const QHash<Mara::MUser::Token, Mara::MUser*> Client::users() const
	{
		return _users;
	}

	QHash<Mara::MChannel::Token, Mara::MChannel*>& Client::channels()
	{
		return _channels;
	}

	const QString Client::statusToString(ClientStatus pStatus)
	{
		switch(pStatus)
		{
			case Client_Inactive : return "Inactive";
			case Client_Connecting : return "Connecting";
			case Client_WaitPing : return "WaitingForPing";
			case Client_VersionCheck : return "VersionCheck";
			case Client_UserLogin : return "UserLogin";
			case Client_Active : return "Active";
			case Client_Denied : return "Denied";
			case Client_LostConnection : return "LostConnection";
			case Client_Kicked : return "Kicked";
		}

		return "Invalid ClientStatus";
	}

	void Client::unknownPacket(Mara::MPacket *pPacket)
	{
		Mara::StandardPackets::UnknownPacket *packet = static_cast<Mara::StandardPackets::UnknownPacket*>(pPacket);
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Unknown packet type %1.").arg(packet->_originalType));
	}

	void Client::unhandledPacket(Mara::MPacket* pPacket)
	{
		Mara::MLogger::log(Mara::LogLevel::Error, QString("Unhandled packet type %1: %2").arg(pPacket->type()).arg(pPacket->typeName()));
	}

	void Client::connectionStatusChanged(MaraClient::ConnectionStatus pStatus)
	{
		switch(pStatus)
		{
			case MaraClient::Connection_Ready:
				// Send Version Info
				//_connection.sendPacket()
				//setStatus(Client_VersionCheck);
				//sendPacket(new Mara::StandardPackets::ClientVersionPacket(Mara::mLibraryVersion(), MaraClient::mClientVersion(), _appVersion));
				setStatus(Client_WaitPing);
				break;
			case MaraClient::Connection_Inactive:
				clientDisconnected();
				if(_status != Client_Denied)
					setStatus(Client_Inactive);
				break;
			case MaraClient::Connection_Error:
				clientDisconnected();
				if(_status != Client_Denied && _status != Client_Kicked)
					setStatus(Client_LostConnection);
				break;
			case MaraClient::Connection_Connecting:
				setStatus(Client_Connecting);
				break;
		};
	}

	void Client::sendPacket(const Mara::MPacket *pPacket)
	{
		_connection.sendPacket(pPacket);
	}

	void Client::sendChat(Mara::MChannel *pChannel, const QString &pText)
	{
		if(notifyNotConnected()) return;
		sendPacket(new Mara::StandardPackets::ChatPacket(serverToken(), pChannel->token(), pText));
	}

	void Client::sendEmote(Mara::MChannel *pChannel, const QString &pText)
	{
		if(notifyNotConnected()) return;
		sendPacket(new Mara::StandardPackets::EmotePacket(serverToken(), pChannel->token(), pText));
	}

	void Client::sendAway(const QString &pText)
	{
		if(notifyNotConnected()) return;
		sendPacket(new Mara::StandardPackets::AwayPacket(serverToken(), pText));
	}

	void Client::sendMessage(Mara::MUser *pToUser, const QString &pText)
	{
		if(notifyNotConnected()) return;
		sendPacket(new Mara::StandardPackets::MessagePacket(serverToken(), pToUser->serverToken(), pText));
	}

	void Client::sendUserUpdate()
	{
		if(notifyNotConnected()) return;
		sendPacket(new Mara::StandardPackets::UserUpdatePacket(_user.name(), _user.serverToken(), _user.font(), _user.color()));
	}

	void Client::sendPing()
	{
		if(notifyNotConnected()) return;
		sendPacket(new Mara::StandardPackets::PingPacket(_user.serverToken(), 0, 0, 0));
	}

	void Client::sendServerCommand(const QString& pCommand, const QString& pArgs)
	{
		if(notifyNotConnected()) return;
		sendPacket(new Mara::StandardPackets::ServerCommandPacket(_user.serverToken(), pCommand, pArgs));
	}

	void Client::clientDisconnected()
	{
		emit disconnected();

		_userToChannels.clear();

		for(ChannelIterator ii = _channels.begin(); ii != _channels.end(); ++ii)
		{
			delete (*ii);
		}

		_channels.clear();

		for(UserIterator ii = _users.begin(); ii != _users.end(); ++ii)
		{
			delete (*ii);
		}

		_users.clear();
	}

	bool Client::notifyNotConnected()
	{
		if(!isConnected())
		{
			Mara::MLogger::log(Mara::LogLevel::Error, "Not connected.");
			return true;
		}

		return false;
	}

	void Client::handleResponsePacket(Mara::MPacket* pPacket)
	{
		Mara::StandardPackets::ResponsePacket *packet = static_cast<Mara::StandardPackets::ResponsePacket*>(pPacket);

		bool handledResponse = false;

		switch(packet->_responseType)
		{
			case Mara::StandardPacketTypes::CLIENTVERSION:
				if(_status == MaraClient::Client_VersionCheck)
				{
					if(packet->_response)
					{
						setStatus(MaraClient::Client_UserLogin);
						sendPacket(new Mara::StandardPackets::UserLoginPacket(_user.name(), _token, _user.font(), _user.color()));
					}
					else
					{
						Mara::MLogger::log(Mara::LogLevel::Error, QString("Failed version check: ") + packet->_message);
						setStatus(MaraClient::Client_Denied);
						_connection.disconnect();
						emit updateRequired();
					}
					handledResponse = true;
				}
				break;
			case Mara::StandardPacketTypes::USERLOGIN:
				if(_status == MaraClient::Client_UserLogin)
				{
					if(packet->_response)
					{
						_user.setServerToken(packet->_response);
						setStatus(MaraClient::Client_Active);
						emit connected();
						emit motd(packet->_message);
					}
					else
					{
						Mara::MLogger::log(Mara::LogLevel::Error, QString("Login failed: ") + packet->_message);
						setStatus(MaraClient::Client_Denied);
						_connection.disconnect();
					}
					handledResponse = true;
				}
				break;
			case Mara::StandardPacketTypes::JOINCHANNEL:
				if(_status == MaraClient::Client_Active)
				{
					if(packet->_response)
					{
						Mara::MChannel *newChannel = new Mara::MChannel(packet->_message, packet->_response);
						_channels.insert(packet->_response, newChannel);
						emit joinedChannel(newChannel);
					}
					else
					{
						Mara::MLogger::log(Mara::LogLevel::Error, QString("Error joining channel: %2").arg(packet->_message));
					}
					handledResponse = true;
				}
				break;
			case Mara::StandardPacketTypes::LEAVECHANNEL:
				if(_status == MaraClient::Client_Active)
				{
					if(packet->_response)
					{
						Mara::MChannel *newChannel = _channels.take(packet->_response);
						if(!newChannel)
						{
							Mara::MLogger::log(Mara::LogLevel::Error, QString("Error leaving channel: Could not find local channel."));
						}
						else	
						{
							emit leftChannel(newChannel);
							delete newChannel;
						}
					}
					else
					{
						Mara::MLogger::log(Mara::LogLevel::Error, QString("Error leaving channel: %2").arg(packet->_message));
					}
					handledResponse = true;
				}
				break;
			case Mara::StandardPacketTypes::USERUPDATE:
				if(!packet->_response)
				{
					Mara::MLogger::log(Mara::LogLevel::Error, QString("Error updating name/font/color: %1").arg(packet->_message));
				}
				handledResponse = true;
				break;
			case Mara::StandardPacketTypes::MESSAGE:
				if(packet->_response && _users.contains(packet->_response))
				{
					emit sentMessage(_users.value(packet->_response), packet->_message);
				}
				else
				{
					Mara::MLogger::log(Mara::LogLevel::Error, QString("Error sending message: %1").arg(packet->_message));
				}
				handledResponse = true;
				break;
			case Mara::StandardPacketTypes::SERVERCOMMAND:
				emit serverMessage(packet->_message);
				//Mara::MLogger::log(Mara::LogLevel::Error, packet->_message);
				handledResponse = true;
				break;
		}

		if(!handledResponse)
		{
			Mara::MLogger::log(Mara::LogLevel::Error, QString("Unexpected response packet [%1] %2: %3").arg(packet->_responseType).arg(packet->_response).arg(packet->_message));
		}

		delete packet;
	}

	void Client::userConnectPacket(Mara::MPacket *pPacket)
	{
		Mara::StandardPackets::UserConnectPacket *packet = static_cast<Mara::StandardPackets::UserConnectPacket*>(pPacket);

		if(_users.contains(packet->_userToken))
		{
			// Client re-connect?
			Mara::MUser *user = _users.value(packet->_userToken);
			emit userReconnected(user);
			user->setName(packet->_userName);
			user->setFont(packet->_font);
			user->setColor(packet->_color);
			user->style().setName(QString("USER%1").arg(packet->_userToken));
			emit userUpdated(user);
		}
		else
		{
			Mara::MUser *newUser = new Mara::MUser(packet->_userName, packet->_font, packet->_color, packet->_userToken);
			newUser->setAway(packet->_away);
			_users.insert(newUser->serverToken(), newUser);
			emit userConnected(newUser);
		}

		delete packet;
	}

	void Client::handleUserUpdate(Mara::MPacket *pPacket)
	{
		Mara::StandardPackets::UserUpdatePacket *packet = static_cast<Mara::StandardPackets::UserUpdatePacket*>(pPacket);

		Mara::MUser *user;
		if(!(user = _users.value(packet->_userToken)))
		{
			Mara::MLogger::log(Mara::LogLevel::Error, QString("Received update for unknown user %1: %2").arg(packet->_userToken).arg(packet->_userName));
			requestUnknownUser(packet->_userToken);
		}
		else
		{
			if(user->name() != packet->_userName)
			{
				emit userChangedName(user->name(), packet->_userName);
				user->setName(packet->_userName);
			}
			user->setFont(packet->_font);
			user->setColor(packet->_color);
			user->style().setName(QString("USER%1").arg(packet->_userToken));

			emit userUpdated(user);
		}

		delete packet;
	}

	void Client::userDisconnectPacket(Mara::MPacket* pPacket)
	{
		Mara::StandardPackets::UserDisconnectPacket *packet = static_cast<Mara::StandardPackets::UserDisconnectPacket*>(pPacket);

		Mara::MUser *user = _users.value(packet->_userToken);
		if(!user)
		{
			Mara::MLogger::log(Mara::LogLevel::Error, "Unknown user disconnected.");
		}
		else
		{
			_users.remove(packet->_userToken);
			Mara::MChannel *channel;
			while((channel = _userToChannels.take(user)))
			{
				channel->removeUser(user);
				emit userLeftChannel(user, channel);
			}

			emit userDisconnected(user);

			delete user;
		}

		delete packet;
	}

	void Client::joinChannelPacket(Mara::MPacket* pPacket)
	{
		Mara::StandardPackets::JoinChannelPacket *packet = static_cast<Mara::StandardPackets::JoinChannelPacket*>(pPacket);

		Mara::MUser *user = _users.value(packet->_userToken);
		Mara::MChannel *channel = _channels.value(packet->_channelToken);

		if(!user)
		{
			Mara::MLogger::log(Mara::LogLevel::Error, "Unknown user attempted to join a channel.");
			requestUnknownUser(packet->_userToken);
		}
		else if(!channel)
		{
			Mara::MLogger::log(Mara::LogLevel::Error, "User attempted to join an unknown channel.");
		}
		else
		{
			if(!channel->userInChannel(user->serverToken()))
			{
				channel->addUser(user);

				if(!_userToChannels.contains(user, channel))
				{			
					_userToChannels.insert(user, channel);
				}
				emit userJoinedChannel(user, channel);
			}
			if(user->away() != "") emit userAway(user);
		}

		delete packet;
	}

	void Client::leaveChannelPacket(Mara::MPacket* pPacket)
	{
		Mara::StandardPackets::LeaveChannelPacket *packet = static_cast<Mara::StandardPackets::LeaveChannelPacket*>(pPacket);

		Mara::MUser *user = _users.value(packet->_userToken);
		Mara::MChannel *channel = _channels.value(packet->_channelToken);

		if(!user)
		{
			Mara::MLogger::log(Mara::LogLevel::Error, "Unknown user attempted to leave a channel.");
			//requestUnknownUser(packet->_userToken);
		}
		else if(!channel)
		{
			Mara::MLogger::log(Mara::LogLevel::Error, "User attempted to leave an unknown channel.");
		}
		else
		{
			if(channel->userInChannel(user->serverToken()))
			{
				channel->removeUser(user);
				_userToChannels.remove(user, channel);
				emit userLeftChannel(user, channel);
			}
		}

		delete packet;
	}

	void Client::handleChat(Mara::MPacket *pPacket)
	{
		Mara::StandardPackets::ChatPacket *packet = static_cast<Mara::StandardPackets::ChatPacket*>(pPacket);

		Mara::MUser *user;
		Mara::MChannel *channel;

		if(!(user = _users.value(packet->_userToken)))
		{
			Mara::MLogger::log(Mara::LogLevel::Error, QString("Unknown user attempted to chat: %1.").arg(packet->_userToken));
			requestUnknownUser(packet->_userToken);
		}
		else if(!(channel = _channels.value(packet->_channelToken)))
		{
			Mara::MLogger::log(Mara::LogLevel::Error, QString("User %1 attempted chat on unknown channel: %1.").arg(user->simpleName()).arg(packet->_channelToken));
		}
		else
		{
			emit channelChat(user, channel, packet->_text);
		}

		delete packet;
	}

	void Client::handleKick(Mara::MPacket *pPacket)
	{
		Mara::StandardPackets::KickPacket *packet = static_cast<Mara::StandardPackets::KickPacket*>(pPacket);

		Mara::MLogger::log(Mara::LogLevel::Error, QString("Kicked: %1").arg(packet->_reason));

		emit kicked();

		setStatus(Client_Kicked);

		delete packet;
	}

	void Client::handleEmote(Mara::MPacket *pPacket)
	{
		Mara::StandardPackets::EmotePacket *packet = static_cast<Mara::StandardPackets::EmotePacket*>(pPacket);

		Mara::MUser *user;
		Mara::MChannel *channel;

		if(!(user = _users.value(packet->_userToken)))
		{
			Mara::MLogger::log(Mara::LogLevel::Error, QString("Unknown user attempted to chat: %1.").arg(packet->_userToken));
			requestUnknownUser(packet->_userToken);
		}
		else if(!(channel = _channels.value(packet->_channelToken)))
		{
			Mara::MLogger::log(Mara::LogLevel::Error, QString("User %1 attempted chat on unknown channel: %1.").arg(user->simpleName()).arg(packet->_channelToken));
		}
		else
		{
			emit channelEmote(user, channel, packet->_text);
		}

		delete packet;
	}

	void Client::handleAway(Mara::MPacket *pPacket)
	{
		Mara::StandardPackets::AwayPacket *packet = static_cast<Mara::StandardPackets::AwayPacket*>(pPacket);
		Mara::MUser *user;
		
		if(!(user = _users.value(packet->_userToken)))
		{
			Mara::MLogger::log(Mara::LogLevel::Error, QString("Unknown user sent away: %1.").arg(packet->_userToken));
			requestUnknownUser(packet->_userToken);
		}
		else
		{
			user->setAway(packet->_text);
			emit userAway(user);
		}

		delete pPacket;
	}

	void Client::handleMessage(Mara::MPacket *pPacket)
	{
		Mara::StandardPackets::MessagePacket *packet = static_cast<Mara::StandardPackets::MessagePacket*>(pPacket);
		Mara::MUser *user;

		if(!(user = _users.value(packet->_userToken)))
		{
			Mara::MLogger::log(Mara::LogLevel::Error, QString("Unknown user sent message: %1.").arg(packet->_userToken));
			requestUnknownUser(packet->_userToken);
		}
		else
		{
			emit userMessage(user, packet->_text);
		}

		delete pPacket;
	}

	void Client::handlePing(Mara::MPacket *pPacket)
	{
		Q_UNUSED(pPacket);
		if(_status == Client_WaitPing)
		{
			setStatus(Client_VersionCheck);
			sendPacket(new Mara::StandardPackets::ClientVersionPacket(Mara::mLibraryVersion(), MaraClient::mClientVersion(), _appVersion));
		}
		emit pingReceived();

		delete pPacket;
	}
}
