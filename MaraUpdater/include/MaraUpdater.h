#ifndef MARAUPDATER_H
#define MARAUPDATER_H

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QProcess>

#include "MaraUpdaterWindow.h"

class MaraUpdaterApplication : public QApplication
{
	Q_OBJECT;
	public:
		MaraUpdaterApplication(int &argc, char **argv);
		~MaraUpdaterApplication();

		static void handleQtMessage(QtMsgType type, const char *msg);

		static MaraUpdaterApplication *_app;

		// UI
		MaraUpdaterWindow *_window;

		bool allowExecute();

	public slots:
		void updateFinished();

	private:
		void executeUpdate();

		friend void qtMessageHandler(QtMsgType type, const char *msg);

		bool _executeCancelled;

		bool _autoRestart;
		bool _launchedFromTemp;
};

void qtMessageHandler(QtMsgType type, const char *msg);

#endif
