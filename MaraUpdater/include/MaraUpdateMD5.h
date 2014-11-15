#ifndef MARAUPDATEMD5
#define MARAUPDATEMD5

#include <QThread>

class MaraUpdateMD5 : public QThread
{
	Q_OBJECT;

	public:
		MaraUpdateMD5(const QString &pFile);
		~MaraUpdateMD5();

		const QByteArray &md5() const;

		void run();

	signals:
		void finished();
		void error(QString pError);

	private:
		QString _file;
		QByteArray _md5;
};

#endif
