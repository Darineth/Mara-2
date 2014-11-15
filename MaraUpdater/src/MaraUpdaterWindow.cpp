#include "MaraUpdaterWindow.h"

#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QUrl>

//#include "//MaraTrace.h"

MaraUpdaterWindow::MaraUpdaterWindow() :QDialog(0, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint),
		_status(Ready),
		_http("mara.pretoast.com")
{
	//MaraTrace trace(__FUNCSIG__, this, __FILE__, __LINE__);
	setupUi(this);

	setStatus(Ready);

	connect(qpbUpdate, SIGNAL(clicked()), this, SLOT(executeUpdate()));

	connect(&_http, SIGNAL(stateChanged(int)), this, SLOT(httpStateChanged(int)));
	connect(&_http, SIGNAL(requestFinished(int, bool)), this, SLOT(httpRequestFinished(int, bool)));
	connect(&_http, SIGNAL(dataReadProgress(int, int)), this, SLOT(httpDataRead(int, int)));
	connect(&_http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)), this, SLOT(readResponseHeader(const QHttpResponseHeader &)));

	this->setWindowTitle("Mara Updater 2.2.5");
}

MaraUpdaterWindow::~MaraUpdaterWindow() {}

void MaraUpdaterWindow::executeUpdate()
{
	//MaraTrace trace(__FUNCSIG__, this, __FILE__, __LINE__);
	qteProgress->clear();
	_updateFiles.clear();

	setStatus(GetFiles);

	log(QString("Getting file list from mara.pretoast.com..."));

	_http.get("/Update/update.php");
}

void MaraUpdaterWindow::httpStateChanged(int pStatus)
{
}

void MaraUpdaterWindow::httpRequestFinished(int pId, bool pError)
{
	//log(QString("Request Finished: %1 - %2").arg(pId).arg(pError));
	Q_UNUSED(pId);

	if(pError)
	{
		log(QString("Error: %1: %2").arg(_http.error()).arg(_http.errorString()));
		setStatus(Error);
		return;
	}

	switch(_status)
	{
		case GetFiles:
			_updateFiles = QString(_http.readAll()).split('\n', QString::SkipEmptyParts);

			if(_updateFiles.size() == 0)
			{
				log("Error: No files available for update.");
				setStatus(Error);
				return;
			}
			else
			{
				setStatus(CheckLocalFiles);
				log("Checking local files...");

				qbrProgress->setMaximum(_updateFiles.size());
				qbrProgress->setValue(0);

				_updateFileIterator = _updateFiles.begin();

				checkFile();
			}

			break;

		case DownloadUpdates:

			QHttpResponseHeader response = _http.lastResponse();

			if(_getBytes == 0)
			{
				log("Error: Received no data.");
				setStatus(Error);
				return;
			}
			else
			{
				log(QString("Finished Download.  Received %1 bytes.").arg(_getBytes));
			}

			_getFileOutput->close();
			delete _getFileOutput;
			_getFileOutput = 0;

			/*if(_http.bytesAvailable() == 0)
			{
				log("Updated file is 0 bytes.");
			}
			else
			{
				QFile file(_outputFile);
				if(!file.open(QIODevice::WriteOnly))
				{
					log("Error opening file for update: " + file.errorString());
					setStatus(Error);
					return;
				}

				if(!file.write(_http.readAll()))
				{
					log("Error writing updated file: " + file.errorString());
					setStatus(Error);
					file.close();
					return;
				}
				file.close();

				log("Finished downloading " + _currentFile);
			}*/

			++_updateFileIterator;
			getNextFile();
			break;
	}
}

void MaraUpdaterWindow::httpDataRead(int pDone, int pTotal)
{
	qbrProgress->setMaximum(pTotal);
	qbrProgress->setValue(pDone);
	if(pTotal > 0)
		qlbProgress->setText(QString("%1%").arg((int)((float)(pDone * 100)/(float)(pTotal))));
	else
		qlbProgress->setText("");
	qlbProgressCount->setText(QString("%1/%2").arg(pDone).arg(pTotal));
	_getBytes = pDone;
}

void MaraUpdaterWindow::readResponseHeader(const QHttpResponseHeader &pResponseHeader)
{
	switch(pResponseHeader.statusCode())
	{
		case 200: // Ok
		case 301: // Moved Permanently
		case 302: // Found
		case 303: // See Other
		case 307: // Temporary Redirect
			break;

		default:
			QMessageBox::information(this, tr("HTTP"), tr("Download failed: %1.").arg(pResponseHeader.reasonPhrase()));
			setStatus(Error);
			_http.abort();
	}
}

void MaraUpdaterWindow::md5Finished()
{
	if(_md5->md5() == _currentMd5)
	{
		_updateFileIterator = _updateFiles.erase(_updateFileIterator);
		log("File up to date.");
	}
	else if(_md5->md5() != "")
	{
		log(tr("File needs update:\r\n\tLocal hash: %1\r\n\tServer hash: %2").arg(QString(_md5->md5())).arg(QString(_currentMd5)));
		++_updateFileIterator;
	}
	else
	{
		log("Local file does not exist.");
		++_updateFileIterator;
	}
	_md5->deleteLater();
	qbrProgress->setValue(qbrProgress->value() + 1);
	checkNextFile();
}

void MaraUpdaterWindow::log(const QString &pText)
{
	qteProgress->appendPlainText(QDateTime::currentDateTime().toString("[hh:mm:ss] ") + pText);
}

void MaraUpdaterWindow::setStatus(UpdateStatus pStatus)
{
	_status = pStatus;
	switch(pStatus)
	{
		case Ready:
			qlbStatus->setText("Ready");
			qpbUpdate->setEnabled(true);
			break;
		case Error:
			qlbStatus->setText("Error");
			qpbUpdate->setEnabled(true);
			break;
		case GetFiles:
			qlbStatus->setText("Getting File List");
			qpbUpdate->setEnabled(false);
			break;
		case CheckLocalFiles:
			qlbStatus->setText("Checking Local Files");
			qpbUpdate->setEnabled(false);
			break;
		case DownloadUpdates:
			qlbStatus->setText("Downloading Updates");
			qpbUpdate->setEnabled(false);
			break;
	}
}

void MaraUpdaterWindow::checkNextFile()
{
	if(_updateFileIterator != _updateFiles.end())
	{
		checkFile();
	}
	else if(_updateFiles.size() == 0)
	{
		log("All files up to date.");
		setStatus(UpdateComplete);
		emit finished();
	}
	else
	{
		_updateFileIterator = _updateFiles.begin();
		setStatus(DownloadUpdates);
		getFile();
	}
}

void MaraUpdaterWindow::getNextFile()
{
	if(_updateFileIterator != _updateFiles.end())
	{
		getFile();
	}
	else
	{
		setStatus(UpdateComplete);
		log("Update Complete.");
		emit finished();
	}
}

void MaraUpdaterWindow::checkFile()
{
	QStringList fileAndMd5((*_updateFileIterator).split('\t'));

	_currentFile = fileAndMd5.at(0);
	_currentMd5 = fileAndMd5.at(1).toLatin1();

	log("Checking file: " + _currentFile);
	_md5 = new MaraUpdateMD5(QDir::current().absoluteFilePath(_currentFile));
	connect(_md5, SIGNAL(finished()), this, SLOT(md5Finished()));
	_md5->start();
}

void MaraUpdaterWindow::getFile()
{
	_currentFile = (*_updateFileIterator).left((*_updateFileIterator).indexOf('\t'));

	_outputFile = QDir::current().absoluteFilePath(_currentFile);
	
	if(QFile::exists(_outputFile))
	{
		QFile::remove(_outputFile);
	}

	_getFileOutput = new QFile(_outputFile);
	if(!_getFileOutput->open(QIODevice::WriteOnly))
	{
		log("Error opening file for update: " + _getFileOutput->errorString());
		setStatus(Error);
		return;
	}

	_getBytes = 0;

	log("Downloading " + _currentFile);
	QString path = "/Update/" + _currentFile;
	_http.get(path, _getFileOutput);
}
