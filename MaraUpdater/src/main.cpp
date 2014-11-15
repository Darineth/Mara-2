#include <QApplication>
#include <QTimer>

#include "MaraUpdater.h"
//#include "MaraTrace.h"

//int MaraTrace::depth = 0;

int main(int argc, char **argv)
{
	freopen("updater_out.txt", "w", stdout);
	//MaraTrace trace(__FUNCSIG__, 0, __FILE__, __LINE__);
	MaraUpdaterApplication app(argc, argv);
	if(app.allowExecute())
	{
		int result = MaraUpdaterApplication::exec();
		return result;
	}
	else
	{
		return 0;
	}
}
