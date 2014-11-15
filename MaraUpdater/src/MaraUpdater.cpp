#include "MaraUpdater.h"

#include <QMessageBox>
#include <QProcess>
#include <QTimer>

#include <iostream>

//#include "MaraTrace.h"

MaraUpdaterApplication *MaraUpdaterApplication::_app = 0;

MaraUpdaterApplication::MaraUpdaterApplication(int &argc, char **argv) : QApplication(argc, argv),
		_window(new MaraUpdaterWindow())
{
	//MaraTrace trace(__FUNCSIG__, this, __FILE__, __LINE__);

	_app = this;
	qInstallMsgHandler(qtMessageHandler);

	_executeCancelled = false;

	_autoRestart = false;
	_launchedFromTemp = false;

	QStringList args = arguments();

	for(QStringList::iterator ii = args.begin(); ii != args.end(); ++ii)
	{
		if((*ii).toLower() == "autorestart")
		{
			_autoRestart = true;
		}

		if((*ii) == "__TEMP_LAUNCHED__")
		{
			_launchedFromTemp = true;
		}
	}

	if(_launchedFromTemp)
	{
		//MaraTrace trace(__FUNCSIG__ ":TempLaunched", this, __FILE__, __LINE__);
		executeUpdate();
	}
	else
	{
		//MaraTrace trace(__FUNCSIG__ ":NotTempLaunched", this, __FILE__, __LINE__);
		QFileInfo exeFileInfo(QCoreApplication::applicationFilePath());
		QString tempPath = QDir::temp().filePath(exeFileInfo.fileName());
		QFile temp(tempPath);

		if(temp.exists() && !temp.remove())
		{
			QMessageBox::warning(0, "Error", QString("Error removing temp executable %1:\n%2\n\nUpdater will not be able to update itself.").arg(tempPath).arg(temp.errorString()));
			executeUpdate();
		}

		QFile exeFile(QCoreApplication::applicationFilePath());

		std::cout << "Copying self to " << qPrintable(tempPath) << "..." << std::flush;

		if(exeFile.copy(tempPath))
		{
			std::cout << "Success!" << std::flush;
			if(!QProcess::startDetached(tempPath, args << "__TEMP_LAUNCHED__"))
			{
				QMessageBox::critical(0, "Error", QString("Failed launching temp updater."));
			}
			_executeCancelled = true;
		}
		else
		{
			QMessageBox::warning(0, "Error", QString("Error copying to temp location %1:\n%2\n\nUpdater will not be able to update itself.").arg(tempPath).arg(exeFile.errorString()));
			executeUpdate();
		}
	}
}

MaraUpdaterApplication::~MaraUpdaterApplication()
{
	//MaraTrace trace(__FUNCSIG__, this, __FILE__, __LINE__);

}

void MaraUpdaterApplication::handleQtMessage(QtMsgType type, const char *msg)
{
	std::cout << type << ": " << msg << std::flush;
}

void MaraUpdaterApplication::updateFinished()
{
	//MaraTrace trace(__FUNCSIG__, this, __FILE__, __LINE__);
	if(_autoRestart)
	{
		QProcess::startDetached("Mara.exe autoconnect");
		exit();
	}
}

bool MaraUpdaterApplication::allowExecute()
{
	//MaraTrace trace(__FUNCSIG__, this, __FILE__, __LINE__);
	return !_executeCancelled;
}

void MaraUpdaterApplication::executeUpdate()
{
	//MaraTrace trace(__FUNCSIG__, this, __FILE__, __LINE__);
	connect(_window, SIGNAL(finished()), this, SLOT(updateFinished()));
	_window->show();
	QTimer::singleShot(1000, _window, SLOT(executeUpdate()));
}

void qtMessageHandler(QtMsgType type, const char *msg)
{
	MaraUpdaterApplication::_app->handleQtMessage(type, msg);
}
