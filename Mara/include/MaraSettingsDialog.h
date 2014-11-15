#ifndef MaraSettingsDialog_H
#define MaraSettingsDialog_H

#include "ui_MaraSettingsDialog.h"

#include <QListWidget>
#include <QSignalMapper>
#include <QColorDialog>
#include <QFontDialog>
#include <QMessageBox>

#include "MaraClientSettings.h"
#include "MFileLogger.h"
#include "MSettings.h"
#include "MTextProcessors.h"
#include "MTextStyle.h"

class MaraSettingsDialog : public QDialog, private Ui::MaraSettingsDialog
{
	Q_OBJECT;

	public:
		MaraSettingsDialog(QWidget *pParent);

		virtual ~MaraSettingsDialog();

		void saveSettings();

	private slots:
		void changeSettingPage(QListWidgetItem *current, QListWidgetItem *previous);
		void selectColor(const QString &pSetting);
		void selectFont(const QString &pSetting);
		void updateTemplatePreview(const QString &pTemplate);

		void generateToken();
		void updateNames(const QString &pText);
		void updateLogFile();
		void updateTimestamp();

		void validateAndAccept();

	private:
		void loadSettings();
		bool validateSettings();

		void showEvent(QShowEvent *pEvent);

		template <typename SettingType>
		SettingType getSetting(const QString &setting) const
		{
			//return _tempSettings->value(setting).value<SettingType>();
			return qVariantValue<SettingType>(_tempSettings->value(setting));
		}

		template <typename SettingType>
		void setSetting(const QString &pSetting, const SettingType &pValue)
		{
			_tempSettings->insert(pSetting, QVariant(pValue));
		}

		QSettings::SettingsMap *_tempSettings;
		QSignalMapper *_colorButtonMapper;
		QSignalMapper *_fontButtonMapper;
		QSignalMapper *_templateUpdateMapper;

		Mara::MTextStyle _defaultStyle;
		Mara::MTextStyle _systemStyle;
		Mara::MTextStyle _userStyle;
		Mara::MTextStyle _messageStyle;
};

#endif
