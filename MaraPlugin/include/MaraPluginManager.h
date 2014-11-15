#ifndef MARAPLUGINMANAGER_H
#define MARAPLUGINMANAGER_H

#include "MaraPluginDll.h"
#include "MaraPlugin.h"
#include "MLogger.h"

#include <QObject>
#include <QList>

namespace Mara
{
	class __MARAPLUGIN__CLASS_DECL MaraPluginExecuter : public MaraPlugin
	{
		Q_OBJECT;

		friend class MaraPluginManager;

		public:
			QString preprocessText(const QString& pqsText);
			QString postprocessText(const QString& pqsText);

			QString preprocessOutgoing(const QString& pqsText);

		private:
			MaraPluginExecuter() {}
			virtual ~MaraPluginExecuter() {}
	};

	class __MARAPLUGIN__CLASS_DECL MaraPluginManager : public QObject
	{
		Q_OBJECT;

		friend class MaraPluginExecuter;

		public:
			MaraPluginManager(QObject *pqoParent = 0);
			virtual ~MaraPluginManager();

			static MaraPluginExecuter& pluginExecute();

		private:
			static QList<MaraPlugin*>& plugins();
	};
}

#endif
