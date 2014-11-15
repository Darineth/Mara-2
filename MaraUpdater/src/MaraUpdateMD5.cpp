#include "MaraUpdateMD5.h"

#include <QCryptographicHash>
#include <QFile>

//#include "MaraTrace.h"

MaraUpdateMD5::MaraUpdateMD5(const QString &pFile) : QThread(), _file(pFile), _md5("") {}
MaraUpdateMD5::~MaraUpdateMD5() { wait(); }

const QByteArray &MaraUpdateMD5::md5() const
{
	return _md5;
}

void MaraUpdateMD5::run()
{
	if(QFile::exists(_file))
	{
		QFile md5File(_file);
		if(md5File.open(QFile::ReadOnly))
		{
			QCryptographicHash md5(QCryptographicHash::Md5);

			while(!md5File.atEnd())
				md5.addData(md5File.read(1024));

			_md5 = md5.result().toHex();
		}
	}
	emit finished();
}
