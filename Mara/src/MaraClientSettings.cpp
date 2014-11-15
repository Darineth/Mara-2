#include "MaraClientSettings.h"

#include <QDateTime>
#include <QHash>

#include "MTextProcessors.h"

#include "MaraClient.h"

const QString MaraClientSettings::Color::User("Color/User");
const QString MaraClientSettings::Color::Default("Color/Default");
const QString MaraClientSettings::Color::Message("Color/Message");
const QString MaraClientSettings::Color::System("Color/System");
const QString MaraClientSettings::Color::Link("Color/Link");
const QString MaraClientSettings::Color::Background("Color/Background");

const QString MaraClientSettings::Font::User("Font/User");
const QString MaraClientSettings::Font::Default("Font/Default");
const QString MaraClientSettings::Font::Message("Font/Message");
const QString MaraClientSettings::Font::System("Font/System");

const QString MaraClientSettings::User::Name("User/Name");
const QString MaraClientSettings::User::Token("User/Token");

const QString MaraClientSettings::Chat::AllowUserFonts("Chat/AllowUserFonts");
const QString MaraClientSettings::Chat::ShowJoinLeave("Chat/ShowJoinLeave");
const QString MaraClientSettings::Chat::Timestamps("Chat/Timestamps");
const QString MaraClientSettings::Chat::TimestampFormat("Chat/TimestampFormat");
const QString MaraClientSettings::Chat::LocalEcho("Chat/LocalEcho");
const QString MaraClientSettings::Chat::LogChat("Chat/LogChat");
const QString MaraClientSettings::Chat::LogFile("Chat/LogFile");
const QString MaraClientSettings::Chat::UserFontForEditBox("Chat/UserFontForEditBox");
const QString MaraClientSettings::Chat::MessageWindows("Chat/MessageWindows");

const QString MaraClientSettings::Image::MaxWidth("Image/MaxWidth");
const QString MaraClientSettings::Image::MaxHeight("Image/MaxHeight");

const QString MaraClientSettings::Connection::DataPort("Connection/DataPort");
const QString MaraClientSettings::Connection::Server("Connection/Server");
const QString MaraClientSettings::Connection::BackupServer("Connection/BackupServer");
const QString MaraClientSettings::Connection::EnableHeartbeat("Connection/EnableHeartbeat");
const QString MaraClientSettings::Connection::HeartbeatTimeout("Connection/HeartbeatTimeout");

const QString MaraClientSettings::UI::Location("UI/Location");
const QString MaraClientSettings::UI::Size("UI/Size");
const QString MaraClientSettings::UI::Maximized("UI/Maximized");
const QString MaraClientSettings::UI::State("UI/State");
const QString MaraClientSettings::UI::SplitterSizes("UI/SplitterSizes");

const QString MaraClientSettings::Macro::FArg("Macro/F%1");
const QString MaraClientSettings::Macro::F1("Macro/F1");
const QString MaraClientSettings::Macro::F2("Macro/F2");
const QString MaraClientSettings::Macro::F3("Macro/F3");
const QString MaraClientSettings::Macro::F4("Macro/F4");
const QString MaraClientSettings::Macro::F5("Macro/F5");
const QString MaraClientSettings::Macro::F6("Macro/F6");
const QString MaraClientSettings::Macro::F7("Macro/F7");
const QString MaraClientSettings::Macro::F8("Macro/F8");
const QString MaraClientSettings::Macro::F9("Macro/F9");
const QString MaraClientSettings::Macro::F10("Macro/F10");
const QString MaraClientSettings::Macro::F11("Macro/F11");
const QString MaraClientSettings::Macro::F12("Macro/F12");

const QString MaraClientSettings::Template::Chat("Template/v2.2/Chat");
const QString MaraClientSettings::Template::Emote("Template/v2.2/Emote");
const QString MaraClientSettings::Template::Message("Template/v2.2/Message");
const QString MaraClientSettings::Template::Plain("Template/v2.2/Plain");

Mara::MSettings& MaraClientSettings::settings()
{
	static Mara::MSettings settings("./Mara.conf");
	return settings;
}
