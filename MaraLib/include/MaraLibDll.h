#ifndef MARALIBDLL_H
#define MARALIBDLL_H

#include <QtGlobal>

#ifdef _WIN32
	#ifdef __MARALIBDLL__
		#define __MARALIB__CLASS_DECL __declspec(dllexport)
	#else
		#define __MARALIB__CLASS_DECL __declspec(dllimport)
	#endif
#else
	#define __MARALIB__CLASS_DECL
#endif

namespace Mara
{
	__MARALIB__CLASS_DECL quint32 mLibraryVersion();
}

#endif
