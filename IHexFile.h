#ifndef IHEXFILE_H
#define IHEXFILE_H

#include <QObject>
#include <QMap>
#include <QByteArray>

#define	INH32M_HEADER	":020000040000FA"

enum {
	IHEXFILE_HEX386	= 0x00,
	IHEXFILE_HEX86  = 0x01
};

class IHexFile : public QObject
{
    Q_OBJECT
public:
    explicit IHexFile(QObject *parent = 0);
	static int saveFileFromByteArray(QByteArray &data, QString &fileName,
		quint32 baseAddr = 0, int type = IHEXFILE_HEX386);
	static QByteArray decode(QString &hexFileName);
	
	bool segmentAddrRecord;
	bool linearAddrRecord;
	QMap<quint32, QByteArray> blocks;

signals:

public slots:
};

#endif // IHEXFILE_H