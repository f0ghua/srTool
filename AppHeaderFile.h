#ifndef APPHEADERFILE_H
#define APPHEADERFILE_H

#include <QByteArray>

#define PROC_FLAG_NONE              0
#define PROC_FLAG_MODULEID          1
#define PROC_FLAG_BCID              2
#define PROC_FLAG_ECUNAME           3
#define PROC_FLAG_ECUID             4
#define PROC_FLAG_APPNBID           5
#define PROC_FLAG_ASLINFO           6
#define PROC_FLAG_MSGDIGEST         7
#define PROC_FLAG_SINFO             8
#define PROC_FLAG_SIGNATURE         9

#define SIZE_MODULEID               2
#define SIZE_BCID                   2
#define SIZE_ECUNAME                8
#define SIZE_ECUID                  16
#define SIZE_APPNBID                2
#define SIZE_ASLINFO                2
#define SIZE_MSGDIGEST              32
#define SIZE_SINFO                  538
#define SIZE_SIGNATURE              256


class AppHeaderFile
{
public:
    AppHeaderFile();
    qint32 load(QString fileName);
    qint32 save(QString fileName);

    QByteArray m_moduleId;      // Module ID
    QByteArray m_bcid;          // Software Compatibility ID
    QByteArray m_ecuName;       // ECU Name [8]
    QByteArray m_ecuId;         // ECU Identity [16]
    QByteArray m_appNbid;       // Application software-Not Before Identifier
    QByteArray m_aslInfo;       // App SW Location Info [2+8*n]
    QByteArray m_msgDigest;     // Message Digest
    QByteArray m_signerInfo;    // Signer Info
    QByteArray m_signature;     // Signature

/*
    quint8 m_moduleId[2]; // Module ID
    quint8 m_bcid[2]; // Software Compatibility ID
    quint8 m_ecuName[8];  // ECU Identity
    quint8 m_appNbid[2];  // Application software-Not Before Identifier
    QByteArray m_aslInfo; // App SW Location Info
    quint8 m_msgDigest[32]; // Message Digest
    quint8 m_signerInfo[538]; // Signer Info
    quint8 m_signature[256]; // Signature
*/
};

#endif // APPHEADERFILE_H