#include "MPacketFactory.h"
#include "MaraUtilityDefines.h"
#include "MaraStandardPacketTypes.h"

namespace Mara
{
	MPacketFactory::MPacketFactory()
	{
	}

	MPacketFactory::~MPacketFactory()
	{
		for(QHash<quint16, const MPacket*>::iterator ii = factory()._prototypes.begin(); ii != factory()._prototypes.end(); ++ii)
		{
			delete (*ii);
		}

		factory()._prototypes.clear();
	}

	MPacketFactory& MPacketFactory::factory()
	{
		static MPacketFactory factory;
		return factory;
	}

	void MPacketFactory::registerPacketType(const MPacket *pPrototypePacket)
	{
		if(pPrototypePacket->type() == StandardPacketTypes::UNKNOWN)
		{
			throw new MException(__FUNCTION_NAME__, QString("Attempt to register UNKNOWN packet type.  Do not attempt to register this type manually."));
		}

		if(factory()._prototypes.contains(pPrototypePacket->type()))
		{
			throw new MException(__FUNCTION_NAME__, QString("Attempt to register duplicate packet type %1 (%2).").arg(pPrototypePacket->type()).arg(pPrototypePacket->typeName()));
		}
		factory()._prototypes.insert(pPrototypePacket->type(), pPrototypePacket);
	}

	void MPacketFactory::registerPacketType(quint16 pType, const MPacket *pPrototypePacket)
	{
		if(pType == StandardPacketTypes::UNKNOWN)
		{
			throw new MException(__FUNCTION_NAME__, QString("Attempt to register UNKNOWN packet type.  Do not attempt to register this type manually."));
		}

		if(factory()._prototypes.contains(pType))
		{
			throw new MException(__FUNCTION_NAME__, QString("Attempt to register duplicate packet type %1 (%2).").arg(pType).arg(pPrototypePacket->typeName()));
		}
		factory()._prototypes.insert(pType, pPrototypePacket);
	}

	MPacket *MPacketFactory::construct(quint16 pType, const QByteArray &pData, MTcpSocket *pSource)
	{
		if(factory()._prototypes.contains(pType))
		{
			MPacket *packet = factory()._prototypes.value(pType)->construct(pSource);
			packet->deserialize(pData);
			return packet;
		}
		else
		{
			return new StandardPackets::UnknownPacket(pType, pData);
		}
	}
};