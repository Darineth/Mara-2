#ifndef MPACKETFACTORY_H
#define MPACKETFACTORY_H

#include <QHash>

#include "MPacket.h"
#include "MaraStandardPackets.h"

namespace Mara
{
	class __MARALIB__CLASS_DECL MPacketFactory
	{
		public:
			static MPacketFactory& factory();

			static void registerPacketType(const MPacket *pPrototypePacket);
			static void registerPacketType(quint16 pType, const MPacket *pPrototypePacket);

			static MPacket *construct(quint16 pType, const QByteArray &pData, MTcpSocket *pSource);

		private:
			MPacketFactory();
			MPacketFactory(const MPacketFactory &pMpf);
			~MPacketFactory();

			QHash<quint16, const MPacket*> _prototypes;
	};
};

#endif
