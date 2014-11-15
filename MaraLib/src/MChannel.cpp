#include "MChannel.h"

namespace Mara
{
	MChannel::MChannel(const QString &pName, Mara::MChannel::Token pToken, QObject *pParent) : QObject(pParent),
		_name(pName),
		_token(pToken),
		_users()
	{
	}

	MChannel::~MChannel()
	{
	}

	void MChannel::setToken(const Token pToken)
	{
		_token = pToken;
	}

	Mara::MChannel::Token MChannel::token() const
	{
		return _token;
	}

	const QString& MChannel::name() const
	{
		return _name;
	}

	void MChannel::addUser(const Mara::MUser* pUser)
	{
		if(!userInChannel(pUser))
		{
			_users.insert(pUser->serverToken(), pUser);
		}
	}

	void MChannel::removeUser(const Mara::MUser* pUser)
	{
		if(userInChannel(pUser))
		{
			_users.remove(pUser->serverToken());
		}
	}

	bool MChannel::userInChannel(const Mara::MUser* pUser) const
	{
		return _users.contains(pUser->serverToken());
	}

	bool MChannel::userInChannel(const Mara::MUser::Token pToken) const
	{
		return _users.contains(pToken);
	}

	QHash<Mara::MUser::Token, const Mara::MUser*> &MChannel::users()
	{
		return _users;
	}
};
