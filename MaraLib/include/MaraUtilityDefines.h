#ifndef MARAUTILITYDEFINES_H
#define MARAULTIITYDEFINES_H

#ifdef _MSC_VER
	#define __FUNCTION_NAME__ __FUNCSIG__
#else
	#define __FUNCTION_NAME__ __PRETTY_FUNCTION__
#endif

#ifdef __DEBUG__
	#include <QDebug>
	#define DEBUG_ONLY(x) x
#else
	#define DEBUG_ONLY(x) // x
#endif

#endif

