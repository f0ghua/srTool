#ifndef HEADERFILE_H
#define HEADERFILE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>

#define SIZE_MODULEID               2
#define SIZE_BCID                   2
#define SIZE_ECUNAME                8
#define SIZE_ECUID                  16
#define SIZE_APPNBID                2
#define SIZE_ASLINFO                2
#define SIZE_MSGDIGEST              32
#define SIZE_SINFO                  538
#define SIZE_SIGNATURE              256

#define MIN_APPHDR_SIZE				(866 + 0x200)
#define MIN_CALHDR_SIZE				(854 + 0x20)
#define MIN_APPINFO_SIZE			76
#define MIN_CALINFO_SIZE			14
#define SECTION_SIGNEDHDR			0
#define SECTION_INFO				1

#define HDRFILE_APP_SECTIONS		30
#define HDRFILE_CAL_SECTIONS		17

#define HDRFILE_TYPE_APP			0
#define HDRFILE_TYPE_CAL			1

#define ARRAY_SIZE(x) sizeof((x))/sizeof((x)[0])

struct BlockInfo_t {
	quint32 addr;
	quint32 size;
};

struct HFileSection_t {
	QString name;
	QByteArray data;

	HFileSection_t(): name(QString()), data(QByteArray()) {}
	void clear() {name.clear();data.clear();}
};

typedef bool (*FunValidator)(const QString fileName, const HFileSection_t *, QString &);
struct SecHelper_t {
	QString name;
	int len;
	QString syncName;
	FunValidator func;
};

extern const SecHelper_t g_appSectionMapping[];
extern const SecHelper_t g_calSectionMapping[];

class HeaderFile : public QObject
{
	Q_OBJECT

public:
    HeaderFile(int fileType = HDRFILE_TYPE_APP);
    //~HeaderFile() {}
    qint32 load(QString fileName);
    qint32 save(QString fileName);
    bool appHeaderIsValid(QString &msgOutput);
    bool calHeaderIsValid(QString &msgOutput);
    QByteArray *getSectionDataByName(const QString &name);
    QByteArray getBinData(int type, QString &msgOutput);
    QByteArray getHdrBinData(int type, QString &msgOutput);
    QByteArray getBinDataWithOutCheck();
    qint64 getHexPartNumber();
	qint32 loadInfoSection(const QByteArray &ba);
	QList<BlockInfo_t> appBlockInfo() {return m_appBlockInfo;}

    static QByteArray getBlockHeader(int type);
	static bool appSWLInfoValidator(const QString fileName, const HFileSection_t *pSc, QString &msgOutput);

	QString m_fileName;
	int m_fileType;
    QList<HFileSection_t> m_sigSections;
    QList<HFileSection_t> m_infSections;
    QList<BlockInfo_t> m_appBlockInfo;

private:
	int indexOfSection(const QString &sectionName, const QList<HFileSection_t> &sections);
	bool isHeaderValid(const SecHelper_t *pSvl, int len, QString &msgOutput);
	bool isSectionsValid(const QList<HFileSection_t> &sections, const SecHelper_t *pSvl, int len, QString &msgOutput);
	void getAppBlockInfo(QList<BlockInfo_t> &bl);
};

#endif // HEADERFILE_H