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

#define SECTION_SIGNEDHDR			0
#define SECTION_INFO				1

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
	FunValidator func;
};

class HeaderFile : public QObject
{
	Q_OBJECT

public:
    HeaderFile();
    //~HeaderFile() {}
    qint32 load(QString fileName);
    qint32 save(QString fileName);
    bool appHeaderIsValid(QString &msgOutput);
    bool calHeaderIsValid(QString &msgOutput);
    QByteArray *getSectionDataByName(const QString &name);
	static bool appSWLInfoValidator(const QString fileName, const HFileSection_t *pSc, QString &msgOutput);

	QString m_fileName;
    QList<HFileSection_t> m_sigSections;
    QList<HFileSection_t> m_infSections;

private:
	int indexOfSection(const QString &sectionName, const QList<HFileSection_t> &sections);
	bool isHeaderValid(const SecHelper_t *pSvl, int len, QString &msgOutput);
	bool isSectionsValid(const QList<HFileSection_t> &sections, const SecHelper_t *pSvl, int len, QString &msgOutput);
};

#endif // HEADERFILE_H