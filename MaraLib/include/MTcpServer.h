#ifndef MTCPSERVER_H
#define MTCPSERVER_H

#include <QMutex>
#include <QTcpServer>
#include <QLinkedList>

#include "MaraLibDll.h"

namespace Mara
{
	class __MARALIB__CLASS_DECL MTcpServer : public QTcpServer
	{
		Q_OBJECT;
		public:
			MTcpServer(QObject *pParent = 0);
			virtual ~MTcpServer();

			int nextPendingSocket();

			bool hasPendingConnections() const;

		protected:
			void incomingConnection(int pSocketDescriptor);

			QLinkedList<int> _pendingSocketDescriptors;
	};
};

#endif
