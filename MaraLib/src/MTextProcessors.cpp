#include "MTextProcessors.h"

#include <QStringList>
#include <QUrl>

namespace Mara
{
	///////////////////// Regex Replacer /////////////////////
	MRegExReplacement::MRegExReplacement(const QString &pRegEx,
							   const QString &pReplace,
							   bool pMinimal) :
		_regExp(pRegEx, Qt::CaseInsensitive),
		_replacement(pReplace)
	{
		_regExp.setMinimal(pMinimal);
		_regExp.setPatternSyntax(QRegExp::RegExp2);
	}

	MRegExReplacement::MRegExReplacement(const QString &pRegEx, const QString &pReplace) :
		_regExp(pRegEx, Qt::CaseInsensitive),
		_replacement(pReplace)
	{
		_regExp.setMinimal(true);
		_regExp.setPatternSyntax(QRegExp::RegExp2);
	}

	MRegExReplacement::MRegExReplacement(const MRegExReplacement &pMR) :
		_regExp(pMR._regExp),
		_replacement(pMR._replacement)
	{
		_regExp.setMinimal(pMR._regExp.isMinimal());
	}

	MRegExReplacement::~MRegExReplacement()
	{
	}

	QString MRegExReplacement::replace(const QString& pText)
	{
		QString output = pText;
		output.replace(_regExp, _replacement);
		return output;
	}

	QString MRegExReplacement::remove(const QString& pText)
	{
		QString output = pText;
		output.replace(_regExp, "");
		return output;
	}



	///////////////////// Escaped Regex Replacer /////////////////////
	MHTMLEscapedRegExReplacement::MHTMLEscapedRegExReplacement(const QString &pRegEx,
							   const QString &pReplace,
							   int pEscapeCapture,
							   bool pMinimal) : MRegExReplacement(pRegEx, pReplace, pMinimal), _escapeCapture(pEscapeCapture)
	{
	}

	MHTMLEscapedRegExReplacement::MHTMLEscapedRegExReplacement(const QString &pRegEx, const QString &pReplace, int pEscapeCapture) :
		MRegExReplacement(pRegEx, pReplace), _escapeCapture(pEscapeCapture)
	{
	}

	MHTMLEscapedRegExReplacement::MHTMLEscapedRegExReplacement(const MHTMLEscapedRegExReplacement &pMR) :
		MRegExReplacement(pMR._regExp.pattern(), pMR._replacement, pMR._regExp.isMinimal())
	{
	}

	MHTMLEscapedRegExReplacement::~MHTMLEscapedRegExReplacement()
	{
	}

	QString MHTMLEscapedRegExReplacement::replace(const QString& pText)
	{
		static QByteArray exclude(":/#=&+;%?");

		QString output = pText;

		int pos = 0;

		while((pos = _regExp.indexIn(pText, pos)) != -1)
		{
			//for(int ii = 1; ii < _regExp.capturedTexts().length(); ++ii)
			//{
				pos -= _regExp.cap(_escapeCapture).length();
				//int cappos = _regExp.pos(ii);
				output.remove(_regExp.pos(_escapeCapture), _regExp.cap(_escapeCapture).length());
				QString replaced = QUrl::toPercentEncoding(_regExp.cap(_escapeCapture).toLatin1(), exclude);
				pos += replaced.length();
				output.insert(_regExp.pos(_escapeCapture), replaced);
			//}
			pos += _regExp.matchedLength();
		}
		
		return output.replace(_regExp, _replacement);
	}


	//////////////// Javascript Escaped Regex Replacer //////////////////
	MJavascriptEscapedRegExReplacement::MJavascriptEscapedRegExReplacement(const QString &pRegEx,
							   const QString &pReplace,
							   bool pMinimal) : MRegExReplacement(pRegEx, pReplace, pMinimal)
	{
	}

	MJavascriptEscapedRegExReplacement::MJavascriptEscapedRegExReplacement(const QString &pRegEx, const QString &pReplace) :
		MRegExReplacement(pRegEx, pReplace)
	{
	}

	MJavascriptEscapedRegExReplacement::MJavascriptEscapedRegExReplacement(const MJavascriptEscapedRegExReplacement &pMR) :
		MRegExReplacement(pMR._regExp.pattern(), pMR._replacement, pMR._regExp.isMinimal())
	{
	}

	MJavascriptEscapedRegExReplacement::~MJavascriptEscapedRegExReplacement()
	{
	}

	QString MJavascriptEscapedRegExReplacement::replace(const QString& pText)
	{
		QString output = pText;

		int pos = 0;

		while((pos = _regExp.indexIn(pText, pos)) != -1)
		{
			for(int ii = 1; ii < _regExp.capturedTexts().length(); ++ii)
			{
				pos -= _regExp.cap(ii).length();
				QString replaced = _regExp.cap(ii);
				replaced.replace("\'", "\\\\x27");
				replaced.replace("\"", "\\\\x22");
				pos += replaced.length();
				output = output.replace(_regExp.cap(ii), replaced);
			}
			pos += _regExp.matchedLength();
		}
		
		return output.replace(_regExp, _replacement);
	}

	//////////////// Flash Replacer //////////////////
	quint32 MFlashReplacement::s_flashId = 1;

	MFlashReplacement::MFlashReplacement() : _regExp("[{\\[]flash[}\\]]([^<>\"]+)[{\\[]/flash[}\\]]"), _regExpYoutube("[{\\[](?:yt|youtube)[}\\]]([^<>\"]+)[{\\[]/(?:yt|youtube)[}\\]]")
	{
		_regExp.setMinimal(true);

		//, "<object width=\"100%\" height=\"100%\"><param name=\"movie\" value=\"\\1\"></param><param name=\"allowFullScreen\" value=\"true\"></param><param name=\"allowscriptaccess\" value=\"always\"></param><embed src=\"\\1\" type=\"application/x-shockwave-flash\" allowscriptaccess=\"always\" allowfullscreen=\"true\" width=\"100%\" height=\"100%\"></embed></object>", true)
	}

	MFlashReplacement::~MFlashReplacement()
	{

	}

	QString MFlashReplacement::replace(const QString& pText)
	{
		QString output = pText;
		int pos;
		//output.replace(_regExp, "<object width=\"100%\" height=\"100%\"><param name=\"movie\" value=\"\\1\"></param><param name=\"allowFullScreen\" value=\"true\"></param><param name=\"allowscriptaccess\" value=\"always\"></param><embed src=\"\\1\" type=\"application/x-shockwave-flash\" allowscriptaccess=\"always\" allowfullscreen=\"true\" width=\"100%\" height=\"100%\"></embed></object>");
		if((pos = _regExp.indexIn(output)) > -1)
		{
			output.remove(pos, _regExp.matchedLength());

			//QString flash("<object width=\"100%\" height=\"100%\"><param name=\"movie\" value=\"\\1\"></param><param name=\"allowFullScreen\" value=\"true\"></param><param name=\"allowscriptaccess\" value=\"always\"></param><embed src=\"\\1\" type=\"application/x-shockwave-flash\" allowscriptaccess=\"always\" allowfullscreen=\"true\" width=\"100%\" height=\"100%\"></embed></object>");
			QString flash("<button onclick=\"swapVisibility('flashdiv_%0')\">Toggle Flash: %1</button><div id=\"flashdiv_%0\" style=\"display:none;\"' class=\"flashdiv\"><object width=\"100%\" height=\"100%\"><param name=\"movie\" value=\"%1\"></param><param name=\"allowFullScreen\" value=\"true\"></param><param name=\"allowscriptaccess\" value=\"always\"></param><embed src=\"%1\" type=\"application/x-shockwave-flash\" allowscriptaccess=\"always\" allowfullscreen=\"true\" width=\"100%\" height=\"100%\"></embed></object></div>");
			flash = flash.arg(s_flashId).arg(_regExp.cap(1));

			s_flashId++;

			output.insert(pos, flash);
		}
		else if((pos = _regExpYoutube.indexIn(output)) > -1)
		{
			output.remove(pos, _regExpYoutube.matchedLength());

			//QString flash("<object width=\"100%\" height=\"100%\"><param name=\"movie\" value=\"\\1\"></param><param name=\"allowFullScreen\" value=\"true\"></param><param name=\"allowscriptaccess\" value=\"always\"></param><embed src=\"\\1\" type=\"application/x-shockwave-flash\" allowscriptaccess=\"always\" allowfullscreen=\"true\" width=\"100%\" height=\"100%\"></embed></object>");
			QString flash("<button onclick=\"swapVisibility('flashdiv_%0')\">YouTube Video: http://www.youtube.com/watch?v=%1</button><div id=\"flashdiv_%0\" style=\"display:none;\"' class=\"flashdiv\"><object width=\"100%\" height=\"100%\"><param name=\"movie\" value=\"http://www.youtube.com/v/%1&hl=en_US&fs=1&rel=1\"></param><param name=\"allowFullScreen\" value=\"true\"></param><param name=\"allowscriptaccess\" value=\"always\"></param><embed src=\"http://www.youtube.com/v/%1&hl=en_US&fs=1&rel=1\" type=\"application/x-shockwave-flash\" allowscriptaccess=\"always\" allowfullscreen=\"true\" width=\"100%\" height=\"100%\"></embed></object></div>");
			flash = flash.arg(s_flashId).arg(QString(QUrl::toPercentEncoding(_regExpYoutube.cap(1))));

			s_flashId++;

			output.insert(pos, flash);
		}
		return output;
	}

	QString MFlashReplacement::remove(const QString& pText)
	{
		QString output = pText;
		output.replace(_regExp, "");
		return output;
	}

	/////////////////////////// HTML Escaper /////////////////////////
	MHtmlEscaper::MHtmlEscaper()
	{
		_replacements.append(new MRegExReplacement("\\\\", "\\\\")); // Seriously.
		_replacements.append(new MRegExReplacement("&(?!amp;)", "&amp;"));
		//_replacements.append(MRegExReplacement(" ", "&nbsp;"));
		_replacements.append(new MRegExReplacement("<", "&lt;"));
		_replacements.append(new MRegExReplacement(">", "&gt;"));
		_replacements.append(new MRegExReplacement("\r\n", "<br>"));
		_replacements.append(new MRegExReplacement("\n", "<br>"));
		//_replacements.append(new MRegExReplacement("(^|[>;\\s\\(\\r\\n])((?:http|ftp|https|HTTP|FTP|HTTPS)://[^().\\[\\]{}]+\\.[^()\\[\\]{}\\ <>\r\n]+)", "\\1<a href=\"\\2\">\\2</a>"));
		_replacements.append(new MHTMLEscapedRegExReplacement("^([a-zA-Z]{2,}://\\S+)(?=\\s|\\)\\.|$)", "<a href=\"\\1\">\\1</a>", 1, true));
		_replacements.append(new MHTMLEscapedRegExReplacement("([\\s|\\(])([a-zA-Z]{2,}://\\S+)(?=\\s|\\)\\.|$)", "\\1<a href=\"\\2\">\\2</a>", 2, true));
	}

	MHtmlEscaper::~MHtmlEscaper()
	{
		for(QList<MReplacement*>::const_iterator ii = _replacements.begin(); ii != _replacements.end(); ++ii)
		{
			delete (*ii);
		}
	}

	const QString MHtmlEscaper::escape(const QString &pText)
	{
		static MHtmlEscaper escaper;
		QString output = pText;

		for(QList<MReplacement*>::const_iterator ii = escaper._replacements.begin(); ii != escaper._replacements.end(); ++ii)
		{
			output = (*ii)->replace(output);
		}

		return output;
	}


	////////////////////////
	// MaraCode Converter //
	////////////////////////

	MCodeConverter::MCodeConverter()
	{
		//_replacements.append(MRegExReplacement("[{\\[](/?)m[}\\]]", "<\\1MARQUEE>"));
		_replacements.append(new MRegExReplacement("[{\\[](/?[bBiIuU])[}\\]]", "<\\1>"));
		_replacements.append(new MRegExReplacement("[{\\[](#[a-fA-F0-9]{6})[}\\]]", "<FONT COLOR=\"\\1\">"));
		_replacements.append(new MRegExReplacement("[{\\[]#[}\\]]", "</FONT>"));
		_replacements.append(new MRegExReplacement("[{\\[]![}\\]]", "<SPAN>"));
		_replacements.append(new MRegExReplacement("[{\\[]/![}\\]]", "</SPAN>"));
		_replacements.append(new MHTMLEscapedRegExReplacement("[{\\[]img[}\\]]([^<>\"]+)[{\\[]/img[}\\]]", "<a href=\"\\1\" title=\"\\1\"><img src=\"\\1\" onload=\"scrollToBottom()\" align=\"top\"></a>", 1, true));
		//_replacements.append(new MHTMLEscapedRegExReplacement("[{\\[]flash[}\\]]([^<>\"]+)[{\\[]/flash[}\\]]", "<object width=\"100%\" height=\"100%\"><param name=\"movie\" value=\"\\1\"></param><param name=\"allowFullScreen\" value=\"true\"></param><param name=\"allowscriptaccess\" value=\"always\"></param><embed src=\"\\1\" type=\"application/x-shockwave-flash\" allowscriptaccess=\"always\" allowfullscreen=\"true\" width=\"100%\" height=\"100%\"></embed></object>", true));
		_replacements.append(new MRegExReplacement("[\\{\\[](?:link|l)=([^\\]\\}]+)[\\]\\}](.+)[\\{\\[]/(?:link|l)[\\]\\}]", "<a href=\"\\2\" title=\"\\2\">\\1</a>", true));
		_replacements.append(new MJavascriptEscapedRegExReplacement("[{\\[](?:s|spoiler)[}\\]]?\\s*(.+)", "<span><input type='submit' value='Spoiler' onclick='parentNode.innerHTML =\"\\1\";' /></span>", false));
		_replacements.append(new MFlashReplacement());
	}

	MCodeConverter::~MCodeConverter()
	{
		for(QList<MReplacement*>::const_iterator ii = _replacements.begin(); ii != _replacements.end(); ++ii)
		{
			delete (*ii);
		}
	}

	const QString MCodeConverter::replace(const QString &pText, bool pRemoveOnly)
	{
		static MCodeConverter converter;
		QString output = pText;

		if(pRemoveOnly)
		{
			for(QList<MReplacement*>::const_iterator ii = converter._replacements.begin(); ii != converter._replacements.end(); ++ii)
			{
				output = (*ii)->remove(output);
			}
		}
		else
		{
			for(QList<MReplacement*>::const_iterator ii = converter._replacements.begin(); ii != converter._replacements.end(); ++ii)
			{
				output = (*ii)->replace(output);
			}
		}

		return output;
	}
};
