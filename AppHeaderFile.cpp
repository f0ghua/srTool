#include "AppHeaderFile.h"
#include "Utility.h"

#include <QFile>

#include <QDebug>
#include <QTime>

#define F_NO_DEBUG

struct sectionMapping_t {
    QString matchStr;
    int flag;
};

struct sectionMapping_t sectionMapping[] = {
    {"@Module ID", PROC_FLAG_MODULEID},
    {"@Software Compatibility ID", PROC_FLAG_BCID},
    {"@ECU Name", PROC_FLAG_ECUNAME},
    {"@ECU Identity", PROC_FLAG_ECUID},
    {"@Application Software-Not Before Identifier", PROC_FLAG_APPNBID},
    {"@App SW Location Info", PROC_FLAG_ASLINFO},
    {"@Message Digest", PROC_FLAG_MSGDIGEST},
    {"@Signer Info", PROC_FLAG_SINFO},
    {"@Signature", PROC_FLAG_SIGNATURE},
    {"@End", PROC_FLAG_NONE}
};

AppHeaderFile::AppHeaderFile()
{
    m_moduleId.fill(0, SIZE_MODULEID);
    m_bcid.fill(0, SIZE_BCID);
    m_ecuName.fill(0, SIZE_ECUNAME);
    m_ecuId.fill(0, SIZE_ECUID);
    m_appNbid.fill(0, SIZE_APPNBID);
    m_aslInfo.fill(0, SIZE_ASLINFO);
    m_msgDigest.fill(0, SIZE_MSGDIGEST);
    m_signerInfo.fill(0, SIZE_SINFO);
    m_signature.fill(0, SIZE_SIGNATURE);
}

qint32 AppHeaderFile::load(QString fileName)
{
    QString line;
    int lineNumber = 0;
    QFile *inFile = new QFile(fileName);
    int procFlag = PROC_FLAG_NONE;
    QByteArray procBa;

    if (!inFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << fileName << " open failed";
        delete inFile;
        return -1;
    }

#ifndef F_NO_DEBUG
    QTime t;
    t.start();
#endif

    while (!inFile->atEnd())
    {
        bool isFound = false;
        quint32 i = 0;

        line = QString(inFile->readLine().simplified());
        for (i = 0;
            i < sizeof(sectionMapping)/sizeof(sectionMapping[0]);
            i++)
        {
            if (line.startsWith(sectionMapping[i].matchStr))
            {
                isFound = true;
#ifndef F_NO_DEBUG
                qDebug() << "found " << sectionMapping[i].matchStr;
#endif
                break;
            }
        }

        line.replace(" ", "");
#ifndef F_NO_DEBUG
        qDebug() << "line: <" << line << ">";
#endif
        if (inFile->atEnd())
        {
            procBa += QByteArray::fromHex(line.toLatin1());
        }

        // found section, store the prev one and then process for next line
        if ((isFound)||inFile->atEnd())
        {
            switch(procFlag)
            {
                case PROC_FLAG_MODULEID:
                {
                    if (procBa.count() == SIZE_MODULEID)
                        m_moduleId = procBa;
                    break;
                }
                case PROC_FLAG_BCID:
                {
                    if (procBa.count() == SIZE_BCID)
                        m_bcid = procBa;
                    break;
                }
                case PROC_FLAG_ECUNAME:
                {
                    if (procBa.count() == SIZE_ECUNAME)
                        m_ecuName = procBa;
                    break;
                }
                case PROC_FLAG_ECUID:
                {
                    if (procBa.count() == SIZE_ECUID)
                        m_ecuId = procBa;
                    break;
                }
                case PROC_FLAG_APPNBID:
                {
                    if (procBa.count() == SIZE_APPNBID)
                        m_appNbid = procBa;
                    break;
                }
                case PROC_FLAG_ASLINFO:
                {
                    if ((procBa.count()-2)%8 == 0)
                        m_aslInfo = procBa;
                    break;
                }
                case PROC_FLAG_MSGDIGEST:
                {
                    if (procBa.count() == SIZE_MSGDIGEST)
                        m_msgDigest = procBa;
                    break;
                }
                case PROC_FLAG_SINFO:
                {
                    if (procBa.count() == SIZE_SINFO)
                        m_signerInfo = procBa;
                    break;
                }
                case PROC_FLAG_SIGNATURE:
                {
                    if (procBa.count() == SIZE_SIGNATURE)
                        m_signature = procBa;
                    break;
                }
                default:
                    break;
            }

            procFlag = sectionMapping[i].flag;
            procBa.clear();
            lineNumber++;
            continue;
        }

        procBa += QByteArray::fromHex(line.toLatin1());
#ifndef F_NO_DEBUG
        qDebug() << "proc line: " << lineNumber << ", procBa count = " << procBa.count();
#endif
        lineNumber++;
    }

#ifndef F_NO_DEBUG
    qDebug() << __FUNCTION__ << ": " << lineNumber << " line readed";
    qDebug("Time elapsed: %d ms", t.elapsed());
#endif

    inFile->close();
    delete inFile;

    return 0;
}

qint32 AppHeaderFile::save(QString fileName)
{
    QString tempStr;
    QFile *outFile = new QFile(fileName);

    if (!outFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        delete outFile;
        return -1;
    }

    for (int i = 0;
        i < sizeof(sectionMapping)/sizeof(sectionMapping[0]);
        i++)
    {
        outFile->write(sectionMapping[i].matchStr.toLatin1());
        tempStr.clear();
        for (int j = 0; j < (64 - sectionMapping[i].matchStr.length()); j++)
        {
            tempStr += "-";
        }
        tempStr += "\n\n";
        outFile->write(tempStr.toLatin1());

        QByteArray ba = QByteArray();
        QByteArray *pba = &ba;
        switch(sectionMapping[i].flag)
        {
            case PROC_FLAG_MODULEID:
                pba = &m_moduleId;
                break;
            case PROC_FLAG_BCID:
                pba = &m_bcid;
                break;
            case PROC_FLAG_ECUNAME:
                pba = &m_ecuName;
                break;
            case PROC_FLAG_ECUID:
                pba = &m_ecuId;
                break;
            case PROC_FLAG_APPNBID:
                pba = &m_appNbid;
                break;
            case PROC_FLAG_ASLINFO:
                pba = &m_aslInfo;
                break;
            case PROC_FLAG_MSGDIGEST:
                pba = &m_msgDigest;
                break;
            case PROC_FLAG_SINFO:
                pba = &m_signerInfo;
                break;
            case PROC_FLAG_SIGNATURE:
                pba = &m_signature;
                break;

        }

        if (sectionMapping[i].flag != PROC_FLAG_NONE)
        {
            tempStr.clear();
            tempStr = Utility::formatByteArray(pba);
            outFile->write(tempStr.toLatin1());
            outFile->write("\n\n");
        }
    }

    outFile->close();
    delete outFile;

    return 0;
}
