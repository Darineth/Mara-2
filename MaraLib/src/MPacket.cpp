#include "MPacket.h"

namespace Mara
{
	MPacket::MPacket(quint16 pType, const QString &pTypeName, bool pCompress) :
		_type(pType),
		_typeName(pTypeName),
		_deleteOnSend(true),
		_compress(pCompress),
		_serializedData(0)
	{}

	MPacket::~MPacket()
	{
		if(_serializedData) delete _serializedData;
	}

	MPacket* MPacket::construct(MTcpSocket *pSource) const
	{
		MPacket *newPacket = construct();
		newPacket->setSource(pSource);
		return newPacket;
	}

	void MPacket::setSource(MTcpSocket *pSource)
	{
		_source = pSource;
	}

	MTcpSocket* MPacket::source()
	{
		return _source;
	}

	quint16 MPacket::type() const { return _type; }
	QString MPacket::typeName() const { return _typeName; }

	bool MPacket::deleteOnSend() const
	{
		return _deleteOnSend;
	}

	//void MPacket::deleteOnSend(bool pDelete)
	//{
	//	_deleteOnSend = pDelete;
	//}

	bool MPacket::compressed() const
	{
		return _compress;
	}

	void MPacket::compress(bool pCompress)
	{
		_compress = pCompress;
	}

	QByteArray* MPacket::serialize() const
	{
		if(!_serializedData)
		{
			_serializedData = new QByteArray();
			serialize(_serializedData);
		}

		return _serializedData;
	}
};
