#ifndef MARACLIENTDLL_H
#define MARACLIENTDLL_H

#include <QtGlobal>

#ifdef _WIN32
	#ifdef __MARACLIENTDLL__
		#define __MARACLIENT__CLASS_DECL __declspec(dllexport)
	#else
		#define __MARACLIENT__CLASS_DECL __declspec(dllimport)
	#endif
#else
	#define __MARACLIENT__CLASS_DECL
#endif

namespace MaraClient
{
	__MARACLIENT__CLASS_DECL quint32 mClientVersion();
}

#endif
