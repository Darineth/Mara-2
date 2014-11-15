#ifndef MARACHATWIDGET_H
#define MARACHATWIDGET_H

#include <QWidget>

#include "MSettings.h"
#include "MTextStyle.h"
#include "MaraClientSettings.h"
#include "MTextProcessors.h"

#include "ui_MaraChatWidget.h"

class MaraChatDisplay
{
	public:
		virtual ~MaraChatDisplay() {}
		virtual void appendHtml(const QString &pHtml) = 0;
};

class MaraChatWidget : public QWidget, public Ui::MaraChatWidget, public MaraChatDisplay
{
	Q_OBJECT;

	public:
		MaraChatWidget(QWidget *pParent);
		~MaraChatWidget();

		void toggleFreeze();
		void clear();

	public slots:
		void reloadSettings();
		void appendText(const Mara::MTextStyle &pStyle, const QString &pText, bool pAllowTimestamp = true);
		void appendHtml(const QString &pHtml);
		void setStyleSheet();

	signals:
		void textEntered(QString pText);

	private:
		bool eventFilter(QObject *pObject, QEvent *pEvent);

		bool _timestamps;
		QString _timestampFormat;

};

#endif
