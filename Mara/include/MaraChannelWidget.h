#ifndef MARACHANNELWIDGET_H
#define MARACHANNELWIDGET_H

#include <QHash>
#include <QListWidget>
#include <QPointer>
#include <QWidget>

#include "MaraChatWidget.h"
#include "MChannel.h"
#include "MTextStyle.h"

#include "ui_MaraChannelWidget.h"

class MaraChannelWidget : public QWidget, public Ui::MaraChannelWidget, public MaraChatDisplay
{
	Q_OBJECT;

	public:
		MaraChannelWidget(QWidget *pParent = 0);
		~MaraChannelWidget();

		void reloadSettings();

		void associateChannel(Mara::MChannel *pChannel);
		void dissociateChannel();

		void appendText(const Mara::MTextStyle &pStyle, const QString &pText, bool pAllowTimestamp = true);
		void appendHtml(const QString &pHtml);

		const QString& channelName() const;
		void setChannelName(const QString &pChannelName);

		static MaraChannelWidget* getChannelWidget(Mara::MChannel* pChannel);

		void toggleFreeze();
		void clear();

	public slots:
		void setStyleSheet();
		void updateUserList();
		void userDoubleClicked(QListWidgetItem *pItem);

	signals:
		void textEntered(Mara::MChannel *pChannel, QString pText);
		void openMessageWindow(QString pUserName);

	private slots:
		void splitterMoved(int pPos, int pIndex);
		void chatWidgetTextEntered(QString pText);

	private:
		bool eventFilter(QObject *pObject, QEvent *pEvent);

		QPointer<Mara::MChannel> _channel;
		QString _channelName;

		static QHash<Mara::MChannel*, MaraChannelWidget*> _channelToWidget;
};

#endif
