#ifndef MARACHANNEL_H
#define MARACHANNEL_H

#include <QObject>
#include <QList>

#include "MException.h"

class MaraChannel : public QObject
{
	Q_OBJECT;
	public:
		MaraChannel(const QString &pqstName);
		virtual ~MaraChannel();

		const QString& name() const { return _name; }

		void join();
		void leave();

	private:
		QString _name;
};

class MaraChannelList : public QList<MaraChannel*>
{
	public:
		MaraChannelList();
		virtual ~MaraChannelList();

		const_iterator findByName(const QString &name) const;
		iterator findByName(const QString &name);
};

class MaraChannelManager : public QObject
{
	Q_OBJECT;
	public:
		MaraChannelManager();
		virtual ~MaraChannelManager();

		void joinChannel(const QString &pqstName);
		void leaveChannel(const QString &pqstName);
		void leaveChannels();

	signals:
		void channelAdded(MaraChannel *pmchChannel);
		void channelRemoved(MaraChannel *pmchChannel);

	private:
		 MaraChannelList *_qlcChannels;
};

#endif
