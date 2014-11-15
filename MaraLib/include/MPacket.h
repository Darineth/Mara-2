#ifndef MPACKET_H
#define MPACKET_H

#include "MException.h"

namespace Mara
{
	class MTcpSocket;
	class __MARALIB__CLASS_DECL MPacket
	{
		public:
			class SerializationException : public MException
			{
				public:
					SerializationException(const MPacket *pPacket, const QString &pMessage) :
							MException("[" + pPacket->typeName() + "] Serialization Error", pMessage) {}
			};

			MPacket(quint16 pType, const QString &pTypeName, bool pCompress = false);
			virtual ~MPacket();

			MPacket *construct(MTcpSocket *pSource) const;
			virtual void deserialize(const QByteArray &pData) = 0;
			QByteArray* serialize() const;

			MTcpSocket* source();

			virtual quint16 type() const;
			QString typeName() const;

			bool deleteOnSend() const;
			//void deleteOnSend(bool pDelete);

			virtual bool compressed() const;

		protected:
			virtual MPacket *construct() const = 0;
			virtual void serialize(QByteArray *pData) const = 0;
			void setSource(MTcpSocket *pSource);
			void compress(bool pCompress);

		private:
			quint16 _type;
			QString _typeName;
			MTcpSocket *_source;
			bool _deleteOnSend;
			mutable bool _compress;
			mutable QByteArray *_serializedData;
	};
};

#endif
