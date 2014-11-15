#ifndef MFILELOGGER_H
#define MFILELOGGER_H

#include <QFile>
#include <QThread>
#include <QMutex>
#include <QDate>

#include "MaraLibDll.h"

#include "MLogLevel.h"

namespace Mara
{
	class __MARALIB__CLASS_DECL MFileLogger : public QThread
	{
		public:
			MFileLogger(QObject *pParent = 0);
			MFileLogger(const QString &pFilenameTemplate, QObject *pParent = 0);
			~MFileLogger();

			void setFilenameTemplate(const QString &pTemplate);
			const QString& filenameTemplate();

			static const QString currentFilename(const QString &pTemplate, const QString &pChannel);
			const QString currentFilename(const QString &pChannel);

			void run();
			void stop();

		public slots:
			void log(const QString &pText, const QString &pChannel);

		private:
			QString _filenameTemplate;
			QMutex _fileMutex;
	};
}

#endif
