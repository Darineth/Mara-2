#ifndef MARASTANDARDPACKETS_H
#define MARASTANDARDPACKETS_H

#include <QByteArray>
#include <QColor>
#include <QDataStream>
#include <QFont>
#include <QString>
#include <QtGlobal>

#include "MPacket.h"

#include "MaraLibDll.h"
#include "MaraStandardPacketTypes.h"
#include "MChannel.h"
#include "MUser.h"

namespace Mara
{
	namespace StandardPackets
	{
		class __MARALIB__CLASS_DECL UnknownPacket : public MPacket
		{
			public:
				UnknownPacket();
				UnknownPacket(quint16 pType);
				UnknownPacket(quint16 pType, const QByteArray &pData);
				~UnknownPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				QByteArray _rawData;
				quint16 _originalType;
		};

		class __MARALIB__CLASS_DECL TestPacket : public MPacket
		{
			public:
				TestPacket();
				TestPacket(const QString &pText);
				~TestPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				QString _text;
		};

		class __MARALIB__CLASS_DECL ResponsePacket : public MPacket
		{
			public:
				ResponsePacket();
				ResponsePacket(quint32 pResponseType, quint32 pResponse, QString pMessage);
				~ResponsePacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				quint16 _responseType;
				qint32 _response;
				QString _message;
		};

		class __MARALIB__CLASS_DECL ClientVersionPacket : public MPacket
		{
			public:
				ClientVersionPacket();
				ClientVersionPacket(quint32 pMaraVersion, quint32 pClientVersion, quint32 pAppVersion);
				~ClientVersionPacket() {}

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				quint32 _maraVersion;
				quint32 _clientVersion;
				quint32 _appVersion;
		};

		class __MARALIB__CLASS_DECL UserLoginPacket : public MPacket
		{
			public:
				UserLoginPacket();
				UserLoginPacket(QString pUserName, QString pUserToken, QFont pFont, QColor pColor);
				~UserLoginPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				QString _userName;
				QString _userToken;
				QFont _font;
				QColor _color;
		};

		class __MARALIB__CLASS_DECL UserConnectPacket : public MPacket
		{
			public:
				UserConnectPacket();
				UserConnectPacket(QString pUserName, MUser::Token pUserToken, QFont pFont, QColor pColor, QString pAway, bool pReconnect = false);
				~UserConnectPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				QString _userName;
				MUser::Token _userToken;
				QFont _font;
				QColor _color;
				QString _away;
				bool _reconnect;
		};

		class __MARALIB__CLASS_DECL UserDisconnectPacket : public MPacket
		{
			public:
				UserDisconnectPacket();
				UserDisconnectPacket(MUser::Token pUserToken, const QString &pReason);
				~UserDisconnectPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				MUser::Token _userToken;
				QString _reason;
		};

		class __MARALIB__CLASS_DECL JoinChannelPacket : public MPacket
		{
			public:
				JoinChannelPacket();
				JoinChannelPacket(MUser::Token pUserToken, const QString &pChannel, MChannel::Token pChannelToken = 0);
				~JoinChannelPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				MUser::Token _userToken;
				QString _channel;
				MChannel::Token _channelToken;
		};

		class __MARALIB__CLASS_DECL LeaveChannelPacket : public MPacket
		{
			public:
				LeaveChannelPacket();
				LeaveChannelPacket(MUser::Token pUserToken, MChannel::Token pChannelToken);
				~LeaveChannelPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				MUser::Token _userToken;
				MChannel::Token _channelToken;
		};

		class __MARALIB__CLASS_DECL UserUpdatePacket : public MPacket
		{
			public:
				UserUpdatePacket();
				UserUpdatePacket(QString pUserName, MUser::Token pUserToken, QFont pFont, QColor pColor);
				~UserUpdatePacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				QString _userName;
				MUser::Token _userToken;
				QFont _font;
				QColor _color;
		};

		class __MARALIB__CLASS_DECL ChatPacket : public MPacket
		{
			public:
				ChatPacket();
				ChatPacket(MUser::Token pUserToken, MChannel::Token pChannelToken, const QString &pText);
				~ChatPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;
				bool compressed() const;

				MUser::Token _userToken;
				MChannel::Token _channelToken;
				QString _text;
		};

		class __MARALIB__CLASS_DECL EmotePacket : public MPacket
		{
			public:
				EmotePacket();
				EmotePacket(MUser::Token pUserToken, MChannel::Token pChannelToken, const QString &pText);
				~EmotePacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				MUser::Token _userToken;
				MChannel::Token _channelToken;
				QString _text;
		};

		class __MARALIB__CLASS_DECL AwayPacket : public MPacket
		{
			public:
				AwayPacket();
				AwayPacket(MUser::Token pUserToken, const QString &pText);
				~AwayPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				MUser::Token _userToken;
				QString _text;
		};

		class __MARALIB__CLASS_DECL MessagePacket : public MPacket
		{
		public:
			MessagePacket();
			MessagePacket(MUser::Token pUserToken, MUser::Token pToUserToken, const QString &pText);
			~MessagePacket();

			MPacket *construct() const;
			void deserialize(const QByteArray &pData);
			void serialize(QByteArray *pData) const;

			MUser::Token _userToken;
			MUser::Token _toUserToken;
			QString _text;
		};

		class __MARALIB__CLASS_DECL KickPacket : public MPacket
		{
			public:
				KickPacket();
				KickPacket(MUser::Token pUserToken, const QString &pReason);
				~KickPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				MUser::Token _userToken;
				QString _reason;
		};

		class __MARALIB__CLASS_DECL PingPacket : public MPacket
		{
			public:
				PingPacket();
				PingPacket(MUser::Token pUserToken, quint32 pPingId, quint32 pSentAt, quint32 pLastPing);
				~PingPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				MUser::Token _userToken;
				quint32 _pingId;
				quint32 _sentAt;
				quint32 _lastPing;
		};

		class __MARALIB__CLASS_DECL ServerCommandPacket : public MPacket
		{
			public:
				ServerCommandPacket();
				ServerCommandPacket(MUser::Token pUserToken, const QString& pCommand, const QString& pArgs);
				~ServerCommandPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				MUser::Token _userToken;
				QString _command;
				QString _args;
		};

		class __MARALIB__CLASS_DECL UnknownUserPacket : public MPacket
		{
			public:
				UnknownUserPacket();
				UnknownUserPacket(MUser::Token pUserToken, MUser::Token pUnknownToken);
				~UnknownUserPacket();

				MPacket *construct() const;
				void deserialize(const QByteArray &pData);
				void serialize(QByteArray *pData) const;

				MUser::Token _userToken;
				MUser::Token _unknownToken;
		};
	};
}

#endif
