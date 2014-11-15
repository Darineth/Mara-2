#ifndef MCHANNEL_H
#define MCHANNEL_H

#include <QHash>
#include <QList>
#include <QString>

#include "MaraLibDll.h"

#include "MUser.h"

namespace Mara
{
	class __MARALIB__CLASS_DECL MChannel : public QObject
	{
		Q_OBJECT;

		public:
			typedef QHash<Mara::MUser::Token, const Mara::MUser*>::iterator UserIterator;
			typedef quint32 Token;

			MChannel(const QString &pName, Mara::MChannel::Token pToken = 0, QObject *pParent = 0);
			~MChannel();

			void setToken(const Token pToken);
			Mara::MChannel::Token token() const;

			const QString& name() const;

			void addUser(const Mara::MUser* pUser);
			void removeUser(const Mara::MUser* pUser);

			bool userInChannel(const Mara::MUser* pUser) const;
			bool userInChannel(const Mara::MUser::Token pToken) const;

			QHash<Mara::MUser::Token, const Mara::MUser*> &users();

		private:
			QString _name;
			Token _token;
			QHash<Mara::MUser::Token, const Mara::MUser*> _users;
	};
};

#endif
