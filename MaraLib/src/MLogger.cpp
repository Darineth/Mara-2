#include "MLogger.h"

#include <QMutexLocker>
#include <QIODevice>

namespace Mara
{
	QMutex MLogger::_mutex;

	QMutex MaraTrace::_mutex;

	MLogger::MLogger() : _logMask(0xFFFFFFFF)
	{
	}

	MLogger::~MLogger()
	{
	}

	MLogger* MLogger::logger()
	{
		static MLogger instance;
		return &instance;
	}

	void MLogger::setMask(unsigned int pMask)
	{
		logger()->_logMask = pMask;
	}

	void MLogger::log(Mara::LogLevel::Level pLevel, const QString &pText)
	{
		if(logger()->_logMask && (unsigned int)pLevel)
		{
			QMutexLocker lock(&_mutex);
			logger()->emitLog(pLevel, pText);
		}
	}

	void MLogger::emitLog(Mara::LogLevel::Level pLevel, const QString &pText)
	{
		emit logged(pLevel, pText);
	}

	MDebugLogBuffer::MDebugLogBuffer(quint32 pMaxLines, QObject *pParent) : QObject(pParent), _maxLines(pMaxLines), _lines()
	{
	}

	MDebugLogBuffer::~MDebugLogBuffer()
	{
		_lines.clear();
	}

	void MDebugLogBuffer::log(QString pText)
	{
		_lines.append(pText);
		if(_lines.size() > _maxLines)
		{
			_lines.removeFirst();
		}
	}

	void MDebugLogBuffer::writeTo(QIODevice *pDevice)
	{
		QLinkedListIterator<QString> ii(_lines);

		while(ii.hasNext())
		{
			pDevice->write(ii.next().toUtf8());
			pDevice->write("\r\n");
		}
	}
};
