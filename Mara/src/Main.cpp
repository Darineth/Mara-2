#include <QCoreApplication>
#include <QDebug>

#include "Mara.h"
#include "MaraWindow.h"

int main(int argc, char **argv)
{
	DEBUG_ONLY(freopen("maraclient_log.txt", "w", stderr));
	DEBUG_ONLY(freopen("maraclient_log.txt", "w", stdout));

	MaraApplication::init(argc, argv);
	int result = MaraApplication::execute();
	MaraApplication::destroy();
	return result;
}
