#include "MTcpServer.h"

#include <QThread>

namespace Mara
{
	MTcpServer::MTcpServer(QObject *pParent) : QTcpServer(pParent),
			_pendingSocketDescriptors()
	{}

	MTcpServer::~MTcpServer()
	{
		_pendingSocketDescriptors.clear();
	}

	int MTcpServer::nextPendingSocket()
	{
		if(!hasPendingConnections())
		{
			return 0;
		}

		return _pendingSocketDescriptors.takeFirst();
	}

	void MTcpServer::incomingConnection(int pSocketDescriptor)
	{
		int test = _pendingSocketDescriptors.size();
		_pendingSocketDescriptors.append(pSocketDescriptor);
		test = _pendingSocketDescriptors.size();
		test = 0;
	}

	bool MTcpServer::hasPendingConnections() const
	{
		return !_pendingSocketDescriptors.empty();
	}
};
