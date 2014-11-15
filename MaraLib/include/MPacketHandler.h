#ifndef MPACKETHANDLER_H
#define MPACKETHANDLER_H

#include <QHash>

#include "MPacket.h"

namespace Mara
{
	class MHandlerCallbackWrapper
	{
	public:
		virtual ~MHandlerCallbackWrapper() {}
		virtual void operator ()(MPacket *pPacket) = 0;
	};

	template <class HandlingClass>
	class MHandlerCallback : public MHandlerCallbackWrapper
	{
		typedef void (HandlingClass::*HandlingFunction)(MPacket*);

		public:
			MHandlerCallback(HandlingClass *pObj, HandlingFunction pFunction) : _object(pObj), _function(pFunction) {}
			virtual void operator ()(MPacket *pPacket) { (_object->*_function)(pPacket); }

		private:
			HandlingClass *_object;
			HandlingFunction _function;
	};

	class __MARALIB__CLASS_DECL MPacketHandler : public QObject
	{
		Q_OBJECT;
		MHandlerCallbackWrapper *_defaultHandler;

		public:
			MPacketHandler(MHandlerCallbackWrapper *pDefaultHandler);
			~MPacketHandler();

			void registerHandler(quint16 pType, MHandlerCallbackWrapper *pHandler);

		public slots:
			void handlePacket(Mara::MPacket *pPacket) const;

		private:
			QHash<quint16, MHandlerCallbackWrapper*> _handlers;

	};
};

#endif
