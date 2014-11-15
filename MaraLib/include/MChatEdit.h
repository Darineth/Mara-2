#ifndef MChatEdit_H
#define MChatEdit_H

#include <QApplication>
#include <QBasicTimer>
#include <QDesktopServices>
#include <QEvent>
#include <QLinkedList>
#include <QMouseEvent>
#include <QSyntaxHighlighter>
#include <QTextBrowser>
#include <QVector>

#include "MaraLibDll.h"

class __MARALIB__CLASS_DECL MChatEdit : public QTextBrowser
{
	Q_OBJECT;

	class MChatEditHighlighter : public QSyntaxHighlighter
	{
		public:
			MChatEditHighlighter(MChatEdit *pParent);
			~MChatEditHighlighter();

		private:
			virtual void highlightBlock(const QString &pText);

			QVector<QTextCharFormat> _formats;
	};

	public:
		MChatEdit(QWidget *pParent = 0);

		virtual ~MChatEdit();

		bool autoResize() const;
		void setAutoResize(bool pAutoResize);

		int maxLength() const;
		void setMaxLength(int maxLength);

		void setFont(const QFont &pFont);

	public slots:
		virtual void updateSize();

	signals:
		void textEntered(QString pText);

	protected slots:
		void checkChangedText();

	protected:
		virtual bool eventFilter(QObject *pObject, QEvent *pEvent);
		virtual void setSource(const QUrl &name);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void mouseDoubleClickEvent(QMouseEvent *e);
		virtual void timerEvent(QTimerEvent *e);
		virtual QVariant loadResource(int type, const QUrl &name);
		bool canInsertFromMimeData(const QMimeData *pSource) const;
		void insertFromMimeData(const QMimeData *pSource);
		void appendHistory(const QString &pText);
		void checkTextLength();

		QBasicTimer _tripleClickTimer;
		QPoint _tripleClick;

		static const int _maxHistoryRecords;
		bool _autoResize;

		int _minimumSize;
		int _maxLength;

		QLinkedList<QString> _history;
		QLinkedList<QString>::const_iterator _historyIterator;
		QString _lastEntered;

		//MChatEditHighlighter _highlighter;
};

#endif
