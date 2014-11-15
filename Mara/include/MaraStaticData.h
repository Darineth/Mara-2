#ifndef MARASTATICDATA_H
#define MARASTATICDATA_H

#include "MTextStyle.h"

namespace Mara
{
	class MUser;
}
namespace MaraClient
{
	class Client;
}
class MaraStaticData
{
	public:
		static void init(MaraClient::Client *pClient);

		static void buildStyleSheet();
		static void reloadStaticSettings();
		static QString chatToHtml(const QString &pUserName, const QString &pText, const Mara::MTextStyle &pUserStyle);
		static QString emoteToHtml(const QString &pUserName, const QString &pText, const Mara::MTextStyle &pUserStyle);
		static QString messageToHtml(const QString &pUserName, const QString &pText, bool pTo, const Mara::MTextStyle &pUserStyle);
		static QString plainToHtml(const QString &pText, bool pAllowTimestamp = true);
		static QString chatToHtml(const QString &pUserName, const QString &pText, const Mara::MTextStyle &pUserStyle, const Mara::MTextStyle &pDefaultStyle, const Mara::MTextStyle &pSystemStyle, const Mara::MTextStyle &pMessageStyle, const QString &pTemplate);

		static const Mara::MTextStyle& userOrDefault(const Mara::MTextStyle& pUserStyle);

		static MaraClient::Client *client();

		static const QString &timestampFormat();
		static bool allowTimestamps();

		static Mara::MTextStyle &styleDefault();
		static Mara::MTextStyle &styleMessage();
		static Mara::MTextStyle &styleSystem();
		static Mara::MTextStyle &styleLink();

		static const QString &styleSheet();

		static void setReplyUser(Mara::MUser *pUser);
		static Mara::MUser *replyUser();

	private:
		static MaraClient::Client *sClient;

		static QString sStyleSheet;

		static QString sTimestampFormat;
		static bool sAllowTimestamps;

		static Mara::MTextStyle sStyleDefault;
		static Mara::MTextStyle sStyleMessage;
		static Mara::MTextStyle sStyleSystem;
		static Mara::MTextStyle sStyleLink;

		static Mara::MUser *sMessageReplyUser;
};


#endif
