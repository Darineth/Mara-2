#include "MaraServerWindow.h"

#include <QScrollBar>

MaraServerWindow::MaraServerWindow() : QMainWindow()
{
	setupUi(this);
	setWindowIcon(QIcon(":images/MaraServer.ico"));
	setObjectName("MaraServerWindow");
	qteConsole->setFont(QFont("Courier New", 8));
	_hidden = false;
	_wasMaximized = false;

	setWindowTitle(QString("Mara Server 2.2.7 - Qt %1").arg(qVersion()));
}

MaraServerWindow::~MaraServerWindow()
{

}

void MaraServerWindow::log(Mara::LogLevel::Level pLevel, const QString &pText)
{
	qteConsole->appendPlainText(QString("[%1] %2").arg(pLevel).arg(pText));
	qteConsole->verticalScrollBar()->setValue(qteConsole->verticalScrollBar()->maximum());
}

void MaraServerWindow::changeEvent(QEvent *pEvent)
{
	if(isMinimized() && !_hidden)
	{
		_hidden = true;
		emit minimized();
		_wasMaximized = isMaximized();
		setWindowFlags(Qt::FramelessWindowHint);
	}
	QMainWindow::changeEvent(pEvent);
}

void MaraServerWindow::restore()
{
	if(_hidden)
	{
		_hidden = false;
		setWindowFlags(0);
		show();

		if(_wasMaximized)
			showMaximized();
		else
			showNormal();
	}
}

void MaraServerWindow::rebuildUserList()
{

}
