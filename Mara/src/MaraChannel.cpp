#include "MaraChannel.h"

// Channel Functions
MaraChannel::MaraChannel(const QString &pqstName)
{
	_name = pqstName;
}
MaraChannel::~MaraChannel()
{

}

void MaraChannel::join()
{

}
void MaraChannel::leave()
{

}

// Channel List
MaraChannelList::MaraChannelList() : QList<MaraChannel*>()
{
}

MaraChannelList::~MaraChannelList()
{
}

MaraChannelList::const_iterator MaraChannelList::findByName(const QString &name) const
{
	QString searchName = name.toLower();
	for(const_iterator ii = begin(); ii != end(); ++ii)
	{
		if((*ii)->name().toLower() == searchName)
		{
			return ii;
		}
	}

	return end();
}

MaraChannelList::iterator MaraChannelList::findByName(const QString &name)
{
	QString searchName = name.toLower();
	for(iterator ii = begin(); ii != end(); ++ii)
	{
		if((*ii)->name().toLower() == searchName)
		{
			return ii;
		}
	}

	return end();
}


// Channel Manager
MaraChannelManager::MaraChannelManager()
{
	_qlcChannels = new MaraChannelList();
}

MaraChannelManager::~MaraChannelManager()
{
	leaveChannels();
	delete _qlcChannels;
}

void MaraChannelManager::joinChannel(const QString &pqstName)
{
	MaraChannel *newChannel = new MaraChannel(pqstName);
	_qlcChannels->append(newChannel);
	emit channelAdded(newChannel);
	newChannel->join();
}

void MaraChannelManager::leaveChannel(const QString &pqstName)
{
	MaraChannelList::iterator ii = _qlcChannels->findByName(pqstName);
	if(ii != _qlcChannels->end())
	{
		MaraChannel *channel = (*ii);
		channel->leave();
		_qlcChannels->erase(ii);
		emit channelRemoved(channel);
		delete channel;
	}
	else
	{
		throw new Mara::MException(__FUNCTION_NAME__, "Attempt to leave unknown channel: " + pqstName);
	}
}

void MaraChannelManager::leaveChannels()
{
	MaraChannel *channel;
	for(MaraChannelList::Iterator ii = _qlcChannels->begin(); ii != _qlcChannels->end(); )
	{
		channel = (*ii);
		channel->leave();
		ii = _qlcChannels->erase(ii);
		emit channelRemoved(channel);
		delete channel;
	}
}