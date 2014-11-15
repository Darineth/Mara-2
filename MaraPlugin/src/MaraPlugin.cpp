#include "MaraPlugin.h"

#include <QString>

namespace Mara
{
	MaraPlugin::MaraPlugin(QObject* pqoParent) : QObject(pqoParent)
	{

	}

	MaraPlugin::~MaraPlugin()
	{

	}

	QString MaraPlugin::preprocessText(const QString& pqsText)
	{
		return pqsText;
	}

	QString MaraPlugin::postprocessText(const QString& pqsText)
	{
		return pqsText;
	}

	QString MaraPlugin::preprocessOutgoing(const QString& pqsText)
	{
		return pqsText;
	}
}
