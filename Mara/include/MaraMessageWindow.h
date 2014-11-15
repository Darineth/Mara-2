#ifndef MARAMESSAGEWINDOW_H
#define MARAMESSAGEWINDOW_H

#include "MaraChatWidget.h"

#include "ui_MaraMessageWindow.h"

#include <QDialog>

class MaraMessageWindow : public QDialog, public Ui::MaraMessageWindow, public MaraChatDisplay
{
	Q_OBJECT;
	public:
		MaraMessageWindow(QWidget *pParent = 0);
		~MaraMessageWindow();

		void associateUser(Mara::MUser *pUser);
		void disassociateUser();
		Mara::MUser *user();

		void appendHtml(const QString &pHtml);

		void setTitle();

		void getAttention();

	public slots:
		void setStyleSheet();

	signals:
		void textEntered(Mara::MUser *pUser, QString pText);
		void closed();

	protected:
		virtual void closeEvent(QCloseEvent *pEvent);

	private slots:
		void chatWidgetTextEntered(QString pText);

	private:
		Mara::MUser *_messageUser;

};

#endif
