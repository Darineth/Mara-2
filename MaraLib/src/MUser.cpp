#include "MUser.h"
#include "MRandom.h"
#include "MTextProcessors.h"

#include <QDateTime>

namespace Mara
{
	MUser::MUser(const QString &pName, const QFont &pFont, const QColor pColor, MUser::Token pServerToken) :
			_serverToken(pServerToken),
			_rawName(pName),
			_simpleName(MUser::buildSimpleName(_rawName)),
			_away(""),
			_style(QString("USER%1").arg(pServerToken), pFont, pColor)
	{
	}

	void MUser::setName(const QString &pName)
	{
		_rawName = pName;
		_simpleName = buildSimpleName(_rawName);
	}

	const QString& MUser::name() const
	{
		return _rawName;
	}

	const QString& MUser::simpleName() const
	{
		return _simpleName;
	}

	void MUser::setServerToken(MUser::Token pServerToken)
	{
		_serverToken = pServerToken;
		_style.setName(QString("USER%1").arg(pServerToken));
	}

	MUser::Token MUser::serverToken() const
	{
		return _serverToken;
	}

	void MUser::setFont(const QFont &pFont)
	{
		_style.setFont(pFont);
	}

	const QFont& MUser::font() const
	{
		return _style.font();
	}

	void MUser::setColor(const QColor &pColor)
	{
		_style.setColor(pColor);
	}

	const QColor& MUser::color() const
	{
		return _style.color();
	}

	void MUser::setAway(const QString &pAway)
	{
		_away = pAway;
	}

	const QString &MUser::away() const
	{
		return _away;
	}

	const MTextStyle &MUser::style() const
	{
		return _style;
	}

	MTextStyle &MUser::style()
	{
		return _style;
	}

	const QString MUser::buildSimpleName(const QString &pName)
	{
		QString name = pName;
		return MCodeConverter::replace(name, true);
	}

	const QString MUser::generateUserToken()
	{
		QString token("");
		MersenneRNG rng(QDateTime::currentDateTime().toTime_t());

		for(int ii = 0; ii < 5; ++ii)
		{
			for(int jj = 0; jj < 5; ++jj)
			{
				int num = rng() % 36;

				if(num > 25)
				{
					token += QChar((char)('0' + (num - 26)));
				}
				else
				{
					token += QChar((char)('A' + num));
				}
			}

			token += "-";
		}

		return token.left(token.size() - 1);
	}

	bool MUser::validateUserToken(const QString &pToken)
	{
		static QRegExp validRegex("[a-zA-Z0-9]{5}-[a-zA-Z0-9]{5}-[a-zA-Z0-9]{5}-[a-zA-Z0-9]{5}-[a-zA-Z0-9]{5}");

		return validRegex.exactMatch(pToken);
	}

	bool MUser::validateUserName(const QString &pName, QString &error)
	{
		error = "";

		QString simpleName = buildSimpleName(pName);

		if(pName.length() > 200)
		{
			error = "User name too long.";
			return false;
		}

		int invalidCharIndex = -1;

		QChar zwsp = pName.at(1);

		QString invalidChars(QString::fromWCharArray(L"\r\n\t\x200B"));
		for(int ii = 0; ii < invalidChars.length(); ++ii)
		{
			QChar invalidChar = invalidChars.at(ii);
			invalidCharIndex = pName.indexOf(invalidChars.at(ii));
			if(invalidCharIndex != -1) break;
		}

		if(invalidCharIndex != -1)
		{
			error = QString("Name contains invalid character: \"%1\"").arg(pName.at(invalidCharIndex));
			return false;
		}

		if(simpleName.length() == 0)
		{
			error = "Simplified name invalid.  Supplied name must include some non-maracode characters.";
			return false;
		}

		if(simpleName.length() > 40)
		{
			error = "Simplified name too long.";
			return false;
		}

		return true;
	}
};
