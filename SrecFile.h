#ifndef SRECFILE_H
#define SRECFILE_H

#include <QMap>

#define SREC_MODULE_LENGTH      20
#define SREC_VERSION_LENGTH     2
#define SREC_REVISION_LENGTH    2
#define SREC_COMMENT_LENGTH     36
#define SREC_DATA_SIZE_MAX      64

//!< Describes the S-Record header (S-Rec type 0).
typedef struct {
    quint32     lineNumber;
    QByteArray  moduleName;
    quint16     versionNumber;
    quint16     revisionNumber;
    QByteArray  comment;
} SrecordHeader_t;

//!< Describes an s-record which contains data (S1, S2 & S3).
typedef struct {
    quint32     lineNumber;
    quint32     address;
    quint32     addressLen;
    QString     hexData;
    QByteArray  binData;
    quint8      count; // address + data + checksum
    quint8      recType;
    quint8      checksum;
} SrecordData_t;

class SrecFile
{
public:
    SrecFile();
    SrecFile(QString fileName);
    qint32 load(QString fileName);
    QByteArray getBinData(quint32 startAddress, int len);

    QString m_fileFullPath;
    QString m_fileName;
    SrecordHeader_t m_headerRecord;
    QMap<quint32, SrecordData_t> m_dataRecords;
    //QList<SrecordData_t> dataRecords;

private:
    bool parseLine(QString &line, int lineNumber);
};

#endif // SRECFILE_H