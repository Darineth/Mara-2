#include "MFileLogger.h"

#include <QDir>

#include "MException.h"

namespace Mara
{
	MFileLogger::MFileLogger(QObject *pParent) : QThread(pParent), _filenameTemplate("")
	{

	}

	MFileLogger::MFileLogger(const QString &pFilenameTemplate, QObject *pParent) : QThread(pParent), _filenameTemplate(pFilenameTemplate)
	{
		this->start();
	}

	MFileLogger::~MFileLogger()
	{
		stop();
	}

	void MFileLogger::setFilenameTemplate(const QString &pTemplate)
	{
		_filenameTemplate = pTemplate;
	}

	const QString& MFileLogger::filenameTemplate()
	{
		return _filenameTemplate;
	}

	void MFileLogger::run()
	{
		exec();
	}

	void MFileLogger::stop()
	{
		exit(0);
	}

	void MFileLogger::log(const QString &pText, const QString &pChannel)
	{
		if(_filenameTemplate != "")
		{
			QFileInfo fi(currentFilename(pChannel));
			QDir dir;
			if(!dir.exists(fi.path()))
			{
				dir.mkpath(fi.path());
			}

			QFile logFile(currentFilename(pChannel));
			if(!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
			{
				QString current = currentFilename(pChannel);
				_filenameTemplate = "";
				throw new MException(__FUNCTION_NAME__, "Error opening logfile " + current + "\n" + logFile.errorString());
			}

			logFile.write(("[" + pChannel + QDateTime::currentDateTime().toString(" yyyy-MM-dd hh:mm:ss] ") + pText + "\n").toAscii());
			logFile.close();
		}
	}

	const QString MFileLogger::currentFilename(const QString &pChannel)
	{
		return currentFilename(_filenameTemplate, pChannel);
	}

	const QString MFileLogger::currentFilename(const QString &pTemplate, const QString &pChannel)
	{
		QDate today = QDate::currentDate();
		QString logFile = pTemplate;

		QString channel = pChannel;

		static QRegExp invalidChars("[^"
									"A-Z,a-z,0-9,"
									"\\^,\\&,\\',\\@,"
									"\\{,\\},\\[,\\],"
									"\\,,\\$,\\=,\\!,"
									"\\-,\\#,\\(,\\),"
									"\\%,\\.,\\+,\\~,\\_"
									"]");

		channel.replace(invalidChars, "_");

		logFile.replace("%dddd", today.toString("dddd"), Qt::CaseInsensitive);
		logFile.replace("%ddd", today.toString("ddd"), Qt::CaseInsensitive);
		logFile.replace("%dd", today.toString("dd"), Qt::CaseInsensitive);
		logFile.replace("%d", today.toString("d"), Qt::CaseInsensitive);
		logFile.replace("%mmmm", today.toString("MMMM"), Qt::CaseInsensitive);
		logFile.replace("%mmm", today.toString("MMM"), Qt::CaseInsensitive);
		logFile.replace("%mm", today.toString("MM"), Qt::CaseInsensitive);
		logFile.replace("%m", today.toString("M"), Qt::CaseInsensitive);
		logFile.replace("%yyyy", today.toString("yyyy"), Qt::CaseInsensitive);
		logFile.replace("%yy", today.toString("yy"), Qt::CaseInsensitive);
		logFile.replace("%c", channel, Qt::CaseInsensitive);

		return logFile;
	}
}
