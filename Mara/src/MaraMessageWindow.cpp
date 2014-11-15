#include "MaraMessageWindow.h"

#ifdef Q_WS_WIN
#include <windows.h>
#endif

MaraMessageWindow::MaraMessageWindow(QWidget *pParent) : QDialog(pParent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint), _messageUser(0)
{
	setupUi(this);
	mcwChat->mceChatEntry->setEnabled(false);
	mcwChat->setStyleSheet();
	setTitle();
	setAttribute(Qt::WA_DeleteOnClose);
	QObject::connect(mcwChat, SIGNAL(textEntered(const QString &)), this, SLOT(chatWidgetTextEntered(const QString &)));
}

MaraMessageWindow::~MaraMessageWindow()
{
	disassociateUser();
}

void MaraMessageWindow::appendHtml(const QString &pHtml)
{
	mcwChat->appendHtml(pHtml);
}

void MaraMessageWindow::associateUser(Mara::MUser *pUser)
{
	_messageUser = pUser;
	mcwChat->mceChatEntry->setEnabled(true);
	setTitle();
}

void MaraMessageWindow::disassociateUser()
{
	_messageUser = 0;
	mcwChat->mceChatEntry->setEnabled(false);
	setTitle();
}

Mara::MUser *MaraMessageWindow::user()
{
	return _messageUser;
}

void MaraMessageWindow::getAttention()
{
#ifdef Q_WS_WIN
	FLASHWINFO flashInfo;
	flashInfo.cbSize = sizeof(flashInfo);
	flashInfo.hwnd = effectiveWinId();
	flashInfo.dwFlags = FLASHW_ALL | FLASHW_TIMER;
	flashInfo.uCount = 10;
	flashInfo.dwTimeout = 0;
	FlashWindowEx(&flashInfo);
#endif
}

void MaraMessageWindow::setTitle()
{
	if(_messageUser)
	{
		setWindowTitle("Mara 2 - Private Message: " + _messageUser->simpleName());
	}
	else
	{
		setWindowTitle("Mara 2 - Private Message");
	}
}

void MaraMessageWindow::setStyleSheet()
{
	mcwChat->setStyleSheet();
}

void MaraMessageWindow::chatWidgetTextEntered(QString pText)
{
	emit textEntered(_messageUser, pText);
}

void MaraMessageWindow::closeEvent(QCloseEvent *pEvent)
{
	QDialog::closeEvent(pEvent);
	emit closed();
}