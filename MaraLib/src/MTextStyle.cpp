#include "MTextStyle.h"

namespace Mara
{
	MTextStyle::MTextStyle() :
			_name(""),
			_font("Arial", 10),
			_color(Qt::white),
			_cssDirty(true),
			_cssInlineDirty(true),
			_css(""),
			_cssInline("")
	{
	}

	MTextStyle::MTextStyle(const QString &pName, const QFont &pFont, const QColor &pColor) :
			_name(pName),
			_font(pFont),
			_color(pColor),
			_cssDirty(true),
			_cssInlineDirty(true),
			_css(""),
			_cssInline("")
	{
	}

	MTextStyle::MTextStyle(const MTextStyle &pStyle) :
			_name(pStyle._name),
			_font(pStyle._font),
			_color(pStyle._color),
			_cssDirty(true),
			_cssInlineDirty(true),
			_css(pStyle._css),
			_cssInline(pStyle._cssInline)
	{
	}

	const QString& MTextStyle::toCss() const
	{
		if(_cssDirty)
		{
			QString weight;
			QString style;

			if(_font.bold())
				weight = "bold";
			else
				weight = "normal";

			if(_font.italic())
				style = "italic";
			else
				style = "normal";

			QString strBaseCSS;
			strBaseCSS = " { white-space:pre-wrap; margin-left:10px; text-indent:-10px; font-family:'%1'; font-size:%2pt; color:rgb(%3, %4, %5); font-weight:%6; font-style:%7; } \n";

			_css = (QString("#" + _name + strBaseCSS)
					.arg(_font.family())
					.arg(_font.pointSize())
					.arg(_color.red())
					.arg(_color.green())
					.arg(_color.blue())
					.arg(weight)
					.arg(style));

			_cssDirty = false;
		}
		return _css;
	}

	const QString& MTextStyle::toInlineCss() const
	{
		if(_cssInlineDirty)
		{
			QString weight;
			QString style;

			if(_font.bold())
				weight = "bold";
			else
				weight = "normal";

			if(_font.italic())
				style = "italic";
			else
				style = "normal";

			QString strBaseCSS;
			strBaseCSS = "white-space:pre-wrap; margin-left:10px; text-indent:-10px; font-family:%1; font-size:%2pt; color:rgb(%3, %4, %5); font-weight:%6; font-style:%7;";

			_cssInline = (strBaseCSS
				.arg(_font.family())
				.arg(_font.pointSize())
				.arg(_color.red())
				.arg(_color.green())
				.arg(_color.blue())
				.arg(weight)
				.arg(style));

			_cssInlineDirty = false;
		}
		return _cssInline;
	}

	const QString& MTextStyle::name() const
	{
		return _name;
	}

	const QFont& MTextStyle::font() const
	{
		return _font;
	}

	const QColor& MTextStyle::color() const
	{
		return _color;
	}

	void MTextStyle::setName(const QString &pName)
	{
		_cssDirty = true;
		_cssInlineDirty = true;
		_name = pName;
	}

	void MTextStyle::setFont(const QFont &pFont)
	{
		_cssDirty = true;
		_cssInlineDirty = true;
		_font = pFont;
	}

	void MTextStyle::setColor(const QColor &pColor)
	{
		_cssDirty = true;
		_cssInlineDirty = true;
		_color = pColor;
	}
}