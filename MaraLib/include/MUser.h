#ifndef MUser_H
#define MUser_H

#include <QString>
#include <QColor>
#include <QFont>
#include <QRegExp>

#include "MTextStyle.h"

#include "MaraLibDll.h"

namespace Mara
{
	class __MARALIB__CLASS_DECL MUser
	{
		public:
			typedef quint32 Token;

			MUser(const QString &pName, const QFont &pFont, const QColor pColor, MUser::Token pServerToken = 0);

			void setName(const QString &pName);
			const QString &name() const;
			const QString &simpleName() const;

			void setServerToken(MUser::Token pServerToken);
			MUser::Token serverToken() const;

			void setFont(const QFont &pFont);
			const QFont &font() const;

			void setColor(const QColor &pColor);
			const QColor &color() const;

			void setAway(const QString &pAway);
			const QString &away() const;

			const MTextStyle &style() const;
			MTextStyle &style();

			static const QString buildSimpleName(const QString &pName);

			static const QString generateUserToken();

			static bool validateUserToken(const QString &pToken);
			static bool validateUserName(const QString &pName, QString &error);

		private:
			MUser::Token _serverToken;

			QString _rawName;
			QString _simpleName;

			QString _away;

			MTextStyle _style;

			// TODO: Avatar (Transmit sideband), Cache via md5 of file?
	};
};

#endif
