#include "MSettings.h"

namespace Mara
{
	MSettings::MSettings(const QString &pFileName, QObject *pParent) : QSettings(pFileName, QSettings::IniFormat, pParent), _mutex()
	{
	}

	MSettings::~MSettings()
	{
	}

	void MSettings::loadIntoMap(QSettings::SettingsMap *pMap)
	{
		QMutexLocker lock(&_mutex);
		QStringList settings = allKeys();

		for(int ii = 0; ii < settings.size(); ++ii)
		{
			pMap->insert(settings.at(ii), value(settings.at(ii)));
		}
	}

	void MSettings::loadFromMap(QSettings::SettingsMap *pMap)
	{
		QMutexLocker lock(&_mutex);
		for(QSettings::SettingsMap::const_iterator ii = pMap->constBegin(); ii != pMap->constEnd(); ++ii)
		{
			setValue(ii.key(), ii.value());
		}
	}
};
