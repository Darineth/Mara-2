#ifndef MTCPSOCKET_H
#define MTCPSOCKET_H

#include <QMutex>
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>

#include "MPacket.h"
#include "MLogLevel.h"
#include "MPacketFactory.h"
#include "MaraStandardPacketTypes.h"

namespace Mara
{
	class __MARALIB__CLASS_DECL MTcpSocket : public QTcpSocket
	{
		Q_OBJECT;

		public:
			MTcpSocket(QObject *pParent = 0);
			MTcpSocket(int pSocketDescriptor, QObject *pParent = 0);
			virtual ~MTcpSocket();

			bool socketOpen() const;

		public slots:
			void close();
			void sendPacket(const Mara::MPacket *pPacket);

		signals:
			void packetReceived(Mara::MPacket *pPacket);
			void signalSendPacket(const Mara::MPacket *pPacket);

		protected:
			virtual void signalPacketReceived(MPacket *pPacket);

		private slots:
			void readData();
			void sendPacketInternal(const Mara::MPacket *pPacket);

		private:
			enum SocketReadState
			{
				ReadReady,
				WaitingForHeader,
				ReadingData,
				Error
			};

			void init();

			QTcpSocket *_socket;
			QDataStream _socketStream;
			SocketReadState _readState;
			int _nextPacketSize;
			int _nextPacketBytesRead;
			QByteArray _nextPacketBuffer;
			quint16 _nextPacketType;
			QMutex _mutex;

			static QString _packetHeader;
			static QByteArray _packetHeaderBytes;
	};
};

#endif
