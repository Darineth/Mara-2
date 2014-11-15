#include "MChatBrowser.h"

#include <QScrollBar>
#include <QTextFrame>
#include <QAction>

MChatBrowser::MChatBrowser(QWidget *pParent /*= 0*/) : QWebView(pParent), _freeze(false)
{
	qtdChatLog = new QTextDocument(this);
	qtcChatCursor = new QTextCursor(qtdChatLog);
	connect(this, SIGNAL(linkClicked(const QUrl &)), this, SLOT(handleLinkClicked(const QUrl &)));
	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(handleLoadFinished(bool)));
	//lineEndings.append(0);
	setAcceptDrops(false);
	setPage(new MWebPage(this));
	initContent();
	page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	page()->action(QWebPage::Reload)->setVisible(false);
}

void MChatBrowser::initContent()
{
	setHtml("<HTML><HEAD>"
			"<STYLE type=\"text/css\">"
			"</STYLE>"
			"<SCRIPT>"
			"function appendHtml(pHtml) {var newSpan = document.createElement('span'); newSpan.innerHTML = pHtml; document.body.appendChild(newSpan);}"

			"function appendStyle(pStyle) {"
			"var newStyle = document.createElement('style'); newStyle.type = 'text/css';"
			"var styleHtml = document.createTextNode(pStyle);"
			//"newStyle.appendChild(styleHtml); document.getElementsByTagName('HEAD')[0].appendChild(newStyle);}"
			"newStyle.appendChild(styleHtml); document.body.appendChild(newStyle);}"

			"function swapVisibility(elmID)"
			"{"
			"	if (document.getElementById(elmID).style.display != \"none\")"
			"	{"
			"		document.getElementById(elmID).style.display=\"none\";"
			"	} else {"
			"		document.getElementById(elmID).style.display=\"block\";"
			"	}"
			"}"
			"window.frozen = false;"
			"function scrollToBottom() { if(!window.frozen) { window.scrollTo(0, document.body.scrollHeight); } }"
			"</SCRIPT>"
			"</HEAD><BODY></BODY></HTML>");
	page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
}

void MChatBrowser::clear()
{
	initContent();
}

void MChatBrowser::setStyleSheet(const QString &pStyleSheet)
{
	QString jsHtml = pStyleSheet;
	jsHtml.replace("'", "\\'");
	jsHtml.replace("\n", "");
	jsHtml.replace("\r", "");
	QString js = QString("appendStyle('%1')").arg(jsHtml);
	page()->mainFrame()->evaluateJavaScript(js);
}

void MChatBrowser::appendHtml(const QString &pHtml)
{
	QString jsHtml = pHtml;
	jsHtml.replace("\'", "\\'");
	QString js = QString("appendHtml('%1')").arg(jsHtml);
	page()->mainFrame()->evaluateJavaScript(js);
	scrollToBottom();
}

void MChatBrowser::scrollToBottom()
{
	if(!_freeze)
	{
		//page()->currentFrame()->setScrollBarValue(Qt::Vertical, page()->currentFrame()->scrollBarMaximum(Qt::Vertical));
		//verticalScrollBar()->setValue(verticalScrollBar()->maximum());
		//triggerPageAction(QWebPage::MoveToEndOfDocument);
		//QApplication::postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_End, Qt::ControlModifier));
		
		page()->mainFrame()->evaluateJavaScript("scrollToBottom()");
	}
}

bool MChatBrowser::toggleFreeze()
{
	_freeze = !_freeze;

	if(_freeze)
	{
		page()->mainFrame()->evaluateJavaScript("window.frozen = true;");
	}
	else
	{
		page()->mainFrame()->evaluateJavaScript("window.frozen = false;");
	}

	if(!_freeze) scrollToBottom();
	return _freeze;
}

void MChatBrowser::freeze(bool pFreeze)
{
	_freeze = pFreeze;
}

void MChatBrowser::handleLinkClicked(const QUrl &pUrl)
{
	QDesktopServices::openUrl(pUrl);
}

void MChatBrowser::handleLoadFinished(bool pOk)
{
	Q_UNUSED(pOk);
	scrollToBottom();
}

MWebPage::MWebPage(QObject *pParent) : QWebPage(pParent)
{

}

MWebPage::~MWebPage() {}

void MWebPage::javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID)
{
	//MLogger::log(Mara::LogLevel::Debug, QString("JavaScript Error: [%1:%2] %3").arg(sourceID).arg(lineNumber).arg(message));
	//qDebug(QString("JavaScript Error: [%1:%2] %3").arg(sourceID).arg(lineNumber).arg(message));
	//QMessageBox::information(view(), QString("JavaScript Error"), QString("JavaScript Error: [%1:%2] %3").arg(sourceID).arg(lineNumber).arg(message));
	qDebug() << message << lineNumber << sourceID;
}

void MWebPage::javaScriptAlert(QWebFrame *originatingFrame, const QString& msg)
{
	Q_UNUSED(originatingFrame);
	qDebug() << msg;
}