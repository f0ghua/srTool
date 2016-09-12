#include "SrecFile.h"

#include <QFile>
#include <QByteArray>

#include <QDebug>
#include <QTime>

SrecFile::SrecFile()
{

}

SrecFile::SrecFile(QString fileName)
{
    load(fileName);
}

qint32 SrecFile::load(QString fileName)
{
    QString line;
    int lineNumber = 1;
    QFile *inFile = new QFile(fileName);

    if (!inFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        delete inFile;
        return -1;
    }

#ifndef F_NO_DEBUG
    QTime t;
    t.start();
#endif

    while (!inFile->atEnd())
    {
        line = QString(inFile->readLine().simplified());
        //if (line.startsWith("S3"))

        parseLine(line, lineNumber);
        lineNumber++;
#ifndef F_NO_DEBUG
        //if (lineNumber > 2) break;
#endif
    }

#ifndef F_NO_DEBUG
    qDebug() << lineNumber << " line readed";
    qDebug("Time elapsed: %d ms", t.elapsed());
#endif

    inFile->close();
    delete inFile;

    return 0;
}

bool SrecFile::parseLine(QString &line, int lineNumber)
{
    SrecordData_t sd;
    SrecordHeader_t &sh = m_headerRecord;

    if (line.at(0) != 'S') {
        return false;
    }

    if (!line.at(1).isDigit()) {
        return false;
    }

    QString hexStr(line.mid(2));
    // ba = count(1) + address + data + checksum(2)
    QByteArray ba = QByteArray::fromHex(hexStr.toLatin1());

    if (ba.count() < 1) return false;

    sd.count = ba.at(0);

    if (ba.count() != (sd.count+1)) {
        qDebug() << "err: ba.count = " << ba.count() << ", sd.count =" << sd.count;
        return false;
    }

    // do checksum from count to data end
    quint8 cksum = 0;
    for (int i = 0; i < ba.count()-1; i++)
    {
        cksum += ba.at(i);
    }
    cksum = ~cksum;

    if (cksum != (quint8)ba.at(ba.count()-1)) {
        qDebug() << "err: cksum = " << cksum << ", ba.val = " << (quint8)ba.at(ba.count()-1);
        return false;
    }

    //qDebug() << ba;

    qint8 srecDigit = line.at(1).toLatin1();
    switch (srecDigit)
    {
        case '0':
        {
            sh.lineNumber = lineNumber;
            // Ref to the spec, the module name should be 20 bytes, but in fact...
            /*
            sh.moduleName = ba.mid(3, 20);
            sh.versionNumber = ba.mid(23, 2).toUShort(0, 16);
            sh.revisionNumber = ba.mid(25, 2).toUShort(0, 16);
            sh.comment = ba.mid(27, 36);
            */
            break;
        }
        case '1':
        case '2':
        case '3':
        {
            sd.lineNumber = lineNumber;
            sd.addressLen = srecDigit - '1' + 2;
            sd.recType = srecDigit - '0';
            sd.checksum = cksum;
            sd.address = 0;
            for (quint32 i = 1; i < sd.addressLen+1; i++)
            {
                sd.address <<= 8;
                sd.address += ba.at(i)&0xFF;
            }
            sd.count = ba.count();
            sd.binData = ba.mid(1+sd.addressLen, ba.count()-2-sd.addressLen+1);

            m_dataRecords.insert(lineNumber, sd);
            break;
        }
        default:
            break;
    }

    sd.hexData = line;

    return true;
}
