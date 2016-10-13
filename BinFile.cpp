#include "BinFile.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>

/*
BinFile::BinFile(QObject *parent) : QObject(parent)
{
	m_pHeaderFile = new HeaderFile();
	m_pSrecFile = new SrecFile();
}
*/

BinFile::BinFile(HeaderFile *pHeaderFile, SrecFile *pSrecFile, QObject *parent) : QObject(parent)
{
    m_pHeaderFile = pHeaderFile;
    m_pSrecFile = pSrecFile;
}

BinFile::~BinFile()
{
	//if (m_pHeaderFile) delete m_pHeaderFile;
	//if (m_pSrecFile) delete m_pSrecFile;
}

qint32 BinFile::load(QString fileName, int type)
{
    const SecHelper_t *arrayMapping;
    int arrayLen;

    //m_fileFullPath = fileName;
    QFileInfo fileinfo = QFileInfo(fileName);
    m_fileName = fileinfo.fileName();

    if (type == HDRFILE_TYPE_APP)
    {
        if (fileinfo.size() < MIN_APPHDR_SIZE) {
            return -1;
        }
        arrayMapping = &g_appSectionMapping[0];
        arrayLen = HDRFILE_APP_SECTIONS;
    }
    else
    {
        if (fileinfo.size() < MIN_CALHDR_SIZE) {
            return -1;
        }
        arrayMapping = &g_calSectionMapping[0];
        arrayLen = HDRFILE_CAL_SECTIONS;
    }

    QFile *inFile = new QFile(fileName);
    if (!inFile->open(QIODevice::ReadOnly))
    {
        delete inFile;
        return -1;
    }

    QList<HFileSection_t> &lsigSections = m_pHeaderFile->m_sigSections;
    QList<HFileSection_t> &linfSections = m_pHeaderFile->m_infSections;
    lsigSections.clear();
    linfSections.clear();

    inFile->read(2); // data type
    for (int i = 0; i < arrayLen; i++)
    {
        const SecHelper_t *pSh = &arrayMapping[i];

        if(pSh->name.startsWith('@'))
        {
            HFileSection_t section;

            section.name = pSh->name;
            if (pSh->len == 0)
            {
                section.data = inFile->read(2);
                int len = section.data.at(1)*8;
                section.data += inFile->read(len);
            }
            else
            {
                section.data = inFile->read(pSh->len);
            }
            lsigSections.append(section);
        }
    }

    inFile->read(2); // data type
    for (int i = 0; i < arrayLen; i++)
    {
        const SecHelper_t *pSh = &arrayMapping[i];

        if(pSh->name.startsWith('$'))
        {
            HFileSection_t section;
            section.name = pSh->name;
            if (pSh->len == 0)
            {
                section.data = inFile->read(2);
                int len = section.data.at(1)*8;
                section.data += inFile->read(len);
            }
            else
            {
                section.data = inFile->read(pSh->len);
            }

            linfSections.append(section);
        }
    }

#ifndef F_NO_DEBUG
    for (int i = 0; i < m_pHeaderFile->m_sigSections.count(); i++)
    {
        qDebug() << m_pHeaderFile->m_sigSections.at(i).name << " : " << m_pHeaderFile->m_sigSections.at(i).data.toHex();
    }

    for (int i = 0; i < m_pHeaderFile->m_infSections.count(); i++)
    {
        qDebug() << m_pHeaderFile->m_infSections.at(i).name << " : " << m_pHeaderFile->m_infSections.at(i).data.toHex();
    }
#endif

    inFile->close();
    delete inFile;

    return 0;
}

qint32 BinFile::loadAppFileEx(QString fileName, HeaderFile *pHeaderFile, SrecFile *pSrecFile)
{
    QFile *inFile = new QFile(fileName);

#ifndef F_NO_DEBUG
    qDebug() << "call loadAppFile";
#endif

    if (!inFile->open(QIODevice::ReadOnly))
    {
        delete inFile;
        return -1;
    }

    if ((pHeaderFile == NULL) || (pSrecFile == NULL))
    {
    	delete inFile;
    	return -1;
    }

    QList<HFileSection_t> &lsigSections = pHeaderFile->m_sigSections;
    QList<HFileSection_t> &linfSections = pHeaderFile->m_infSections;

    lsigSections.clear();
    linfSections.clear();

    inFile->read(2); // data type
    for (int i = 0; i < HDRFILE_APP_SECTIONS; i++)
    {
    	const SecHelper_t *pSh = &g_appSectionMapping[i];

    	if(pSh->name.startsWith('@'))
    	{
    		HFileSection_t section;

    		section.name = pSh->name;
    		if (pSh->len == 0)
    		{
    			section.data = inFile->read(2);
    			int len = section.data.at(1)*8;
    			section.data += inFile->read(len);
    		}
    		else
    		{
    			section.data = inFile->read(pSh->len);
    		}

    		lsigSections.append(section);
    	}
    }

    inFile->read(2); // data type
    for (int i = 0; i < HDRFILE_APP_SECTIONS; i++)
    {
    	const SecHelper_t *pSh = &g_appSectionMapping[i];

    	if(pSh->name.startsWith('$'))
    	{
    		HFileSection_t section;
    		section.name = pSh->name;
    		if (pSh->len == 0)
    		{
    			section.data = inFile->read(2);
    			int len = section.data.at(1)*8;
    			section.data += inFile->read(len);
    		}
    		else
    		{
    			section.data = inFile->read(pSh->len);
    		}

    		linfSections.append(section);
    	}
    }

#ifndef F_NO_DEBUG
	for (int i = 0; i < pHeaderFile->m_sigSections.count(); i++)
	{
		qDebug() << pHeaderFile->m_sigSections.at(i).name << " : " << pHeaderFile->m_sigSections.at(i).data.toHex();
	}

	for (int i = 0; i < pHeaderFile->m_infSections.count(); i++)
	{
		qDebug() << pHeaderFile->m_infSections.at(i).name << " : " << pHeaderFile->m_infSections.at(i).data.toHex();
	}
#endif

    inFile->close();
    delete inFile;

    return 0;
}

qint32 BinFile::loadCalFileEx(QString fileName, HeaderFile *pHeaderFile, SrecFile *pSrecFile)
{
    QFile *inFile = new QFile(fileName);

#ifndef F_NO_DEBUG
    qDebug() << "call loadAppFile";
#endif

    if (!inFile->open(QIODevice::ReadOnly))
    {
        delete inFile;
        return -1;
    }

    if ((pHeaderFile == NULL) || (pSrecFile == NULL))
    {
    	delete inFile;
    	return -1;
    }

    QList<HFileSection_t> &lsigSections = pHeaderFile->m_sigSections;
    QList<HFileSection_t> &linfSections = pHeaderFile->m_infSections;

    lsigSections.clear();
    linfSections.clear();

    inFile->read(2); // data type
    for (int i = 0; i < HDRFILE_CAL_SECTIONS; i++)
    {
    	const SecHelper_t *pSh = &g_calSectionMapping[i];

    	if(pSh->name.startsWith('@'))
    	{
    		HFileSection_t section;

    		section.name = pSh->name;
    		if (pSh->len == 0)
    		{
    			section.data = inFile->read(2);
    			int len = section.data.at(1)*8;
    			section.data += inFile->read(len);
    		}
    		else
    		{
    			section.data = inFile->read(pSh->len);
    		}

    		lsigSections.append(section);
    	}
    }

    inFile->read(2); // data type
    for (int i = 0; i < HDRFILE_CAL_SECTIONS; i++)
    {
    	const SecHelper_t *pSh = &g_calSectionMapping[i];

    	if(pSh->name.startsWith('$'))
    	{
    		HFileSection_t section;
    		section.name = pSh->name;
    		if (pSh->len == 0)
    		{
    			section.data = inFile->read(2);
    			int len = section.data.at(1)*8;
    			section.data += inFile->read(len);
    		}
    		else
    		{
    			section.data = inFile->read(pSh->len);
    		}

    		linfSections.append(section);
    	}
    }

#ifndef F_NO_DEBUG
	for (int i = 0; i < pHeaderFile->m_sigSections.count(); i++)
	{
		qDebug() << pHeaderFile->m_sigSections.at(i).name << " : " << pHeaderFile->m_sigSections.at(i).data.toHex();
	}

	for (int i = 0; i < pHeaderFile->m_infSections.count(); i++)
	{
		qDebug() << pHeaderFile->m_infSections.at(i).name << " : " << pHeaderFile->m_infSections.at(i).data.toHex();
	}
#endif

    inFile->close();
    delete inFile;

    return 0;
}
