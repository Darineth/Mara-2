#include "MTcpSocket.h"

#include "MLogger.h"

int Mara::MaraTrace::depth = 0;

namespace Mara
{
	QString MTcpSocket::_packetHeader("MARA");
	QByteArray MTcpSocket::_packetHeaderBytes(_packetHeader.toAscii());

	MTcpSocket::MTcpSocket(QObject *pParent) : QTcpSocket(pParent),
			_socketStream(this),
			_readState(ReadReady),
			_nextPacketSize(-1),
			_nextPacketType(StandardPacketTypes::UNKNOWN),
			_mutex()
	{
		_socketStream.setVersion(QDataStream::Qt_4_4);
		init();
	}

	MTcpSocket::MTcpSocket(int pSocketDescriptor, QObject *pParent) : QTcpSocket(pParent),
			_socketStream(this),
			_readState(ReadReady),
			_nextPacketSize(-1),
			_nextPacketType(StandardPacketTypes::UNKNOWN),
			_mutex()
	{
		_socketStream.setVersion(QDataStream::Qt_4_4);
		setSocketDescriptor(pSocketDescriptor);
		init();
	}

	void MTcpSocket::init()
	{
		connect(this, SIGNAL(readyRead()), this, SLOT(readData()), Qt::QueuedConnection);
		connect(this, SIGNAL(signalSendPacket(const Mara::MPacket*)), this, SLOT(sendPacketInternal(const Mara::MPacket*)), Qt::QueuedConnection);
	}

	MTcpSocket::~MTcpSocket()
	{
		if(socketOpen()) disconnect();
	}

	bool MTcpSocket::socketOpen() const
	{
		return state() != QAbstractSocket::UnconnectedState;
	}

	void MTcpSocket::sendPacket(const Mara::MPacket *pPacket)
	{
		emit signalSendPacket(pPacket);
	}

	void MTcpSocket::sendPacketInternal(const Mara::MPacket *pPacket)
	{
		QByteArray data;
		bool compressed = pPacket->compressed();

		if(compressed)
		{
			QByteArray uncompressed(*(pPacket->serialize()));
			data = qCompress(uncompressed);
			qDebug("Compression Ratio: %d/%d", data.size(), uncompressed.size());
		}
		else
		{
			data = *(pPacket->serialize());
		}

		{
			QMutexLocker lock(&_mutex);

			write(_packetHeaderBytes);
			_socketStream << data.length();
			_socketStream << pPacket->type();

			write(data);

			_socketStream << qChecksum(data.constData(), data.length());
			_socketStream << compressed;
		}

		if(pPacket->deleteOnSend()) delete pPacket;
	}

	void MTcpSocket::readData()
	{
		qDebug("readData");
		static int headerLength = _packetHeaderBytes.length();
		switch(_readState)
		{
			case ReadReady:
			{
				qDebug("ReadReady");
				_nextPacketSize = -1;
			}
			case WaitingForHeader:
			{
				qDebug("WaitingForHeader");
				if(bytesAvailable() > headerLength)
				{
					qDebug("WaitingForHeader1");
					QByteArray rawHeader(read(headerLength));
					QString header(rawHeader);

					if(header != _packetHeader)
					{
						qDebug("WaitingForHeader->Error");
						_readState = Error;
						qWarning("Encountered invalid packet header \"%s\".", qPrintable(header));
						qDebug("Invalid header hex: [%s]", rawHeader.toHex().data());
						Mara::MLogger::log(Mara::LogLevel::Error, QString("Invalid packet header: %1.").arg(qPrintable(header)));
						emit readyRead();
						break;
					}

					_socketStream >> _nextPacketSize;
					_socketStream >> _nextPacketType;
					_readState = ReadingData;
					_nextPacketBytesRead = 0;
					_nextPacketBuffer.clear();
				}
				else
				{
					qDebug("WaitingForHeader2");
					_readState = WaitingForHeader;
					break;
				}
			}
			case ReadingData:
			{
				qDebug("ReadingData");
				if(_nextPacketSize <= 0)
				{
					qDebug("ReadingData-Error1");
					_readState = Error;
					qWarning("Entered ReadingData state with invalid next packet size %d.", _nextPacketSize);
					Mara::MLogger::log(Mara::LogLevel::Error, QString("Invalid packet size: %1.").arg(_nextPacketSize));
					emit readyRead();
					break;
				}

				quint64 available = bytesAvailable();
				qDebug("ReadingData-Available: %d", available);
				if(available < (_nextPacketSize - _nextPacketBytesRead))
				{
					qDebug("ReadingData-1");
					_nextPacketBytesRead += available;
					_nextPacketBuffer.append(read(available));
					break;
				}
				else
				{
					qDebug("ReadingData-2");
					_nextPacketBuffer.append(read(_nextPacketSize - _nextPacketBytesRead));

					bool compressed;
					quint16 sentChecksum;
					quint16 calculatedChecksum;

					_socketStream >> sentChecksum;

					calculatedChecksum = qChecksum(_nextPacketBuffer, _nextPacketSize);

					if(calculatedChecksum != sentChecksum)
					{
						qWarning("Received packet with bad checksum: %d expected, %d received.", sentChecksum, calculatedChecksum);
						Mara::MLogger::log(Mara::LogLevel::Error, QString("Received packet with bad checksum: %1 expected, %2 received.").arg(sentChecksum).arg(calculatedChecksum));
						_readState = Error;
						break;
					}

					_socketStream >> compressed;

					if(compressed)
					{
						qDebug("ReadingData-Decompressing");
						_nextPacketBuffer = qUncompress(_nextPacketBuffer);
					}

					MPacket *newPacket = MPacketFactory::construct(_nextPacketType, _nextPacketBuffer, this);

					signalPacketReceived(newPacket);

					_readState = ReadReady;

					if(bytesAvailable() > 0)
					{
						qDebug("ReadingData-More Available");
						emit readyRead();
					}
				}

				break;
			}
		};

		if(_readState == Error)
		{
			readAll();
			_readState = ReadReady;
			_nextPacketBytesRead = 0;
			_nextPacketBuffer.clear();
		}
	}

	void MTcpSocket::signalPacketReceived(MPacket *pPacket)
	{
		emit packetReceived(pPacket);
	}

	void MTcpSocket::close()
	{
		QTcpSocket::close();
	}
};
