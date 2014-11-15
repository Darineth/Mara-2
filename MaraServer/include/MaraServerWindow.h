#ifndef MARASERVERWINDOW_H
#define MARASERVERWINDOW_H

#include <QMainWindow>

#include "MLogLevel.h"

#include "ui_MaraServer.h"

class MaraServerWindow : public QMainWindow, public Ui::MaraServer
{
	Q_OBJECT;
	public:
		MaraServerWindow();
		~MaraServerWindow();

		void log(Mara::LogLevel::Level pLevel, const QString &pText);

		void restore();

	public slots:
		void rebuildUserList();

	signals:
		void minimized();

	private:
		void changeEvent(QEvent *pEvent);

		bool _wasMaximized;
		bool _hidden;
};

#endif
