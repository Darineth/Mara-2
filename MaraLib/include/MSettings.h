#ifndef MSettings_H
#define MSettings_H

#include <QSettings>
#include <QColor>
#include <QPoint>
#include <QSize>
#include <QFont>
#include <QMutex>

#include "MaraLibDll.h"

namespace Mara
{
	class __MARALIB__CLASS_DECL MSettings : protected QSettings
	{
		Q_OBJECT;
		public:
			MSettings(const QString &pFileName, QObject *pParent = 0);
			virtual ~MSettings();

			template <typename SettingType>
			SettingType load(const QString &pSetting, const SettingType &pDefaultValue)
			{
				QMutexLocker lock(&_mutex);
				//return value(pSetting, QVariant(pDefaultValue)).value<QString>();
				return qVariantValue<SettingType>(value(pSetting, QVariant(pDefaultValue)));
				//return pDefaultValue;
			}

			template <typename SettingType>
			SettingType load(const QString &pSetting) const
			{
				QMutexLocker lock(&_mutex);
				//return value(pSetting, QVariant()).value<SettingType>();
				return qVariantValue<SettingType>(value(pSetting, QVariant()));
			}

			template <typename SettingType>
			void save(const QString &pSetting, const SettingType &pValue)
			{
				QMutexLocker lock(&_mutex);
				setValue(pSetting, QVariant(pValue));
			}

			template <typename SettingType>
			void setDefault(const QString &pSetting, const SettingType &pValue)
			{
				QMutexLocker lock(&_mutex);
				if(!contains(pSetting)) setValue(pSetting, QVariant(pValue));
			}

			void loadIntoMap(QSettings::SettingsMap *pMap);
			void loadFromMap(QSettings::SettingsMap *pMap);

			using QSettings::fileName;
			using QSettings::sync;

		private:
			mutable QMutex _mutex;
	};
};

#endif
