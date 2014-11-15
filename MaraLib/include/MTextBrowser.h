#ifndef MTextBrowser_H
#define MTextBrowser_H

#include <QApplication>
#include <QBasicTimer>
#include <QDesktopServices>
#include <QMouseEvent>
#include <QTextBrowser>

#include "MaraLibDll.h"

class __MARALIB__CLASS_DECL MTextBrowser : public QTextBrowser
{
	Q_OBJECT;
	public:
		MTextBrowser(QWidget *pParent = 0);

		virtual ~MTextBrowser()
		{
		}

		void clear();
		void appendHtml(const QString &pHtml);

		bool toggleFreeze();
		void freeze(bool pFreeze);

		void scrollToBottom();

	protected:
		virtual void setSource(const QUrl &name);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void mouseDoubleClickEvent(QMouseEvent *e);
		virtual void timerEvent(QTimerEvent *e);
		virtual void resizeEvent(QResizeEvent *pEvent);
		virtual QVariant loadResource(int type, const QUrl &name);

		QList<qint32> lineEndings;
		QBasicTimer tripleClickTimer;
		QPoint tripleClick;
		bool _freeze;
};

#endif
