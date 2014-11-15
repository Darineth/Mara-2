#ifndef MARACLIENTSETTINGS_H
#define MARACLIENTSETTINGS_H

#include <QString>

#include "MSettings.h"
#include "MTextStyle.h"
#include "MUser.h"

namespace MaraClient
{
	class Client;
};

class MaraClientSettings
{
	public:
		class Color
		{
			public:
				static const QString User;
				static const QString Default;
				static const QString Message;
				static const QString System;
				static const QString Link;
				static const QString Background;
		};

		class Font
		{
			public:
				static const QString User;
				static const QString Default;
				static const QString Message;
				static const QString System;
		};

		class User
		{
			public:
				static const QString Name;
				static const QString Token;
		};

		class Chat
		{
			public:
				static const QString AllowUserFonts;
				static const QString ShowJoinLeave;
				static const QString Timestamps;
				static const QString TimestampFormat;
				static const QString LocalEcho;
				static const QString LogChat;
				static const QString LogFile;
				static const QString UserFontForEditBox;
				static const QString MessageWindows;
		};

		class Image
		{
			public:
				static const QString MaxWidth;
				static const QString MaxHeight;
		};

		class Connection
		{
			public:
				static const QString DataPort;
				static const QString Server;
				static const QString BackupServer;
				static const QString EnableHeartbeat;
				static const QString HeartbeatTimeout;
		};

		class UI
		{
			public:
				static const QString Location;
				static const QString Size;
				static const QString Maximized;
				static const QString State;
				static const QString SplitterSizes;
		};

		class Macro
		{
			public:
				static const QString FArg;
				static const QString F1;
				static const QString F2;
				static const QString F3;
				static const QString F4;
				static const QString F5;
				static const QString F6;
				static const QString F7;
				static const QString F8;
				static const QString F9;
				static const QString F10;
				static const QString F11;
				static const QString F12;
		};

		class Template
		{
			public:
				static const QString Chat;
				static const QString Emote;
				static const QString Message;
				static const QString Plain;
		};

		static Mara::MSettings& settings();

	private:
		MaraClientSettings();
};

#endif
