#include "HeaderFile.h"
#include "Utility.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QTime>

//#define F_NO_DEBUG

#define CH_COMMENT_INDICATOR "#"
#define CH_SIGNAME_INDICATOR "@"	// signed header
#define CH_INFNAME_INDICATOR "$"	// plain data info

static const char g_dataTypeSig[] = {0x03, 0x01};
static const char g_dataTypeInf[] = {0x01, 0x01};

const SecHelper_t g_appSectionMapping[HDRFILE_APP_SECTIONS] = {
	// App Signed Header (data type 03 01)
	{"@Module ID@", 2, NULL},
	{"@BCID@", 2, NULL},
	{"@ECU Name@", 8, NULL},
	{"@ECU ID@", 16, NULL},
	{"@App-NBID@", 2, NULL},
	{"@App SW Location Info@", 0, HeaderFile::appSWLInfoValidator},
	{"@Message Digest@", 32, NULL},
	// Signer Info 538
	{"@Subject Name@", 16, NULL},
	{"@Certificate ID@", 8, NULL},
	{"@Key-NBID@", 2, NULL},
	{"@Signer Public Key@", 256, NULL},
	{"@Root Signature@", 256, NULL},
	{"@Signature@", 256, NULL},

	// App SW Info (data type 01 01)
	{"$Integrity Word$", 2, NULL},
	{"$Configuration Options$", 2, NULL},
	{"$Module ID$", 2, NULL},
	{"$BCID$", 2, NULL},
	{"$App-NBID$", 2, NULL},
	{"$DLS$", 2, NULL},
	{"$Hex Part Number$", 4, NULL},
	{"$App SW Location Info$", 0, HeaderFile::appSWLInfoValidator},
	{"$Number of Partitions$", 2, NULL},
	{"$P1.Num of Regions$", 2, NULL},
	{"$P1.Region Info 1$", 8, NULL},
	{"$P1.Num of Cal Modules$", 2, NULL},
	{"$P1.Cal Module Info 1$", 12, NULL},
	{"$P2.Num of Regions$", 2, NULL},
	{"$P2.Region Info 1$", 8, NULL},
	{"$P2.Num of Cal Modules$", 2, NULL},
	{"$P2.Cal Module Info 1$", 12, NULL}
};

const SecHelper_t g_calSectionMapping[HDRFILE_CAL_SECTIONS] = {
	// App Signed Header (data type 03 01)
	{"@Module ID@", 2, NULL},
	{"@CCID@", 2, NULL},
	{"@ECU Name@", 8, NULL},
	{"@ECU ID@", 16, NULL},
	{"@Message Digest@", 32, NULL},
	// Signer Info 538
	{"@Subject Name@", 16, NULL},
	{"@Certificate ID@", 8, NULL},
	{"@Key-NBID@", 2, NULL},
	{"@Signer Public Key@", 256, NULL},
	{"@Root Signature@", 256, NULL},
	{"@Signature@", 256, NULL},

	// App SW Info (data type 01 01)
	{"$Integrity Word$", 2, NULL},
	{"$Configuration Options$", 2, NULL},
	{"$Module ID$", 2, NULL},
	{"$CCID$", 2, NULL},
	{"$DLS$", 2, NULL},
	{"$Hex Part Number$", 4, NULL},
};

HeaderFile::HeaderFile() :
	m_fileName(),
	m_sigSections()
{

}

bool HeaderFile::appSWLInfoValidator(const QString fileName, const HFileSection_t *pSc, QString &msgOutput)
{
	if ((pSc->data.count()-2)%8 != 0)
	{
		msgOutput += fileName + tr(": section ") + pSc->name +
		tr(" data length is error(%1 != 2+8*n)").arg(pSc->data.count());
		msgOutput += "\n";

		return false;
	}

	return true;
}

int HeaderFile::indexOfSection(const QString &sectionName, const QList<HFileSection_t> &sections)
{
	for (int i = 0; i < sections.count(); i++)
	{
		if (sectionName == sections.at(i).name)
			return i;
	}

	return -1;
}

qint32 HeaderFile::load(QString fileName)
{
    QString line;
    int lineNumber = 0;
    QFile *inFile = new QFile(fileName);
    HFileSection_t section;
    int lastSectionType = SECTION_SIGNEDHDR;

    if (!inFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << fileName << " open failed";
        delete inFile;
        return -1;
    }

    QFileInfo fileinfo = QFileInfo(fileName);
    m_fileName = fileinfo.fileName();

    m_sigSections.clear();
    m_infSections.clear();
    section.clear();
    while (!inFile->atEnd())
    {
        line = QString(inFile->readLine().simplified());

    	if ((line.startsWith(CH_SIGNAME_INDICATOR)) ||
    		(line.startsWith(CH_INFNAME_INDICATOR)))
    	{
    		if (lastSectionType == SECTION_SIGNEDHDR) {
    			if (!section.name.isEmpty() &&
    				(indexOfSection(section.name, m_sigSections) == -1)) {
    				m_sigSections.append(section);
    			}
    		}
    		else {
    			if (!section.name.isEmpty() &&
    				(indexOfSection(section.name, m_infSections) == -1)) {
    				m_infSections.append(section);
    			}
    		}
    	}

        if (line.startsWith(CH_COMMENT_INDICATOR))
        {
        	// it's comment, do nothing
        }
        else
        if (line.startsWith(CH_SIGNAME_INDICATOR))
        {
        	section.clear();
        	int chEnd = line.indexOf(CH_SIGNAME_INDICATOR, 1);
        	if (chEnd != -1) {
        		section.name = line.mid(0, chEnd+1);
        		lastSectionType = SECTION_SIGNEDHDR;
        	}
#ifndef F_NO_DEBUG
        	qDebug() << "processing section " << section.name;
#endif
        }
        else
        if (line.startsWith(CH_INFNAME_INDICATOR))
        {
        	section.clear();
        	int chEnd = line.indexOf(CH_INFNAME_INDICATOR, 1);
        	if (chEnd != -1) {
        		section.name = line.mid(0, chEnd+1);
        		lastSectionType = SECTION_INFO;
        	}
#ifndef F_NO_DEBUG
        	qDebug() << "processing section " << section.name;
#endif
        }
        else
        {
        	line.remove(" ");
        	if (!line.isEmpty()) {
        		section.data += QByteArray::fromHex(line.toLatin1());
#ifndef F_NO_DEBUG
        	qDebug() << "append data " << QByteArray::fromHex(line.toLatin1());
#endif
        	}
        }

        lineNumber++;
    }

    // append the last section
    if (!section.name.isEmpty()) {
    	if ((lastSectionType == SECTION_SIGNEDHDR) &&
    		(indexOfSection(section.name, m_sigSections) == -1))
    		m_sigSections.append(section);
    	else if ((indexOfSection(section.name, m_infSections) == -1))
    		m_infSections.append(section);
    }

    inFile->close();
    delete inFile;

	return 0;
}

qint32 HeaderFile::save(QString fileName)
{
    QString tempStr;
    QFile *outFile = new QFile(fileName);

    if (!outFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        delete outFile;
        return -1;
    }

    for (int i = 0; i < m_sigSections.count(); i++)
    {
    	const HFileSection_t *pSc = &m_sigSections.at(i);

        tempStr.clear();
        tempStr += pSc->name;
        for (int j = 0; j < (64 - pSc->name.length()); j++)
        {
            tempStr += "-";
        }
        tempStr += "\n\n";
        outFile->write(tempStr.toLatin1());

        tempStr.clear();
        tempStr = Utility::formatByteArray(&pSc->data);
        outFile->write(tempStr.toLatin1());
        outFile->write("\n\n");
    }

    for (int i = 0; i < m_infSections.count(); i++)
    {
    	const HFileSection_t *pSc = &m_infSections.at(i);

        tempStr.clear();
        tempStr += pSc->name;
        for (int j = 0; j < (64 - pSc->name.length()); j++)
        {
            tempStr += "-";
        }
        tempStr += "\n\n";
        outFile->write(tempStr.toLatin1());

        tempStr.clear();
        tempStr = Utility::formatByteArray(&pSc->data);
        outFile->write(tempStr.toLatin1());
        outFile->write("\n\n");
    }

    outFile->close();
    delete outFile;

	return 0;
}

QByteArray HeaderFile::getBinData(int type, QString &msgOutput)
{
	const SecHelper_t *pSh;
	int arrayLen;
	QByteArray ba;

	if (type == HDRFILE_TYPE_APP)
	{
		pSh = &g_appSectionMapping[0];
		arrayLen = ARRAY_SIZE(g_appSectionMapping);
	}
	else
	{
		pSh = &g_calSectionMapping[0];
		arrayLen = ARRAY_SIZE(g_calSectionMapping);
	}

	ba.clear();
	ba.append(QByteArray::fromRawData(g_dataTypeSig, sizeof(g_dataTypeSig)));

	for (int i = 0; i < arrayLen; i++)
	{
		QByteArray *pBa = getSectionDataByName(pSh[i].name);
		if (pBa == NULL)
		{
			msgOutput += tr("section %1 is missing\n").arg(pSh[i].name);
			return QByteArray();
		}
		if (pSh[i].name == "$Integrity Word$")
		{
			ba.append(QByteArray::fromRawData(g_dataTypeInf, sizeof(g_dataTypeInf)));
		}
		ba.append(*pBa);
	}

	return ba;
}

QByteArray HeaderFile::getBinDataWithOutCheck()
{
	QByteArray ba;

	ba.clear();
	ba.append(QByteArray::fromRawData(g_dataTypeSig, sizeof(g_dataTypeSig)));
	for (int i = 0; i < m_sigSections.count(); i++)
	{
		ba.append(m_sigSections.at(i).data);
	}
	ba.append(QByteArray::fromRawData(g_dataTypeInf, sizeof(g_dataTypeInf)));
	for (int i = 0; i < m_infSections.count(); i++)
	{
		ba.append(m_infSections.at(i).data);
	}

	return ba;
}

qint64 HeaderFile::getHexPartNumber()
{
	qint64 r = 0;

	QByteArray *pBa = getSectionDataByName("$Hex Part Number$");
	if (pBa == NULL)
		return -1;

	for (int i = 0; i < pBa->count(); i++)
		r = (r << 8) + (pBa->at(i)&0xFF);

	return r;
}

QByteArray *HeaderFile::getSectionDataByName(const QString &name)
{
	for (int i = 0; i < m_sigSections.count(); i++)
	{
		if (m_sigSections.at(i).name == name)
		{
			return &m_sigSections[i].data;
		}
	}

	for (int i = 0; i < m_infSections.count(); i++)
	{
		if (m_infSections.at(i).name == name)
		{
			return &m_infSections[i].data;
		}
	}

	return NULL;
}

static const SecHelper_t *getValidator(const SecHelper_t *pSv,
		int len, QString name)
{
	for (int i = 0; i < len; i++)
	{
		if (pSv[i].name == name)
		{
			return &pSv[i];
		}
	}

	return NULL;
}

bool HeaderFile::isSectionsValid(const QList<HFileSection_t> &sections,
	const SecHelper_t *pSvl, int len, QString &msgOutput)
{
	msgOutput.clear();

	for (int i = 0; i < sections.count(); i++)
	{
		const HFileSection_t *pSc = &sections.at(i);
		const SecHelper_t *pSv = getValidator(pSvl, len, pSc->name);
		//qDebug() << pSc->name << ": pSv = " << pSv;
		if (pSv == NULL) continue; // we allow unknown sections to insert

		if (pSv->func)
		{
			pSv->func(m_fileName, pSc, msgOutput);
		}
		else
		if (pSv->len != pSc->data.count())
		{
			msgOutput += m_fileName + tr(": section ") + pSc->name +
				tr(" data length is error(%1 != %2)").arg(pSc->data.count()).arg(pSv->len);
			msgOutput += "\n";
		}
	}

	if (msgOutput.isEmpty())
		return true;

	return false;
}

bool HeaderFile::isHeaderValid(const SecHelper_t *pSvl, int len, QString &msgOutput)
{
	for (int i = 0; i < len;  i++)
	{
		if ((indexOfSection(pSvl[i].name, m_sigSections) == -1) &&
			(indexOfSection(pSvl[i].name, m_infSections) == -1))
		{
			msgOutput += m_fileName + tr(": section ") + pSvl[i].name + tr("is missing");
			msgOutput += "\n";
		}
	}

	if (isSectionsValid(m_sigSections, pSvl, len, msgOutput) &&
		isSectionsValid(m_infSections, pSvl, len, msgOutput))
		return true;

	return false;
}

bool HeaderFile::appHeaderIsValid(QString &msgOutput)
{
	return isHeaderValid(&g_appSectionMapping[0],
		ARRAY_SIZE(g_appSectionMapping),
		msgOutput);
}

bool HeaderFile::calHeaderIsValid(QString &msgOutput)
{
	return isHeaderValid(&g_calSectionMapping[0],
		ARRAY_SIZE(g_calSectionMapping),
		msgOutput);
}
