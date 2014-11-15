#ifndef MTextStyle_H
#define MTextStyle_H

#include <QString>
#include <QFont>
#include <QColor>

#include "MaraLibDll.h"

namespace Mara
{
	class __MARALIB__CLASS_DECL MTextStyle
	{
		public:
			MTextStyle();
			MTextStyle(const QString &pName, const QFont &pFont, const QColor &pColor);
			MTextStyle(const MTextStyle &pStyle);
			~MTextStyle() {}

			const QString& toCss() const;
			const QString& toInlineCss() const;
			const QString& name() const;
			const QFont& font() const;
			const QColor& color() const;

			void setName(const QString &pName);
			void setFont(const QFont &pFont);
			void setColor(const QColor &pColor);

		private:
			QString _name;
			QFont _font;
			QColor _color;

			mutable bool _cssDirty;
			mutable bool _cssInlineDirty;
			mutable QString _css;
			mutable QString _cssInline;
	};
};

#endif
