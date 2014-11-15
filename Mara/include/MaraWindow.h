#ifndef MARAWINDOW_H
#define MARAWINDOW_H

#include <QCloseEvent>
#include <QHash>
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QObject>
#include <QPoint>

#include "MaraChannelWidget.h"
#include "MaraMessageWindow.h"
#include "MSettings.h"
#include "ui_Mara.h"

class MaraWindow : public QMainWindow, public Ui::MaraWindow
{
	Q_OBJECT;

	public:
		MaraWindow(QWidget *pParent);
		virtual ~MaraWindow();

		bool welcomeTabOpen() const;
		void closeWelcomeTab();

		MaraChannelWidget *getChannelWidget(const QString &pChannel);
		MaraChannelWidget *currentChannelWidget();

		MaraMessageWindow *getMessageWidget(Mara::MUser::Token pUserToken, bool pAllowCreate, bool pForceCreate);

		QList<MaraChannelWidget*> channelWidgets() const;

	public slots:
		void reloadSettings();
		void openMessageWindow(QString pUserName);
		void toggleFreeze();
		void clearChat();

	signals:
		void channelTextEntered(Mara::MChannel *pChannel, QString pText);
		void messageTextEntered(Mara::MUser *pUser, QString pText);
		void updateStyleSheets();

	protected:
		virtual bool eventFilter(QObject *pObject, QEvent *pEvent);
		virtual void closeEvent(QCloseEvent *pEvent);

	private slots:
		void currentTabChanged(int index);
		void messageWindowClosed();

	private:

		bool _welcomeTabOpen;

		QHash<QString, QPointer<MaraChannelWidget> > _channelWidgets;
		QHash<Mara::MUser::Token, QPointer<MaraMessageWindow> > _messageWindows;
		QHash<QString, QPointer<MaraMessageWindow> > _messageWindowsByName;
		QPointer<MaraChannelWidget> _currentChannelWidget;
};

#endif
