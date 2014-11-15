#ifndef MARATEXTPROCESSORS_H
#define MARATEXTPROCESSORS_H

#include <QString>
#include <QRegExp>
#include <QList>

#include "MaraLibDll.h"

namespace Mara
{
	class __MARALIB__CLASS_DECL MReplacement
	{
		public:
			virtual ~MReplacement() {}

			virtual QString replace(const QString& pText) = 0;
			virtual QString remove(const QString& pText) = 0;
	};

	class __MARALIB__CLASS_DECL MRegExReplacement : public MReplacement
	{
		public:
			MRegExReplacement(const QString &pRegEx, const QString &pReplace, bool pMinimal);
			MRegExReplacement(const QString &pRegEx, const QString &pReplace);
			MRegExReplacement(const MRegExReplacement &pMR);
			~MRegExReplacement();

			virtual QString replace(const QString& pText);
			virtual QString remove(const QString& pText);

		protected:
			QRegExp _regExp;
			QString _replacement;
	};

	class __MARALIB__CLASS_DECL MHTMLEscapedRegExReplacement : public MRegExReplacement
	{
		public:
			MHTMLEscapedRegExReplacement(const QString &pRegEx, const QString &pReplace, int pEscapeCapture, bool pMinimal);
			MHTMLEscapedRegExReplacement(const QString &pRegEx, const QString &pReplace, int pEscapeCapture);
			MHTMLEscapedRegExReplacement(const MHTMLEscapedRegExReplacement &pMR);
			~MHTMLEscapedRegExReplacement();

			virtual QString replace(const QString& pText);

		protected:
			int _escapeCapture;
	};

	class __MARALIB__CLASS_DECL MJavascriptEscapedRegExReplacement : public MRegExReplacement
	{
		public:
			MJavascriptEscapedRegExReplacement(const QString &pRegEx, const QString &pReplace, bool pMinimal);
			MJavascriptEscapedRegExReplacement(const QString &pRegEx, const QString &pReplace);
			MJavascriptEscapedRegExReplacement(const MJavascriptEscapedRegExReplacement &pMR);
			~MJavascriptEscapedRegExReplacement();

			virtual QString replace(const QString& pText);
	};

	class __MARALIB__CLASS_DECL MFlashReplacement : public MReplacement
	{
		public:
			MFlashReplacement();
			~MFlashReplacement();

			virtual QString replace(const QString& pText);
			virtual QString remove(const QString& pText);

		protected:
			static quint32 s_flashId;
			QRegExp _regExp;
			QRegExp _regExpYoutube;

		private:
			MFlashReplacement(const MFlashReplacement &pMR) {Q_UNUSED(pMR);}
	};

	class __MARALIB__CLASS_DECL MHtmlEscaper
	{
		public:
			static const QString escape(const QString &pText);

		private:
			MHtmlEscaper();
			~MHtmlEscaper();

			QList<MReplacement*> _replacements;
	};

	class __MARALIB__CLASS_DECL MCodeConverter
	{
		public:
			static const QString replace(const QString &pText, bool pRemoveOnly = false);

		private:
			MCodeConverter();
			~MCodeConverter();

			QList<MReplacement*> _replacements;
	};
};

#endif
