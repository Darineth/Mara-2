#ifndef MARAPLUGIN_H
#define MARAPLUGIN_H

#include "MaraPluginDll.h"

#include <QtPlugin>

namespace Mara
{
	class __MARAPLUGIN__CLASS_DECL MaraPlugin : public QObject
	{
		public:
			MaraPlugin(QObject* pqoParent = 0);
			virtual ~MaraPlugin();

			virtual QString preprocessText(const QString& pqsText);
			virtual QString postprocessText(const QString& pqsText);

			virtual QString preprocessOutgoing(const QString& pqsText);
	};
}

Q_DECLARE_INTERFACE(Mara::MaraPlugin, "Mara.Client.Plugin/1.0")

#endif
