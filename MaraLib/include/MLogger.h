#ifndef MARALOGGER_H
#define MARALOGGER_H

#include <QObject>
#include <QMutex>
#include <QLinkedList>

#include "MaraLibDll.h"
#include "MLogLevel.h"

//#ifdef __DEBUG__
#include <iostream>
#define __TRACE__ Mara::MaraTrace trace(__FUNCSIG__, this, __FILE__, __LINE__);
//#else
//#define __TRACE__
//#endif

class QIODevice;

namespace Mara
{
	class __MARALIB__CLASS_DECL MLogger : public QObject
	{
		Q_OBJECT;

		public:
			static MLogger* logger();
			static void setMask(unsigned int pMask);
			static void log(Mara::LogLevel::Level pLevel, const QString &pText);

		signals:
			void logged(Mara::LogLevel::Level pLevel, const QString &pText);

		private:
			MLogger();
			~MLogger();

			void emitLog(Mara::LogLevel::Level pLevel, const QString &pText);

			static QMutex _mutex;
			unsigned int _logMask;
	};

	class MaraTrace
	{
		public:
			MaraTrace(const char* pFunction, const void* pObj, const char* pFile, int pLine) : _function(pFunction), _obj(pObj), _file(pFile), _line(pLine)
			{
//#ifdef __DEBUG__
				_mutex.lock();
				std::cout << std::string(depth, '\t') << _function << ": " << _obj << " at " << _file << ":" << _line << "\n";
				std::cout << std::string(depth, '\t') << "{\n" << std::flush;
				depth++;
				_mutex.unlock();
//#endif
			}

			~MaraTrace()
			{
//#ifdef __DEBUG__
				_mutex.lock();
				depth--;
				std::cout << std::string(depth, '\t') << "} [" << _function << "]\n" << std::flush;
				//std::cout << std::string(depth, '\t') << _function << ": " << _obj << " at " << _file << ":" << _line << "\n";
//#endif
				_mutex.unlock();
			}

			const char* _function;
			const void* _obj;
			const char* _file;
			int _line;

			static int depth;
			static QMutex _mutex;
	};

	class __MARALIB__CLASS_DECL MDebugLogBuffer : public QObject
	{
		Q_OBJECT;

		public:
			MDebugLogBuffer(quint32 pMaxLines, QObject *pParent = 0);
			virtual ~MDebugLogBuffer();

			void log(QString pText);

			void writeTo(QIODevice *pDevice);

		private:
			QLinkedList<QString> _lines;
			quint32 _maxLines;
	};
};

#endif