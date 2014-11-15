#include "MaraSettingsDialog.h"
#include "MaraStaticData.h"
#include "MUser.h"

MaraSettingsDialog::MaraSettingsDialog(QWidget *pParent) : QDialog(pParent),
		_tempSettings(new QSettings::SettingsMap()),
		_colorButtonMapper(new QSignalMapper(this)),
		_fontButtonMapper(new QSignalMapper(this)),
		_templateUpdateMapper(new QSignalMapper(this))
{
	setupUi(this);

	MaraClientSettings::settings().loadIntoMap(_tempSettings);

	connect(qpbSave, SIGNAL(clicked()), this, SLOT(validateAndAccept()));

	connect(qpbGenerateToken, SIGNAL(clicked()), this, SLOT(generateToken()));
	connect(qleUserName, SIGNAL(textChanged(const QString&)), this, SLOT(updateNames(const QString&)));
	connect(qleLogFile, SIGNAL(textChanged(const QString&)), this, SLOT(updateLogFile()));
	connect(qleTimestampFormat, SIGNAL(textChanged(const QString&)), this, SLOT(updateTimestamp()));

	qleDataPort->setValidator(new QIntValidator(0, 65535, this));
	qleHeartbeatTimeout->setValidator(new QIntValidator(0, 600000, this));

	qlwPages->setViewMode(QListView::IconMode);
	qlwPages->setFlow(QListView::LeftToRight);
	qlwPages->setIconSize(QSize(60, 60));
	qlwPages->setMovement(QListView::Static);
	qlwPages->setMaximumHeight(85);
	qlwPages->setSpacing(2);
	
	QListWidgetItem *qliConnection = new QListWidgetItem(qlwPages);
	qliConnection->setIcon(QIcon(":/images/config.png"));
	qliConnection->setText(tr("Connection"));
	qliConnection->setTextAlignment(Qt::AlignHCenter);
	qliConnection->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	QListWidgetItem *qliFonts = new QListWidgetItem(qlwPages);
	qliFonts->setIcon(QIcon(":/images/fonts.png"));
	qliFonts->setText(tr("Fonts"));
	qliFonts->setTextAlignment(Qt::AlignHCenter);
	qliFonts->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	QListWidgetItem *qliMacros = new QListWidgetItem(qlwPages);
	qliMacros->setIcon(QIcon(":/images/macros.png"));
	qliMacros->setText(tr("Macros"));
	qliMacros->setTextAlignment(Qt::AlignHCenter);
	qliMacros->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	QListWidgetItem *qliTemplates = new QListWidgetItem(qlwPages);
	qliTemplates->setIcon(QIcon(":/images/templates.png"));
	qliTemplates->setText(tr("Templates"));
	qliTemplates->setTextAlignment(Qt::AlignHCenter);
	qliTemplates->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	connect(qlwPages,
			SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
			this,
			SLOT(changeSettingPage(QListWidgetItem *, QListWidgetItem*)));

	qlwPages->setCurrentItem(qliConnection);

	connect(qpbUserColor, SIGNAL(clicked()), _colorButtonMapper, SLOT(map()));
	connect(qpbDefaultColor, SIGNAL(clicked()), _colorButtonMapper, SLOT(map()));
	connect(qpbMessageColor, SIGNAL(clicked()), _colorButtonMapper, SLOT(map()));
	connect(qpbSystemColor, SIGNAL(clicked()), _colorButtonMapper, SLOT(map()));
	connect(qpbLinkColor, SIGNAL(clicked()), _colorButtonMapper, SLOT(map()));
	connect(qpbBackgroundColor, SIGNAL(clicked()), _colorButtonMapper, SLOT(map()));
	_colorButtonMapper->setMapping(qpbUserColor, MaraClientSettings::Color::User);
	_colorButtonMapper->setMapping(qpbDefaultColor, MaraClientSettings::Color::Default);
	_colorButtonMapper->setMapping(qpbMessageColor, MaraClientSettings::Color::Message);
	_colorButtonMapper->setMapping(qpbSystemColor, MaraClientSettings::Color::System);
	_colorButtonMapper->setMapping(qpbLinkColor, MaraClientSettings::Color::Link);
	_colorButtonMapper->setMapping(qpbBackgroundColor, MaraClientSettings::Color::Background);

	connect(qpbUserFont, SIGNAL(clicked()), _fontButtonMapper, SLOT(map()));
	connect(qpbDefaultFont, SIGNAL(clicked()), _fontButtonMapper, SLOT(map()));
	connect(qpbMessageFont, SIGNAL(clicked()), _fontButtonMapper, SLOT(map()));
	connect(qpbSystemFont, SIGNAL(clicked()), _fontButtonMapper, SLOT(map()));
	_fontButtonMapper->setMapping(qpbUserFont, MaraClientSettings::Font::User);
	_fontButtonMapper->setMapping(qpbDefaultFont, MaraClientSettings::Font::Default);
	_fontButtonMapper->setMapping(qpbMessageFont, MaraClientSettings::Font::Message);
	_fontButtonMapper->setMapping(qpbSystemFont, MaraClientSettings::Font::System);

	connect(qleChatTemplate, SIGNAL(textChanged(const QString&)), _templateUpdateMapper, SLOT(map()));
	connect(qleEmoteTemplate, SIGNAL(textChanged(const QString&)), _templateUpdateMapper, SLOT(map()));
	connect(qleMessageTemplate, SIGNAL(textChanged(const QString&)), _templateUpdateMapper, SLOT(map()));
	connect(qlePlainTemplate, SIGNAL(textChanged(const QString&)), _templateUpdateMapper, SLOT(map()));
	_templateUpdateMapper->setMapping(qleChatTemplate, MaraClientSettings::Template::Chat);
	_templateUpdateMapper->setMapping(qleEmoteTemplate, MaraClientSettings::Template::Emote);
	_templateUpdateMapper->setMapping(qleMessageTemplate, MaraClientSettings::Template::Message);
	_templateUpdateMapper->setMapping(qlePlainTemplate, MaraClientSettings::Template::Plain);

	connect(_colorButtonMapper, SIGNAL(mapped(const QString&)), this, SLOT(selectColor(const QString&)));
	connect(_fontButtonMapper, SIGNAL(mapped(const QString&)), this, SLOT(selectFont(const QString&)));
	connect(_templateUpdateMapper, SIGNAL(mapped(const QString&)), this, SLOT(updateTemplatePreview(const QString&)));

	// Do a one-time load for settings edited in-place on the control.
	qleUserName->setText(getSetting<QString>(MaraClientSettings::User::Name));

	qleUserToken->setText(getSetting<QString>(MaraClientSettings::User::Token));

	qcbAllowUserFonts->setChecked(getSetting<bool>(MaraClientSettings::Chat::AllowUserFonts));
	qcbTimestamps->setChecked(getSetting<bool>(MaraClientSettings::Chat::Timestamps));
	qleTimestampFormat->setText(getSetting<QString>(MaraClientSettings::Chat::TimestampFormat));
	qcbShowJoinLeave->setChecked(getSetting<bool>(MaraClientSettings::Chat::ShowJoinLeave));
	qcbLocalEcho->setChecked(getSetting<bool>(MaraClientSettings::Chat::LocalEcho));
	qcbLogging->setChecked(getSetting<bool>(MaraClientSettings::Chat::LogChat));
	qleLogFile->setText(getSetting<QString>(MaraClientSettings::Chat::LogFile));
	qcbMessageWindows->setChecked(getSetting<bool>(MaraClientSettings::Chat::MessageWindows));

	qcbHeartbeat->setChecked(getSetting<bool>(MaraClientSettings::Connection::EnableHeartbeat));
	qleHeartbeatTimeout->setText(QString().setNum(getSetting<int>(MaraClientSettings::Connection::HeartbeatTimeout)));
	qleDataPort->setText(QString().setNum(getSetting<quint16>(MaraClientSettings::Connection::DataPort)));
	qleServer->setText(getSetting<QString>(MaraClientSettings::Connection::Server));
	qleBackupServer->setText(getSetting<QString>(MaraClientSettings::Connection::BackupServer));

	qcbUserFontInEdit->setChecked(getSetting<bool>(MaraClientSettings::Chat::UserFontForEditBox));

	qleMacroF1->setText(getSetting<QString>(MaraClientSettings::Macro::F1));
	qleMacroF2->setText(getSetting<QString>(MaraClientSettings::Macro::F2));
	qleMacroF3->setText(getSetting<QString>(MaraClientSettings::Macro::F3));
	qleMacroF4->setText(getSetting<QString>(MaraClientSettings::Macro::F4));
	qleMacroF5->setText(getSetting<QString>(MaraClientSettings::Macro::F5));
	qleMacroF6->setText(getSetting<QString>(MaraClientSettings::Macro::F6));
	qleMacroF7->setText(getSetting<QString>(MaraClientSettings::Macro::F7));
	qleMacroF8->setText(getSetting<QString>(MaraClientSettings::Macro::F8));
	qleMacroF9->setText(getSetting<QString>(MaraClientSettings::Macro::F9));
	qleMacroF10->setText(getSetting<QString>(MaraClientSettings::Macro::F10));
	qleMacroF11->setText(getSetting<QString>(MaraClientSettings::Macro::F11));
	qleMacroF12->setText(getSetting<QString>(MaraClientSettings::Macro::F12));

	_defaultStyle.setName("DEFAULT");
	_systemStyle.setName("SYSTEM");
	_userStyle.setName("USER");
	_messageStyle.setName("MESSAGE");

	loadSettings();

	qleChatTemplate->setText(getSetting<QString>(MaraClientSettings::Template::Chat));
	qleEmoteTemplate->setText(getSetting<QString>(MaraClientSettings::Template::Emote));
	qleMessageTemplate->setText(getSetting<QString>(MaraClientSettings::Template::Message));
	qlePlainTemplate->setText(getSetting<QString>(MaraClientSettings::Template::Plain));

	qleImageMaxWidth->setText(getSetting<QString>(MaraClientSettings::Image::MaxWidth));
	qleImageMaxHeight->setText(getSetting<QString>(MaraClientSettings::Image::MaxHeight));
}

MaraSettingsDialog::~MaraSettingsDialog()
{
	delete _templateUpdateMapper;
	delete _colorButtonMapper;
	delete _fontButtonMapper;
	delete _tempSettings;
}

void MaraSettingsDialog::changeSettingPage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
		current = previous;

	qswPages->setCurrentIndex(qlwPages->row(current));
}

void MaraSettingsDialog::generateToken()
{
	qleUserToken->setText(Mara::MUser::generateUserToken());
}

void MaraSettingsDialog::updateNames(const QString &pText)
{
	qleTextName->setText(Mara::MUser::buildSimpleName(pText));

	mtbChatName->clear();
	QPalette palette;
	palette.setColor(QPalette::Base, getSetting<QColor>(MaraClientSettings::Color::Background));
	palette.setColor(QPalette::Text, getSetting<QColor>(MaraClientSettings::Color::User));
	mtbChatName->setPalette(palette);
	mtbChatName->setFont(getSetting<QFont>(MaraClientSettings::Font::User));

	QString chatName = "<" + pText + "> Testing!";
	mtbChatName->setHtml(Mara::MCodeConverter::replace(Mara::MHtmlEscaper::escape(chatName), false));

	int size = mtbChatName->document()->size().height() + 4;
	mtbChatName->setMinimumHeight(size);
	mtbChatName->setMaximumHeight(size);
}

void MaraSettingsDialog::validateAndAccept()
{
	if(validateSettings()) accept();
}

bool MaraSettingsDialog::validateSettings()
{
	if(!qleDataPort->hasAcceptableInput())
	{
		qleDataPort->setFocus();
		QMessageBox::critical(this, tr("Error"), tr("Invalid Data Port"));
		return false;
	}

	if(!qleHeartbeatTimeout->hasAcceptableInput())
	{
		qleHeartbeatTimeout->setFocus();
		QMessageBox::critical(this, tr("Error"), tr("Invalid heartbeat timeout."));
		return false;
	}

	if(qleUserName->text() == "")
	{
		QMessageBox::critical(this, tr("Error"), tr("User name is required."));
		qleUserName->setFocus();
		return false;
	}

	QString validationError = "";

	if(!Mara::MUser::validateUserName(qleUserName->text(), validationError))
	{
		qleUserName->setFocus();
		QMessageBox::critical(this, tr("Error"), validationError);
		return false;
	}

	if(qleServer->text() == "")
	{
		qleServer->setFocus();
		QMessageBox::critical(this, tr("Error"), tr("Server address is required."));
		return false;
	}

	if(qleServer->text().indexOf(':') == -1)
	{
		qleServer->setFocus();
		QMessageBox::critical(this, tr("Error"), tr("Server address must include port number."));
		return false;
	}

	return true;
}

void MaraSettingsDialog::showEvent(QShowEvent *pEvent)
{
	updateNames(qleUserName->text());
	QDialog::showEvent(pEvent);
}
 
void MaraSettingsDialog::saveSettings() 
{
	setSetting(MaraClientSettings::User::Name, qleUserName->text());
	setSetting(MaraClientSettings::User::Token, qleUserToken->text());
	setSetting(MaraClientSettings::Chat::AllowUserFonts, qcbAllowUserFonts->isChecked());
	setSetting(MaraClientSettings::Chat::Timestamps, qcbTimestamps->isChecked());
	setSetting(MaraClientSettings::Chat::TimestampFormat, qleTimestampFormat->text());
	setSetting(MaraClientSettings::Chat::ShowJoinLeave, qcbShowJoinLeave->isChecked());
	setSetting(MaraClientSettings::Chat::LocalEcho, qcbLocalEcho->isChecked());
	setSetting(MaraClientSettings::Chat::LogChat, qcbLogging->isChecked());
	setSetting(MaraClientSettings::Chat::LogFile, qleLogFile->text());
	setSetting(MaraClientSettings::Chat::MessageWindows, qcbMessageWindows->isChecked());
	setSetting(MaraClientSettings::Connection::DataPort, (quint16)(qleDataPort->text().toUInt()));
	setSetting(MaraClientSettings::Connection::EnableHeartbeat, qcbHeartbeat->isChecked());
	setSetting(MaraClientSettings::Connection::HeartbeatTimeout, (int)(qleHeartbeatTimeout->text().toUInt()));
	setSetting(MaraClientSettings::Connection::Server, qleServer->text());

	setSetting(MaraClientSettings::Chat::UserFontForEditBox, qcbUserFontInEdit->isChecked());

	setSetting(MaraClientSettings::Macro::F1, qleMacroF1->text());
	setSetting(MaraClientSettings::Macro::F2, qleMacroF2->text());
	setSetting(MaraClientSettings::Macro::F3, qleMacroF3->text());
	setSetting(MaraClientSettings::Macro::F4, qleMacroF4->text());
	setSetting(MaraClientSettings::Macro::F5, qleMacroF5->text());
	setSetting(MaraClientSettings::Macro::F6, qleMacroF6->text());
	setSetting(MaraClientSettings::Macro::F7, qleMacroF7->text());
	setSetting(MaraClientSettings::Macro::F8, qleMacroF8->text());
	setSetting(MaraClientSettings::Macro::F9, qleMacroF9->text());
	setSetting(MaraClientSettings::Macro::F10, qleMacroF10->text());
	setSetting(MaraClientSettings::Macro::F11, qleMacroF11->text());
	setSetting(MaraClientSettings::Macro::F12, qleMacroF12->text());

	setSetting(MaraClientSettings::Template::Chat, qleChatTemplate->text());
	setSetting(MaraClientSettings::Template::Emote, qleEmoteTemplate->text());
	setSetting(MaraClientSettings::Template::Message, qleMessageTemplate->text());
	setSetting(MaraClientSettings::Template::Plain, qlePlainTemplate->text());

	setSetting(MaraClientSettings::Image::MaxWidth, qleImageMaxWidth->text());
	setSetting(MaraClientSettings::Image::MaxHeight, qleImageMaxHeight->text());

	MaraClientSettings::settings().loadFromMap(_tempSettings);
}

void MaraSettingsDialog::selectColor(const QString &pSetting)
{
	QColor newColor = QColorDialog::getColor(getSetting<QColor>(pSetting));
	if(newColor.isValid())
	{
		setSetting(pSetting, newColor);
		loadSettings();
	}
}

void MaraSettingsDialog::selectFont(const QString &pSetting)
{
	bool blnOk = false;
	QString oldFont = getSetting<QFont>(pSetting).family();
	QFont newFont = QFontDialog::getFont(&blnOk, getSetting<QFont>(pSetting), this);
	if(blnOk)
	{
		setSetting(pSetting, newFont);
		loadSettings();
	}
}

void MaraSettingsDialog::loadSettings()
{
	// Fonts/Colors
	QPalette lineEditPalette;

	QColor test = getSetting<QColor>(MaraClientSettings::Color::Background);
	lineEditPalette.setColor(QPalette::Base, getSetting<QColor>(MaraClientSettings::Color::Background));
	lineEditPalette.setColor(QPalette::Background, getSetting<QColor>(MaraClientSettings::Color::Background));

	lineEditPalette.setColor(QPalette::Text, getSetting<QColor>(MaraClientSettings::Color::User));
	qleUserExample->setPalette(lineEditPalette);
	qleUserExample->setFont(getSetting<QFont>(MaraClientSettings::Font::User));
	qleUserExample->setCursorPosition(0);

	lineEditPalette.setColor(QPalette::Text, getSetting<QColor>(MaraClientSettings::Color::Default));
	qleDefaultExample->setPalette(lineEditPalette);
	qleDefaultExample->setFont(getSetting<QFont>(MaraClientSettings::Font::Default));
	qleDefaultExample->setCursorPosition(0);

	lineEditPalette.setColor(QPalette::Text, getSetting<QColor>(MaraClientSettings::Color::Message));
	qleMessageExample->setPalette(lineEditPalette);
	qleMessageExample->setFont(getSetting<QFont>(MaraClientSettings::Font::Message));
	qleMessageExample->setCursorPosition(0);

	lineEditPalette.setColor(QPalette::Text, getSetting<QColor>(MaraClientSettings::Color::System));
	qleSystemExample->setPalette(lineEditPalette);
	qleSystemExample->setFont(getSetting<QFont>(MaraClientSettings::Font::System));
	qleSystemExample->setCursorPosition(0);

	lineEditPalette.setColor(QPalette::Text, getSetting<QColor>(MaraClientSettings::Color::Link));
	qleLinkExample->setPalette(lineEditPalette);
	QFont linkFont = getSetting<QFont>(MaraClientSettings::Font::Default);
	linkFont.setUnderline(true);
	qleLinkExample->setFont(linkFont);
	qleLinkExample->setCursorPosition(0);

	updateNames(qleUserName->text());

	_defaultStyle.setColor(getSetting<QColor>(MaraClientSettings::Color::Default));
	_systemStyle.setColor(getSetting<QColor>(MaraClientSettings::Color::System));
	_userStyle.setColor(getSetting<QColor>(MaraClientSettings::Color::User));
	_messageStyle.setColor(getSetting<QColor>(MaraClientSettings::Color::Message));
	_defaultStyle.setFont(getSetting<QFont>(MaraClientSettings::Font::Default));
	_systemStyle.setFont(getSetting<QFont>(MaraClientSettings::Font::System));
	_userStyle.setFont(getSetting<QFont>(MaraClientSettings::Font::User));
	_messageStyle.setFont(getSetting<QFont>(MaraClientSettings::Font::Message));

	QString styleSheet = _defaultStyle.toCss() + _systemStyle.toCss() + _userStyle.toCss() + _messageStyle.toCss();

	QPalette templatePalette;
	templatePalette.setColor(QPalette::Base, getSetting<QColor>(MaraClientSettings::Color::Background));
	templatePalette.setColor(QPalette::Text, getSetting<QColor>(MaraClientSettings::Color::Default));

	mtbChatTemplate->setPalette(templatePalette);
	mtbChatTemplate->setFont(getSetting<QFont>(MaraClientSettings::Font::Default));
	mtbChatTemplate->document()->setDefaultStyleSheet(styleSheet);

	mtbEmoteTemplate->setPalette(templatePalette);
	mtbEmoteTemplate->setFont(getSetting<QFont>(MaraClientSettings::Font::Default));
	mtbEmoteTemplate->document()->setDefaultStyleSheet(styleSheet);

	mtbMessageTemplate->setPalette(templatePalette);
	mtbMessageTemplate->setFont(getSetting<QFont>(MaraClientSettings::Font::Default));
	mtbMessageTemplate->document()->setDefaultStyleSheet(styleSheet);

	mtbPlainTemplate->setPalette(templatePalette);
	mtbPlainTemplate->setFont(getSetting<QFont>(MaraClientSettings::Font::Default));
	mtbPlainTemplate->document()->setDefaultStyleSheet(styleSheet);
}

void MaraSettingsDialog::updateLogFile()
{
	qleCurrentLogFile->setText(Mara::MFileLogger::currentFilename(qleLogFile->text(), "Chat"));
}

void MaraSettingsDialog::updateTimestamp()
{
	qleCurrentTimestamp->setText(QDateTime::currentDateTime().toString(qleTimestampFormat->text()));
}

void MaraSettingsDialog::updateTemplatePreview(const QString &pTemplate)
{
	if(pTemplate == MaraClientSettings::Template::Chat)
	{
		mtbChatTemplate->setHtml(MaraStaticData::chatToHtml("Mara User {#ff0000}123{#}", "I am chatting.  This is what my text would look like!\nEven if there are multiple lines.",
															_userStyle, _defaultStyle, _systemStyle, _messageStyle, qleChatTemplate->text()));
	}
	else if(pTemplate == MaraClientSettings::Template::Emote)
	{
		mtbEmoteTemplate->setHtml(MaraStaticData::chatToHtml("Mara User {#ff0000}123{#}", "emotes for the greater good of all Mara-kind!",
															 _userStyle, _defaultStyle, _systemStyle, _messageStyle, qleEmoteTemplate->text()));
	}
	else if(pTemplate == MaraClientSettings::Template::Message)
	{
		mtbMessageTemplate->setHtml(MaraStaticData::chatToHtml("Mara User {#ff0000}123{#}", "I just wanted you to know...I LOVE YOU!",
															   _userStyle, _defaultStyle, _systemStyle, _messageStyle, qleMessageTemplate->text()));
	}
	else if(pTemplate == MaraClientSettings::Template::Plain)
	{
		mtbPlainTemplate->setHtml(MaraStaticData::chatToHtml("Mara User {#ff0000}123{#}", "This is a test of the emergency system message...system.",
															 _userStyle, _defaultStyle, _systemStyle, _messageStyle, qlePlainTemplate->text()));
	}
}
