#ifndef MCHATBROWSER_H
#define MCHATBROWSER_H

#include <QApplication>
#include <QBasicTimer>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QWebView>
#include <QWebFrame>
#include <QWebPage>
#include <QTextDocument>
#include <QTextCursor>
#include <QMessageBox>

#include <QDebug>

#include "MaraLibDll.h"

class __MARALIB__CLASS_DECL MChatBrowser : public QWebView
{
	Q_OBJECT;
	public:
		MChatBrowser(QWidget *pParent = 0);

		virtual ~MChatBrowser()
		{
		}

		void clear();
		void appendHtml(const QString &pHtml);
		void setStyleSheet(const QString &pStyleSheet);

		bool toggleFreeze();
		void freeze(bool pFreeze);

		void scrollToBottom();

	protected slots:
		void handleLinkClicked(const QUrl &pUrl);
		void handleLoadFinished(bool pOk);

	protected:
		QTextDocument *qtdChatLog;
		QTextCursor *qtcChatCursor;
		QString _styleSheet;
		//virtual void setSource(const QUrl &name);
		//virtual void mousePressEvent(QMouseEvent *e);
		//virtual void mouseDoubleClickEvent(QMouseEvent *e);
		//virtual void timerEvent(QTimerEvent *e);
		//virtual void resizeEvent(QResizeEvent *pEvent);
		//virtual QVariant loadResource(int type, const QUrl &name);

		void initContent();

		//QList<qint32> lineEndings;
		//QBasicTimer tripleClickTimer;
		//QPoint tripleClick;
		bool _freeze;
};

class __MARALIB__CLASS_DECL MWebPage : public QWebPage
{
	public:
		MWebPage(QObject *pParent = 0);
		virtual ~MWebPage();

	protected:
		virtual void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID);
		virtual void javaScriptAlert(QWebFrame *originatingFrame, const QString& msg);
};

#endif
