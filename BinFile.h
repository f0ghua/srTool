#ifndef BINFILE_H
#define BINFILE_H

#include <QObject>

#include "HeaderFile.h"
#include "SrecFile.h"

class BinFile : public QObject
{
    Q_OBJECT
public:
    //explicit BinFile(QObject *parent = 0);
    explicit BinFile(HeaderFile *pHeaderFile, SrecFile *pSrecFile, QObject *parent = 0);
    ~BinFile();
    qint32 load(QString fileName, int type);

    static qint32 loadAppFileEx(QString fileName, HeaderFile *pHeaderFile, SrecFile *pSrecFile);
    static qint32 loadCalFileEx(QString fileName, HeaderFile *pHeaderFile, SrecFile *pSrecFile);

    QString m_fileName;
	HeaderFile *m_pHeaderFile;
	SrecFile *m_pSrecFile;

signals:

public slots:


};

#endif // BINFILE_H