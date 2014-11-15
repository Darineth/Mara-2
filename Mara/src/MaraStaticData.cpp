#include "MaraStaticData.h"

#include <QDateTime>

#include "MaraClient.h"
#include "MaraClientSettings.h"
#include "MTextProcessors.h"
#include "MUser.h"

QString MaraStaticData::sStyleSheet("");

QString MaraStaticData::sTimestampFormat("");
bool MaraStaticData::sAllowTimestamps = false;

Mara::MTextStyle MaraStaticData::sStyleDefault;
Mara::MTextStyle MaraStaticData::sStyleMessage;
Mara::MTextStyle MaraStaticData::sStyleSystem;
Mara::MTextStyle MaraStaticData::sStyleLink;

MaraClient::Client *MaraStaticData::sClient;

Mara::MUser *MaraStaticData::sMessageReplyUser = 0;

void MaraStaticData::init(MaraClient::Client *pClient)
{
	sClient = pClient;
}

void MaraStaticData::buildStyleSheet()
{
	QColor linkColor = MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Link);
	QColor backgroundColor = MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Background);

	sStyleSheet = QString("body {background-color: rgb(%1, %2, %3);}\n").arg(backgroundColor.red()).arg(backgroundColor.green()).arg(backgroundColor.blue()) + sStyleDefault.toCss() + sStyleMessage.toCss() + sStyleSystem.toCss() + sStyleLink.toCss() + QString("a { color:rgb(%1, %2, %3); } \n").arg(linkColor.red()).arg(linkColor.green()).arg(linkColor.blue());

	Mara::MTextStyle defaultTemp(sStyleDefault);

	//QHash<Mara::MUser::Token, Mara::MUser*> users = sClient->users();
	//for(QHash<Mara::MUser::Token, Mara::MUser*>::iterator ii = users.begin(); ii != users.end(); ++ii)
	//{
	//	if(MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::AllowUserFonts) || (*ii)->serverToken() == sClient->serverToken())
	//	{
	//		Mara::MTextStyle userStyle = (*ii)->style();

	//		if((abs(userStyle.color().red() - backgroundColor.red()) +
	//			abs(userStyle.color().green() - backgroundColor.green()) +
	//			abs(userStyle.color().blue() - backgroundColor.blue())) < 70)
	//		{
	//			userStyle.setColor(MaraStaticData::styleDefault().color());
	//		}

	//		sStyleSheet += userStyle.toCss();
	//	}
	//	else
	//	{
	//		defaultTemp.setName((*ii)->style().name());
	//		sStyleSheet += defaultTemp.toCss();
	//	}
	//}
	sStyleSheet += QString(" img {max-width: %1; max-height: %2} ")
		.arg(MaraClientSettings::settings().load<QString>(MaraClientSettings::Image::MaxWidth))
		.arg(MaraClientSettings::settings().load<QString>(MaraClientSettings::Image::MaxHeight));

	sStyleSheet += QString(" div.flashdiv {width: %1; height: %2} ")
		.arg(MaraClientSettings::settings().load<QString>(MaraClientSettings::Image::MaxWidth))
		.arg(MaraClientSettings::settings().load<QString>(MaraClientSettings::Image::MaxHeight));
}

void MaraStaticData::reloadStaticSettings()
{
	sAllowTimestamps = MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::Timestamps);
	sTimestampFormat = MaraClientSettings::settings().load<QString>(MaraClientSettings::Chat::TimestampFormat);

	sStyleDefault = Mara::MTextStyle("DEFAULT",
		MaraClientSettings::settings().load<QFont>(MaraClientSettings::Font::Default),
		MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Default));
	sStyleMessage = Mara::MTextStyle("MESSAGE",
		MaraClientSettings::settings().load<QFont>(MaraClientSettings::Font::Message),
		MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Message));
	sStyleSystem = Mara::MTextStyle("SYSTEM",
		MaraClientSettings::settings().load<QFont>(MaraClientSettings::Font::System),
		MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::System));
	sStyleLink = Mara::MTextStyle("LINK",
		MaraClientSettings::settings().load<QFont>(MaraClientSettings::Font::Default),
		MaraClientSettings::settings().load<QColor>(MaraClientSettings::Color::Link));
}

QString MaraStaticData::chatToHtml(const QString &pUserName, const QString &pText, const Mara::MTextStyle &pUserStyle)
{
	QString timestamp = "";

	if(sAllowTimestamps)
	{
		timestamp = Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(QDateTime::currentDateTime().toString(sTimestampFormat)));
	}

	QString userNameHtml = Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(pUserName));

	QString output = MaraClientSettings::settings().load<QString>(MaraClientSettings::Template::Chat);
	output.replace("%SYSTEMSTYLE%", sStyleSystem.toInlineCss(), Qt::CaseInsensitive);
	output.replace("%DEFAULTSTYLE%", sStyleDefault.toInlineCss(), Qt::CaseInsensitive);
	output.replace("%USERSTYLE%", userOrDefault(pUserStyle).toInlineCss(), Qt::CaseInsensitive);
	output.replace("%TIMESTAMP%", timestamp, Qt::CaseInsensitive);
	output.replace("%USERNAME%", userNameHtml, Qt::CaseInsensitive);
	output.replace("%TEXT%", Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(pText)), Qt::CaseInsensitive);
	return output;
}

QString MaraStaticData::emoteToHtml(const QString &pUserName, const QString &pText, const Mara::MTextStyle &pUserStyle)
{
	QString timestamp = "";

	if(sAllowTimestamps)
	{
		timestamp = Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(QDateTime::currentDateTime().toString(sTimestampFormat)));
	}

	QString userNameHtml = Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(pUserName));

	QString output = MaraClientSettings::settings().load<QString>(MaraClientSettings::Template::Emote);
	output.replace("%SYSTEMSTYLE%", sStyleSystem.toInlineCss(), Qt::CaseInsensitive);
	output.replace("%DEFAULTSTYLE%", sStyleDefault.toInlineCss(), Qt::CaseInsensitive);
	output.replace("%USERSTYLE%", userOrDefault(pUserStyle).toInlineCss(), Qt::CaseInsensitive);
	output.replace("%TIMESTAMP%", timestamp, Qt::CaseInsensitive);
	output.replace("%USERNAME%", userNameHtml, Qt::CaseInsensitive);
	output.replace("%TEXT%", Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(pText)), Qt::CaseInsensitive);
	return output;
}

QString MaraStaticData::messageToHtml(const QString &pUserName, const QString &pText, bool pTo, const Mara::MTextStyle &pUserStyle)
{
	QString timestamp = "";

	if(sAllowTimestamps)
	{
		timestamp = Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(QDateTime::currentDateTime().toString(sTimestampFormat)));
	}

	QString userNameHtml = Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(pUserName));

	QString output = MaraClientSettings::settings().load<QString>(MaraClientSettings::Template::Message);
	output.replace("%SYSTEMSTYLE%", sStyleSystem.toInlineCss(), Qt::CaseInsensitive);
	output.replace("%DEFAULTSTYLE%", sStyleDefault.toInlineCss(), Qt::CaseInsensitive);
	output.replace("%USERSTYLE%", userOrDefault(pUserStyle).toInlineCss(), Qt::CaseInsensitive);
	output.replace("%MESSAGESTYLE%", sStyleMessage.toInlineCss(), Qt::CaseInsensitive);
	if(pTo)
	{
		output.replace("%TOFROM%", "To", Qt::CaseInsensitive);
	}
	else
	{
		output.replace("%TOFROM%", "From", Qt::CaseInsensitive);
	}
	output.replace("%TIMESTAMP%", timestamp, Qt::CaseInsensitive);
	output.replace("%USERNAME%", userNameHtml, Qt::CaseInsensitive);
	output.replace("%TEXT%", Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(pText)), Qt::CaseInsensitive);
	return output;
}

QString MaraStaticData::plainToHtml(const QString &pText, bool pAllowTimestamp)
{
	QString timestamp = "";

	QString output = MaraClientSettings::settings().load<QString>(MaraClientSettings::Template::Plain);
	output.replace("%SYSTEMSTYLE%", sStyleSystem.toInlineCss(), Qt::CaseInsensitive);
	output.replace("%DEFAULTSTYLE%", sStyleDefault.toInlineCss(), Qt::CaseInsensitive);

	if(sAllowTimestamps && pAllowTimestamp)
	{
		timestamp = Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(QDateTime::currentDateTime().toString(sTimestampFormat)));
		output.replace("%TIMESTAMP%", timestamp, Qt::CaseInsensitive);
	}
	else
	{
		output.replace("%TIMESTAMP% ", "", Qt::CaseInsensitive);
		output.replace("%TIMESTAMP%", "", Qt::CaseInsensitive);
	}

	output.replace("%TEXT%", Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(pText)), Qt::CaseInsensitive);

	return output;
}

QString MaraStaticData::chatToHtml(const QString &pUserName, const QString &pText, const Mara::MTextStyle &pUserStyle, const Mara::MTextStyle &pDefaultStyle, const Mara::MTextStyle &pSystemStyle, const Mara::MTextStyle &pMessageStyle, const QString &pTemplate)
{
	QString timestamp = "";

	if(sAllowTimestamps)
	{
		timestamp = Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(QDateTime::currentDateTime().toString(sTimestampFormat)));
	}

	QString userNameHtml = Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(pUserName));

	QString output = pTemplate;
	output.replace("%SYSTEMSTYLE%", pSystemStyle.toInlineCss(), Qt::CaseInsensitive);
	output.replace("%DEFAULTSTYLE%", pDefaultStyle.toInlineCss(), Qt::CaseInsensitive);
	output.replace("%USERSTYLE%", userOrDefault(pUserStyle).toInlineCss(), Qt::CaseInsensitive);
	output.replace("%MESSAGESTYLE%", pMessageStyle.toInlineCss(), Qt::CaseInsensitive);
	output.replace("%TOFROM%", "From", Qt::CaseInsensitive);
	output.replace("%TIMESTAMP%", timestamp, Qt::CaseInsensitive);
	output.replace("%USERNAME%", userNameHtml, Qt::CaseInsensitive);
	output.replace("%TEXT%", Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(pText)), Qt::CaseInsensitive);
	return output;
}

MaraClient::Client * MaraStaticData::client()
{
	return sClient;
}

const QString & MaraStaticData::timestampFormat()
{
	return sTimestampFormat;
}

bool MaraStaticData::allowTimestamps()
{
	return sAllowTimestamps;
}

Mara::MTextStyle & MaraStaticData::styleDefault()
{
	return sStyleDefault;
}

Mara::MTextStyle & MaraStaticData::styleMessage()
{
	return sStyleMessage;
}

Mara::MTextStyle & MaraStaticData::styleSystem()
{
	return sStyleSystem;
}

Mara::MTextStyle & MaraStaticData::styleLink()
{
	return sStyleLink;
}

const QString & MaraStaticData::styleSheet()
{
	return sStyleSheet;
}

void MaraStaticData::setReplyUser(Mara::MUser *pUser)
{
	sMessageReplyUser = pUser;
}

Mara::MUser *MaraStaticData::replyUser()
{
	return sMessageReplyUser;
}

const Mara::MTextStyle& MaraStaticData::userOrDefault(const Mara::MTextStyle& pUserStyle)
{
	if(MaraClientSettings::settings().load<bool>(MaraClientSettings::Chat::AllowUserFonts) ||
			(pUserStyle.name() == MaraStaticData::client()->user().style().name()))
		return pUserStyle;
	else
		return sStyleDefault;
}