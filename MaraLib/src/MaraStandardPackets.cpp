#include "MaraStandardPackets.h"

namespace Mara
{
	namespace StandardPackets
	{
		////////////////////////////////////////////
		// Unknown Catch-all Packet
		////////////////////////////////////////////

		UnknownPacket::UnknownPacket() : MPacket(StandardPacketTypes::UNKNOWN, "Unknown"), _originalType(65535)
		{

		}

		UnknownPacket::UnknownPacket(quint16 pType) : MPacket(StandardPacketTypes::UNKNOWN, "Unknown"), _originalType(pType)
		{

		}

		UnknownPacket::UnknownPacket(quint16 pType, const QByteArray &pData) : MPacket(StandardPacketTypes::UNKNOWN, "Unknown"), _rawData(pData), _originalType(pType)
		{

		}

		UnknownPacket::~UnknownPacket()
		{

		}

		MPacket* UnknownPacket::construct() const
		{
			return new UnknownPacket();
		}

		void UnknownPacket::deserialize(const QByteArray &pData)
		{
			_rawData = pData;
		}

		void UnknownPacket::serialize(QByteArray *pData) const
		{
			throw new SerializationException(this, "Attempt to serialize Unknown packet.");
			Q_UNUSED(pData);
		}

		////////////////////////////////////////////
		// Debug Test Packet
		////////////////////////////////////////////

		TestPacket::TestPacket() : MPacket(StandardPacketTypes::TEST, "TestPacket")
		{

		}

		TestPacket::TestPacket(const QString &pText) : MPacket(StandardPacketTypes::TEST, "TestPacket"), _text(pText)
		{

		}

		TestPacket::~TestPacket()
		{

		}

		MPacket* TestPacket::construct() const
		{
			return new TestPacket();
		}

		void TestPacket::deserialize(const QByteArray &pData)
		{
			_text = QString(pData);
		}

		void TestPacket::serialize(QByteArray *pData) const
		{
			pData->append(_text.toAscii());
		}

		////////////////////////////////////////////
		// Generic Response Packet
		////////////////////////////////////////////

		ResponsePacket::ResponsePacket() : MPacket(StandardPacketTypes::RESPONSE, "ResponsePacket"),
			_responseType(0), 
			_response(0),
			_message()
		{

		}

		ResponsePacket::ResponsePacket(quint32 pResponseType, quint32 pResponse, QString pMessage) : MPacket(StandardPacketTypes::RESPONSE, "ResponsePacket"),
			_responseType(pResponseType), 
			_response(pResponse),
			_message(pMessage)
		{

		}

		ResponsePacket::~ResponsePacket()
		{

		}

		MPacket* ResponsePacket::construct() const
		{
			return new ResponsePacket();
		}

		void ResponsePacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _responseType;
			stream >> _response;
			stream >> _message;
		}

		void ResponsePacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _responseType;
			stream << _response;
			stream << _message;
		}

		////////////////////////////////////////////
		// Client Version Check Packet
		////////////////////////////////////////////

		ClientVersionPacket::ClientVersionPacket() : MPacket(StandardPacketTypes::CLIENTVERSION, "ClientVersionPacket"),
			_maraVersion(0), 
			_clientVersion(0),
			_appVersion(0)
		{

		}

		ClientVersionPacket::ClientVersionPacket(quint32 pMaraVersion, quint32 pClientVersion, quint32 pAppVersion) : MPacket(StandardPacketTypes::CLIENTVERSION, "ClientVersionPacket"),
			_maraVersion(pMaraVersion),
			_clientVersion(pClientVersion),
			_appVersion(pAppVersion)
		{

		}

		MPacket* ClientVersionPacket::construct() const
		{
			return new ClientVersionPacket();
		}

		void ClientVersionPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _maraVersion;
			stream >> _clientVersion;
		}

		void ClientVersionPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _maraVersion;
			stream << _clientVersion;
		}

		////////////////////////////////////////////
		// User Login Packet
		////////////////////////////////////////////

		UserLoginPacket::UserLoginPacket() : MPacket(StandardPacketTypes::USERLOGIN, "UserLoginPacket"),
			_userName(),
			_userToken(),
			_font(),
			_color()
		{

		}

		UserLoginPacket::UserLoginPacket(QString pUserName, QString pUserToken, QFont pFont, QColor pColor) :
			MPacket(StandardPacketTypes::USERLOGIN, "UserLoginPacket"),
			_userName(pUserName),
			_userToken(pUserToken),
			_font(pFont),
			_color(pColor)
		{

		}

		UserLoginPacket::~UserLoginPacket()
		{

		}

		MPacket* UserLoginPacket::construct() const
		{
			return new UserLoginPacket();
		}

		void UserLoginPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userName;
			stream >> _userToken;
			stream >> _font;
			stream >> _color;
		}

		void UserLoginPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userName;
			stream << _userToken;
			stream << _font;
			stream << _color;
		}

		////////////////////////////////////////////
		// User Connect Packet
		////////////////////////////////////////////

		UserConnectPacket::UserConnectPacket() : MPacket(StandardPacketTypes::USERCONNECT, "UserConnectPacket"),
			_userName(),
			_userToken(0),
			_font(),
			_color(),
			_reconnect(false)
		{
		}

		UserConnectPacket::UserConnectPacket(QString pUserName, MUser::Token pUserToken, QFont pFont, QColor pColor, QString pAway, bool pReconnect) :
			MPacket(StandardPacketTypes::USERCONNECT, "UserConnectPacket"),
			_userName(pUserName),
			_userToken(pUserToken),
			_font(pFont),
			_color(pColor),
			_away(pAway),
			_reconnect(pReconnect)
		{
		}

		UserConnectPacket::~UserConnectPacket()
		{

		}

		MPacket* UserConnectPacket::construct() const
		{
			return new UserConnectPacket();
		}

		void UserConnectPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userName;
			stream >> _userToken;
			stream >> _font;
			stream >> _color;
			stream >> _away;
			stream >> _reconnect;
		}

		void UserConnectPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userName;
			stream << _userToken;
			stream << _font;
			stream << _color;
			stream << _away;
			stream << _reconnect;
		}

		////////////////////////////////////////////
		// User Disconnect Packet
		////////////////////////////////////////////

		UserDisconnectPacket::UserDisconnectPacket() : MPacket(StandardPacketTypes::USERDISCONNECT, "UserDisconnectPacket"),
			_userToken(0),
			_reason()
		{
		}

		UserDisconnectPacket::UserDisconnectPacket(MUser::Token pUserToken, const QString &pReason) :
			MPacket(StandardPacketTypes::USERDISCONNECT, "UserDisconnectPacket"),
			_userToken(pUserToken),
			_reason(pReason)
		{
		}

		UserDisconnectPacket::~UserDisconnectPacket()
		{

		}

		MPacket* UserDisconnectPacket::construct() const
		{
			return new UserDisconnectPacket();
		}

		void UserDisconnectPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userToken;
			stream >> _reason;
		}

		void UserDisconnectPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userToken;
			stream << _reason;
		}

		////////////////////////////////////////////
		// Join Channel Packet
		////////////////////////////////////////////

		JoinChannelPacket::JoinChannelPacket() : MPacket(StandardPacketTypes::JOINCHANNEL, "JoinChannelPacket"),
			_userToken(0),
			_channel(),
			_channelToken(0)
		{
		}

		JoinChannelPacket::JoinChannelPacket(MUser::Token pUserToken, const QString &pChannel, MChannel::Token pChannelToken) :
			MPacket(StandardPacketTypes::JOINCHANNEL, "JoinChannelPacket"),
			_userToken(pUserToken),
			_channel(pChannel),
			_channelToken(pChannelToken)
		{
		}

		JoinChannelPacket::~JoinChannelPacket()
		{

		}

		MPacket* JoinChannelPacket::construct() const
		{
			return new JoinChannelPacket();
		}

		void JoinChannelPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userToken;
			stream >> _channel;
			stream >> _channelToken;
		}

		void JoinChannelPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userToken;
			stream << _channel;
			stream << _channelToken;
		}

		////////////////////////////////////////////
		// Leave Channel Packet
		////////////////////////////////////////////

		LeaveChannelPacket::LeaveChannelPacket() : MPacket(StandardPacketTypes::LEAVECHANNEL, "LeaveChannelPacket"),
			_userToken(0),
			_channelToken()
		{
		}

		LeaveChannelPacket::LeaveChannelPacket(MUser::Token pUserToken, MChannel::Token pChannelToken) :
			MPacket(StandardPacketTypes::LEAVECHANNEL, "LeaveChannelPacket"),
			_userToken(pUserToken),
			_channelToken(pChannelToken)
		{
		}

		LeaveChannelPacket::~LeaveChannelPacket()
		{

		}

		MPacket* LeaveChannelPacket::construct() const
		{
			return new LeaveChannelPacket();
		}

		void LeaveChannelPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userToken;
			stream >> _channelToken;
		}

		void LeaveChannelPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userToken;
			stream << _channelToken;
		}

		////////////////////////////////////////////
		// User Update Packet
		////////////////////////////////////////////

		UserUpdatePacket::UserUpdatePacket() : MPacket(StandardPacketTypes::USERUPDATE, "UserUpdatePacket"),
			_userName(),
			_userToken(0),
			_font(),
			_color()
		{
		}

		UserUpdatePacket::UserUpdatePacket(QString pUserName, MUser::Token pUserToken, QFont pFont, QColor pColor) :
			MPacket(StandardPacketTypes::USERUPDATE, "UserUpdatePacket"),
			_userName(pUserName),
			_userToken(pUserToken),
			_font(pFont),
			_color(pColor)
		{
		}

		UserUpdatePacket::~UserUpdatePacket()
		{

		}

		MPacket* UserUpdatePacket::construct() const
		{
			return new UserUpdatePacket();
		}

		void UserUpdatePacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userName;
			stream >> _userToken;
			stream >> _font;
			stream >> _color;
		}

		void UserUpdatePacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userName;
			stream << _userToken;
			stream << _font;
			stream << _color;
		}

		////////////////////////////////////////////
		// Chat Packet
		////////////////////////////////////////////

		ChatPacket::ChatPacket() : MPacket(StandardPacketTypes::CHAT, "ChatPacket"),
			_userToken(0),
			_channelToken(0),
			_text()
		{
		}

		ChatPacket::ChatPacket(MUser::Token pUserToken, MChannel::Token pChannelToken, const QString &pText) :
			MPacket(StandardPacketTypes::CHAT, "ChatPacket"),
			_userToken(pUserToken),
			_channelToken(pChannelToken),
			_text(pText)
		{
		}

		ChatPacket::~ChatPacket()
		{

		}

		MPacket* ChatPacket::construct() const
		{
			return new ChatPacket();
		}

		void ChatPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userToken;
			stream >> _channelToken;
			stream >> _text;
		}

		void ChatPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userToken;
			stream << _channelToken;
			stream << _text;
		}

		bool ChatPacket::compressed() const
		{
			return _text.size() > 100;
		}

		////////////////////////////////////////////
		// Emote Packet
		////////////////////////////////////////////

		EmotePacket::EmotePacket() : MPacket(StandardPacketTypes::EMOTE, "EmotePacket"),
			_userToken(0),
			_channelToken(0),
			_text()
		{
		}

		EmotePacket::EmotePacket(MUser::Token pUserToken, MChannel::Token pChannelToken, const QString &pText) :
			MPacket(StandardPacketTypes::EMOTE, "EmotePacket"),
			_userToken(pUserToken),
			_channelToken(pChannelToken),
			_text(pText)
		{
		}

		EmotePacket::~EmotePacket()
		{

		}

		MPacket* EmotePacket::construct() const
		{
			return new EmotePacket();
		}

		void EmotePacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userToken;
			stream >> _channelToken;
			stream >> _text;
		}

		void EmotePacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userToken;
			stream << _channelToken;
			stream << _text;
		}

		////////////////////////////////////////////
		// Away Packet
		////////////////////////////////////////////

		AwayPacket::AwayPacket() : MPacket(StandardPacketTypes::AWAY, "AwayPacket"),
			_userToken(0),
			_text()
		{
		}

		AwayPacket::AwayPacket(MUser::Token pUserToken, const QString &pText) :
			MPacket(StandardPacketTypes::AWAY, "AwayPacket"),
			_userToken(pUserToken),
			_text(pText)
		{
		}

		AwayPacket::~AwayPacket()
		{
		}

		MPacket* AwayPacket::construct() const
		{
			return new AwayPacket();
		}

		void AwayPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userToken;
			stream >> _text;
		}

		void AwayPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userToken;
			stream << _text;
		}

		////////////////////////////////////////////
		// Message Packet
		////////////////////////////////////////////

		MessagePacket::MessagePacket() : MPacket(StandardPacketTypes::MESSAGE, "MessagePacket"),
			_userToken(0),
			_toUserToken(0),
			_text()
		{
		}

		MessagePacket::MessagePacket(MUser::Token pUserToken, MUser::Token pToUserToken, const QString &pText) :
			MPacket(StandardPacketTypes::MESSAGE, "MessagePacket"),
			_userToken(pUserToken),
			_toUserToken(pToUserToken),
			_text(pText)
		{
		}

		MessagePacket::~MessagePacket()
		{
		}

		MPacket* MessagePacket::construct() const
		{
			return new MessagePacket();
		}

		void MessagePacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userToken;
			stream >> _toUserToken;
			stream >> _text;
		}

		void MessagePacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userToken;
			stream << _toUserToken;
			stream << _text;
		}

		////////////////////////////////////////////
		// Kick Packet
		////////////////////////////////////////////

		KickPacket::KickPacket() : MPacket(StandardPacketTypes::KICK, "KickPacket"),
			_userToken(0),
			_reason()
		{
		}

		KickPacket::KickPacket(MUser::Token pUserToken, const QString &pReason) :
			MPacket(StandardPacketTypes::KICK, "KickPacket"),
			_userToken(pUserToken),
			_reason(pReason)
		{
		}

		KickPacket::~KickPacket()
		{

		}

		MPacket* KickPacket::construct() const
		{
			return new KickPacket();
		}

		void KickPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userToken;
			stream >> _reason;
		}

		void KickPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userToken;
			stream << _reason;
		}

		////////////////////////////////////////////
		// Ping Packet
		////////////////////////////////////////////

		PingPacket::PingPacket() : MPacket(StandardPacketTypes::PING, "PingPacket") {}

		PingPacket::PingPacket(MUser::Token pUserToken, quint32 pPingId, quint32 pSentAt, quint32 pLastPing) : MPacket(StandardPacketTypes::PING, "PingPacket"),
				_userToken(pUserToken),
				_pingId(pPingId),
				_sentAt(pSentAt),
				_lastPing(pLastPing)
		{
		}

		PingPacket::~PingPacket() {}

		MPacket * PingPacket::construct() const
		{
			return new PingPacket();
		}

		void PingPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userToken;
			stream >> _pingId;
			stream >> _sentAt;
			stream >> _lastPing;
		}

		void PingPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userToken;
			stream << _pingId;
			stream << _sentAt;
			stream << _lastPing;
		}

		////////////////////////////////////////////
		// Server Command Packet
		////////////////////////////////////////////

		ServerCommandPacket::ServerCommandPacket() : MPacket(StandardPacketTypes::SERVERCOMMAND, "ServerCommandPacket") {}

		ServerCommandPacket::ServerCommandPacket(MUser::Token pUserToken, const QString& pCommand, const QString& pArgs) : MPacket(StandardPacketTypes::SERVERCOMMAND, "ServerCommandPacket"),
				_userToken(pUserToken),
				_command(pCommand),
				_args(pArgs)
		{
		}

		ServerCommandPacket::~ServerCommandPacket() {}

		MPacket * ServerCommandPacket::construct() const
		{
			return new ServerCommandPacket();
		}

		void ServerCommandPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userToken;
			stream >> _command;
			stream >> _args;
		}

		void ServerCommandPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userToken;
			stream << _command;
			stream << _args;
		}

		////////////////////////////////////////////
		// Unknown User Packet
		////////////////////////////////////////////

		UnknownUserPacket::UnknownUserPacket() : MPacket(StandardPacketTypes::UNKNOWNUSER, "UnknownUserPacket"),
			_userToken(0)
		{
		}

		UnknownUserPacket::UnknownUserPacket(MUser::Token pUserToken, MUser::Token pUnknownToken) :
			MPacket(StandardPacketTypes::UNKNOWNUSER, "UnknownUserPacket"),
			_userToken(pUserToken),
			_unknownToken(pUnknownToken)
		{
		}

		UnknownUserPacket::~UnknownUserPacket()
		{

		}

		MPacket* UnknownUserPacket::construct() const
		{
			return new UnknownUserPacket();
		}

		void UnknownUserPacket::deserialize(const QByteArray &pData)
		{
			QDataStream stream(pData);
			stream.setVersion(QDataStream::Qt_4_4);
			stream >> _userToken;
			stream >> _unknownToken;
		}

		void UnknownUserPacket::serialize(QByteArray *pData) const
		{
			QDataStream stream(pData, QIODevice::WriteOnly);
			stream.setVersion(QDataStream::Qt_4_4);
			stream << _userToken;
			stream << _unknownToken;
		}
	};
};
