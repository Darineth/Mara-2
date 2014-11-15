#include "MaraPluginManager.h"
#include "MaraPlugin.h"

#include <QDir>
#include <QPluginLoader>
#include <QApplication>

namespace Mara
{
	MaraPluginManager::MaraPluginManager(QObject *pqoParent) : QObject(pqoParent)
	{
		QDir qdirPlugins(qApp->applicationDirPath());

#if defined(Q_OS_MAC)
		if (qdirPlugins.dirName() == "MacOS")
		{
			qdirPlugins.cdUp();
			qdirPlugins.cdUp();
			qdirPlugins.cdUp();
		}
#endif

		qdirPlugins.cd("plugins");
		qdirPlugins.cd("mara");

		foreach(QString qsFileName, qdirPlugins.entryList(QDir::Files))
		{
			QPluginLoader pqlLoader(qdirPlugins.absoluteFilePath(qsFileName));
			QObject* qoPlugin = pqlLoader.instance();
			MaraPlugin* mpPlugin;
			if(qoPlugin && (mpPlugin = qobject_cast<MaraPlugin*>(qoPlugin)))
			{
				mpPlugin->setParent(this);
				plugins().append(mpPlugin);
				Mara::MLogger::log(Mara::LogLevel::AppStatus, "Loaded plugin: " + qsFileName);
			}
			else
			{
				Mara::MLogger::log(Mara::LogLevel::Error, "Error loading plugin: " + qsFileName);
				delete qoPlugin;
			}
		}
	}

	MaraPluginManager::~MaraPluginManager()
	{

	}

	QList<MaraPlugin*>& MaraPluginManager::plugins()
	{
		static QList<MaraPlugin*> qlPlugins;

		return qlPlugins;
	}

	MaraPluginExecuter& MaraPluginManager::pluginExecute()
	{
		static MaraPluginExecuter mpeExecuter;

		return mpeExecuter;
	}

	QString MaraPluginExecuter::preprocessText(const QString& pqsText)
	{
		QString qsProcessed = pqsText;
		foreach(MaraPlugin* mpPlugin, MaraPluginManager::plugins())
		{
			qsProcessed = mpPlugin->preprocessText(qsProcessed);
		}

		return qsProcessed;
	}

	QString MaraPluginExecuter::postprocessText(const QString& pqsText)
	{
		QString qsProcessed = pqsText;
		foreach(MaraPlugin* mpPlugin, MaraPluginManager::plugins())
		{
			qsProcessed = mpPlugin->postprocessText(qsProcessed);
		}

		return qsProcessed;
	}

	QString MaraPluginExecuter::preprocessOutgoing(const QString& pqsText)
	{
		QString qsProcessed = pqsText;
		foreach(MaraPlugin* mpPlugin, MaraPluginManager::plugins())
		{
			qsProcessed = mpPlugin->preprocessOutgoing(qsProcessed);
		}

		return qsProcessed;
	}
}
