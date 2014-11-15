#ifndef MARAUPDATERWINDOW_H
#define MARAUPDATERWINDOW_H

#include "ui_MaraUpdateWindow.h"

#include <QHttp>

#include "MaraUpdateMD5.h"

class MaraUpdaterWindow : public QDialog, public Ui::MaraUpdateWindow
{
	Q_OBJECT;

	enum UpdateStatus
	{
		Ready,
		Error,
		GetFiles,
		CheckLocalFiles,
		DownloadUpdates,
		UpdateComplete
	};

	public:
		MaraUpdaterWindow();
		~MaraUpdaterWindow();

	public slots:
		void executeUpdate();

	signals:
		void finished();

	private slots:
		void httpStateChanged(int pStatus);
		void httpRequestFinished(int pId, bool pError);
		void httpDataRead(int pDone, int pTotal);
		void readResponseHeader(const QHttpResponseHeader &pResponseHeader);

		void md5Finished();

	private:
		void log(const QString &pText);
		void setStatus(UpdateStatus pStatus);

		void checkNextFile();
		void checkFile();
		void getNextFile();
		void getFile();

		bool _updating;

		UpdateStatus _status;

		QHttp _http;

		QStringList _updateFiles;
		QStringList::iterator _updateFileIterator;

		MaraUpdateMD5 *_md5;
		QString _currentFile;
		QByteArray _currentMd5;
		QIODevice *_getFileOutput;
		int _getBytes;

		QString _outputFile;
};

#endif
