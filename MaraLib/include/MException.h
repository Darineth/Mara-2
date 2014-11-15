#ifndef MException_H
#define MException_H

#include <QString>

#include "MaraLibDll.h"
#include "MaraUtilityDefines.h"

namespace Mara
{
	class __MARALIB__CLASS_DECL MException
	{
		public:
			MException() { _source = ""; _message = "MException"; }
			MException(const QString &pSource, const QString &pMessage) { _source = pSource; _message = pMessage; }
			MException(const MException &pEx) { this->_source = pEx._source; this->_message = pEx._message; }
			virtual ~MException() {}

			const QString& source() const { return _source; }
			const QString& message() const { return _message; }
			virtual QString toString() const { return _source + ": " + _message; }

		private:
			QString _source;
			QString _message;
	};
};

#endif
