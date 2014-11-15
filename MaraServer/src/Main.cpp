#include <QDebug>
#include <QMutex>

#include <iostream>

#include "MaraServer.h"

#include "MLogger.h"

int Mara::MaraTrace::depth = 0;
QMutex Mara::MaraTrace::_mutex;


int main(int argc, char **argv)
{
	freopen("server_log.txt", "w", stderr);
	freopen("server_log.txt", "w", stdout);

	// Process some commandline args to see if we should be updating instead of running Mara proper.
	MaraServerApplication::init(argc, argv);
	int result = MaraServerApplication::execute();
	MaraServerApplication::destroy();
	return result;
}
