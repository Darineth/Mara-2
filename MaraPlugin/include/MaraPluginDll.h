#ifndef MARAPLUGINDLL_H
#define MARAPLUGINDLL_H

#include <QtGlobal>

#ifdef _WIN32
#ifdef __MARAPLUGINDLL__
#define __MARAPLUGIN__CLASS_DECL __declspec(dllexport)
#else
#define __MARAPLUGIN__CLASS_DECL __declspec(dllimport)
#endif
#else
#define __MARAPLUGIN__CLASS_DECL
#endif

#endif
