#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Utility.h"
#include "SrConfig.h"

#include <QDesktopWidget>
#include <QFileDialog>
#include <QDir>
#include <QTextCursor>
#include <QTextBlock>
#include <QRegExp>
#include <QIcon>
#include <QDebug>

//#define F_NO_DEBUG

#ifndef F_NO_DEBUG
#include <QTime>
#endif

#define LISTWIDGET_SETTEXT(l, a) \
    if ((a) && ((l).count() == (a)->count())) \
    { \
        for (int i = 0; i < (l).count(); i++) \
            (l).at(i)->setText(Utility::formatHexNumber((a)->at(i)&0xFF)); \
    }

#define LISTWIDGET_TEXTSET(l, a) \
    ba.clear(); \
    for (int i = 0; i < (l).count(); i++) \
    { \
        ba.append((char)(l).at(i)->text().toUInt(&ok, 16)); \
    } \
    if (a) (*a) = ba;

extern double g_dpiScaleValue;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pAppHeaderFile(NULL),
    m_pCal1HeaderFile(NULL),
    m_pCal2HeaderFile(NULL),
    m_pAppSrecordFile(NULL),
    m_pCal1SrecordFile(NULL),
    m_pCal2SrecordFile(NULL)
{
    ui->setupUi(this);

/*
    const QRect ag = QApplication::desktop()->availableGeometry(this);
    qint32 w = ag.width() * 2 / 3;
    resize(w , w * 3 / 4);
*/
    resize(width()*g_dpiScaleValue, height()*g_dpiScaleValue);

    QGroupBox *gb;
    gb = this->findChild<QGroupBox *>("m_gbAppDLS");
    if (gb != NULL)
    {
        m_listAppDLS = gb->findChildren<QLineEdit *>();
    }
    gb = this->findChild<QGroupBox *>("m_gbAppHPN");
    if (gb != NULL)
    {
        m_listAppHPN = gb->findChildren<QLineEdit *>();
    }
    gb = this->findChild<QGroupBox *>("m_gbAppBCID");
    if (gb != NULL)
    {
        m_listAppBCID = gb->findChildren<QLineEdit *>();
    }
    gb = this->findChild<QGroupBox *>("m_gbAppNBID");
    if (gb != NULL)
    {
        m_listAppNBID = gb->findChildren<QLineEdit *>();
    }

/*
    gb = this->findChild<QGroupBox *>("m_gbAppASLInfo");
    if (gb != NULL)
    {
        m_listAppASLInfo = gb->findChildren<QLineEdit *>();
    }
*/
    QWidget *wd;
    wd = this->findChild<QWidget *>("m_wgASLInfoSub1");
    if (wd != NULL)
    {
        QList<QLineEdit *> l = wd->findChildren<QLineEdit *>();
        m_listAppASLInfo.append(l);

        //m_listAppASLInfoSub1 = wd->findChildren<QLineEdit *>();
    }
    wd = this->findChild<QWidget *>("m_wgASLInfoSub2");
    if (wd != NULL)
    {
        QList<QLineEdit *> l = wd->findChildren<QLineEdit *>();
        m_listAppASLInfo.append(l);
    }
    wd = this->findChild<QWidget *>("m_wgASLInfoSub3");
    if (wd != NULL)
    {
        QList<QLineEdit *> l = wd->findChildren<QLineEdit *>();
        m_listAppASLInfo.append(l);
    }
    wd = this->findChild<QWidget *>("m_wgASLInfoSub4");
    if (wd != NULL)
    {
        QList<QLineEdit *> l = wd->findChildren<QLineEdit *>();
        m_listAppASLInfo.append(l);
    }
#if 0//ndef F_NO_DEBUG
    for (int i = 0; i < m_listAppASLInfo.count(); i++)
    {
        qDebug() << m_listAppASLInfo.at(i)->objectName();
    }
#endif

    gb = this->findChild<QGroupBox *>("m_gbCal1DLS");
    if (gb != NULL)
    {
        m_listCal1DLS = gb->findChildren<QLineEdit *>();
    }
    gb = this->findChild<QGroupBox *>("m_gbCal1HPN");
    if (gb != NULL)
    {
        m_listCal1HPN = gb->findChildren<QLineEdit *>();
    }
    gb = this->findChild<QGroupBox *>("m_gbCal1CCID");
    if (gb != NULL)
    {
        m_listCal1CCID = gb->findChildren<QLineEdit *>();
    }
    gb = this->findChild<QGroupBox *>("m_gbCal2DLS");
    if (gb != NULL)
    {
        m_listCal2DLS = gb->findChildren<QLineEdit *>();
    }
    gb = this->findChild<QGroupBox *>("m_gbCal2HPN");
    if (gb != NULL)
    {
        m_listCal2HPN = gb->findChildren<QLineEdit *>();
    }
    gb = this->findChild<QGroupBox *>("m_gbCal2CCID");
    if (gb != NULL)
    {
        m_listCal2CCID = gb->findChildren<QLineEdit *>();
    }

    QRegExp rx("^[0-9a-fA-F]{2}$");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    QList<QLineEdit *> list = this->findChildren<QLineEdit *>();
    for (int i = 0; i < list.count(); i++)
        list.at(i)->setValidator(pReg);

    ui->m_tblFileList->verticalHeader()->setDefaultSectionSize(16);
    m_model = new QStandardItemModel(ui->m_tblFileList);

    m_pAppHeaderFile = new HeaderFile(HDRFILE_TYPE_APP);
    m_pCal1HeaderFile = new HeaderFile(HDRFILE_TYPE_CAL);
    m_pCal2HeaderFile = new HeaderFile(HDRFILE_TYPE_CAL);
    m_pFullSrecordFile = new SrecFile();
    m_pAppSrecordFile = new SrecFile();
    m_pCal1SrecordFile = new SrecFile();
    m_pCal2SrecordFile = new SrecFile();

    m_pAppBinFile = new BinFile(m_pAppHeaderFile, m_pAppSrecordFile);
    m_pCal1BinFile = new BinFile(m_pCal1HeaderFile, m_pCal1SrecordFile);
    m_pCal2BinFile = new BinFile(m_pCal2HeaderFile, m_pCal2SrecordFile);

#if 0
    // set default values
    m_parameters["ADDR_APPL_SWINFO"]    = "0x00010000";
    m_parameters["SIZE_APPL_SWINFO"]    = "0x200";
    m_parameters["ADDR_APPL_BLOCK"]     = "0x00010200";
    m_parameters["SIZE_APPL_BLOCK"]     = "0xCFFF8";

    m_parameters["ADDR_CAL1_DATAINFO"]  = "0x00008000";
    m_parameters["SIZE_CAL1_DATAINFO"]  = "0x20";
    m_parameters["ADDR_CAL1_BLOCK"]     = "0x00008020";
    m_parameters["SIZE_CAL1_BLOCK"]     = "0x3FD8";

    m_parameters["ADDR_CAL2_DATAINFO"]  = "0x00000000";
    m_parameters["SIZE_CAL2_DATAINFO"]  = "0x20";
    m_parameters["ADDR_CAL2_BLOCK"]     = "0x00000020";
    m_parameters["SIZE_CAL2_BLOCK"]     = "0x7FD8";

    m_parameters["HEX_HEADER_ENABLE"]   = "0x00";
#endif

    // load settings from setting file
    SrConfig::readConfigSettings(m_parameters);

    if (!SrConfig::validConfigSettings(m_parameters)) {
        messageBoxAlert(tr("config value is not correct, please correct it"));
        close();
    }

    on_actionReLoad_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;

    if (m_pAppHeaderFile) delete m_pAppHeaderFile;
    if (m_pCal1HeaderFile) delete m_pCal1HeaderFile;
    if (m_pAppSrecordFile) delete m_pAppSrecordFile;
    if (m_pCal1SrecordFile) delete m_pCal1SrecordFile;
    if (m_pCal2SrecordFile) delete m_pCal2SrecordFile;
    if (m_pAppBinFile) delete m_pAppBinFile;
    if (m_pCal1BinFile) delete m_pCal1BinFile;
    if (m_pCal2BinFile) delete m_pCal2BinFile;
}

void MainWindow::updateAppHeaderOnGui()
{
    LISTWIDGET_SETTEXT(m_listAppDLS,
    	m_pAppHeaderFile->getSectionDataByName("$DLS$"));
    LISTWIDGET_SETTEXT(m_listAppHPN,
    	m_pAppHeaderFile->getSectionDataByName("$Hex Part Number$"))
    LISTWIDGET_SETTEXT(m_listAppBCID,
    	m_pAppHeaderFile->getSectionDataByName("$BCID$"))
    LISTWIDGET_SETTEXT(m_listAppNBID,
    	m_pAppHeaderFile->getSectionDataByName("$App-NBID$"))

    QByteArray *pBa=
    	m_pAppHeaderFile->getSectionDataByName("$App SW Location Info$");
    if (pBa && (!pBa->isEmpty()) && (pBa->at(0) == 0) &&
    	(pBa->at(1) > 0 || pBa->at(1) < 5)) {

        ui->m_cbASLInfoSub0->setCurrentIndex(pBa->at(1)-1);
        on_m_cbASLInfoSub0_currentIndexChanged(pBa->at(1)-1);

        for (int i = 0; i < 8*pBa->at(1); i++)
            m_listAppASLInfo.at(i)->setText(Utility::formatHexNumber(pBa->at(i+2)&0xFF));
        for (int i = 8*pBa->at(1); i < m_listAppASLInfo.size(); i++)
            m_listAppASLInfo.at(i)->setText("00");
    }
}

void MainWindow::updateCal1HeaderOnGui()
{
    LISTWIDGET_SETTEXT(m_listCal1DLS,
    	m_pCal1HeaderFile->getSectionDataByName("$DLS$"));
    LISTWIDGET_SETTEXT(m_listCal1HPN,
    	m_pCal1HeaderFile->getSectionDataByName("$Hex Part Number$"))
    LISTWIDGET_SETTEXT(m_listCal1CCID,
    	m_pCal1HeaderFile->getSectionDataByName("$CCID$"))
}

void MainWindow::updateCal2HeaderOnGui()
{
    LISTWIDGET_SETTEXT(m_listCal2DLS,
    	m_pCal2HeaderFile->getSectionDataByName("$DLS$"));
    LISTWIDGET_SETTEXT(m_listCal2HPN,
    	m_pCal2HeaderFile->getSectionDataByName("$Hex Part Number$"))
    LISTWIDGET_SETTEXT(m_listCal2CCID,
    	m_pCal2HeaderFile->getSectionDataByName("$CCID$"))
}

void MainWindow::updateHeadersOnGui()
{
	updateAppHeaderOnGui();
	updateCal1HeaderOnGui();
	updateCal2HeaderOnGui();
}

int MainWindow::loadAppHeaderFile()
{
    if (m_pAppHeaderFile->load(FILENAME_APP_SIGNED_HEADER) == -1) {
    	messageBoxAlert("load app signed header file failure.");
    	return -1;
    }

#if 0//ndef F_NO_DEBUG
    for (int i = 0; i < m_pAppHeaderFile->m_sigSections.count(); i++)
    {
    	qDebug() << "section: " << m_pAppHeaderFile->m_sigSections[i].name;
    	qDebug() << qPrintable(Utility::formatByteArray(&m_pAppHeaderFile->m_sigSections[i].data));
    }

    for (int i = 0; i < m_pAppHeaderFile->m_infSections.count(); i++)
    {
    	qDebug() << "section: " << m_pAppHeaderFile->m_infSections[i].name;
    	qDebug() << qPrintable(Utility::formatByteArray(&m_pAppHeaderFile->m_infSections[i].data));
    }
#endif

    QString s;
    if (!m_pAppHeaderFile->appHeaderIsValid(s))
    {
    	messageBoxAlert(s);
    	return -1;
    }

    updateAppHeaderOnGui();

    return 0;
}

int MainWindow::loadCal1HeaderFile()
{
    if (m_pCal1HeaderFile->load(FILENAME_CAL1_SIGNED_HEADER) == -1) {
    	messageBoxAlert("Load cal1 signed header file failure.");
    	return -1;
    }

#if 0//ndef F_NO_DEBUG
    for (int i = 0; i < m_pCal1HeaderFile->m_sigSections.count(); i++)
    {
    	qDebug() << "section: " << m_pCal1HeaderFile->m_sigSections[i].name;
    	qDebug() << qPrintable(Utility::formatByteArray(&m_pCal1HeaderFile->m_sigSections[i].data));
    }

    for (int i = 0; i < m_pCal1HeaderFile->m_infSections.count(); i++)
    {
    	qDebug() << "section: " << m_pCal1HeaderFile->m_infSections[i].name;
    	qDebug() << qPrintable(Utility::formatByteArray(&m_pCal1HeaderFile->m_infSections[i].data));
    }
#endif

    QString s;
    if (!m_pCal1HeaderFile->calHeaderIsValid(s))
    {
    	messageBoxAlert(s);
    	return -1;
    }

	updateCal1HeaderOnGui();

    return 0;
}

int MainWindow::loadCal2HeaderFile()
{
    if (m_pCal2HeaderFile->load(FILENAME_CAL2_SIGNED_HEADER) == -1) {
    	messageBoxAlert("Load cal2 signed header file failure.");
    	return -1;
    }

#if 0//ndef F_NO_DEBUG
    for (int i = 0; i < m_pCal2HeaderFile->m_sigSections.count(); i++)
    {
    	qDebug() << "section: " << m_pCal2HeaderFile->m_sigSections[i].name;
    	qDebug() << qPrintable(Utility::formatByteArray(&m_pCal2HeaderFile->m_sigSections[i].data));
    }

    for (int i = 0; i < m_pCal2HeaderFile->m_infSections.count(); i++)
    {
    	qDebug() << "section: " << m_pCal2HeaderFile->m_infSections[i].name;
    	qDebug() << qPrintable(Utility::formatByteArray(&m_pCal2HeaderFile->m_infSections[i].data));
    }
#endif

    QString s;
    if (!m_pCal2HeaderFile->calHeaderIsValid(s))
    {
    	messageBoxAlert(s);
    	return -1;
    }

    updateCal2HeaderOnGui();

    return 0;
}

void MainWindow::messageBoxAlert(QString s)
{
    QMessageBox message(
        QMessageBox::Warning,
        "Warning", s,
        QMessageBox::Ok,
        NULL);
    message.exec();
}

int MainWindow::updateAppHeaderFile()
{
    bool ok;
    QByteArray ba;

    LISTWIDGET_TEXTSET(m_listAppDLS,
    	m_pAppHeaderFile->getSectionDataByName("$DLS$"));
    LISTWIDGET_TEXTSET(m_listAppHPN,
    	m_pAppHeaderFile->getSectionDataByName("$Hex Part Number$"));
    LISTWIDGET_TEXTSET(m_listAppBCID,
    	m_pAppHeaderFile->getSectionDataByName("$BCID$"));
    LISTWIDGET_TEXTSET(m_listAppNBID,
    	m_pAppHeaderFile->getSectionDataByName("$App-NBID$"));
#if 1
    LISTWIDGET_TEXTSET(m_listAppBCID,
    	m_pAppHeaderFile->getSectionDataByName("@BCID@"));
    LISTWIDGET_TEXTSET(m_listAppNBID,
    	m_pAppHeaderFile->getSectionDataByName("@App-NBID@"));
#endif

    int idx = ui->m_cbASLInfoSub0->currentIndex();
    int len = (idx+1)*8;
    ba.clear();
    ba.append((char)0);
    ba.append((char)(idx+1));
    for (int i = 0; i < len; i++)
    {
    	ba.append((char)m_listAppASLInfo.at(i)->text().toUInt(&ok, 16));
    }

    QByteArray *pBa;
    pBa = m_pAppHeaderFile->getSectionDataByName("$App SW Location Info$");
    if (pBa) *pBa = ba;
#if 1
    pBa = m_pAppHeaderFile->getSectionDataByName("@App SW Location Info@");
    if (pBa) *pBa = ba;
#endif
    return 0;
}

int MainWindow::updateCal1HeaderFile()
{
    bool ok;
    QByteArray ba;

    LISTWIDGET_TEXTSET(m_listCal1DLS,
    	m_pCal1HeaderFile->getSectionDataByName("$DLS$"));
    LISTWIDGET_TEXTSET(m_listCal1HPN,
    	m_pCal1HeaderFile->getSectionDataByName("$Hex Part Number$"));
    LISTWIDGET_TEXTSET(m_listCal1CCID,
    	m_pCal1HeaderFile->getSectionDataByName("$CCID$"));
#if 1
    LISTWIDGET_TEXTSET(m_listCal1CCID,
    	m_pCal1HeaderFile->getSectionDataByName("@CCID@"));
#endif
    return 0;
}

int MainWindow::updateCal2HeaderFile()
{
    bool ok;
    QByteArray ba;

    LISTWIDGET_TEXTSET(m_listCal2DLS,
    	m_pCal2HeaderFile->getSectionDataByName("$DLS$"));
    LISTWIDGET_TEXTSET(m_listCal2HPN,
    	m_pCal2HeaderFile->getSectionDataByName("$Hex Part Number$"));
    LISTWIDGET_TEXTSET(m_listCal2CCID,
    	m_pCal2HeaderFile->getSectionDataByName("$CCID$"));
#if 1
    LISTWIDGET_TEXTSET(m_listCal2CCID,
    	m_pCal2HeaderFile->getSectionDataByName("@CCID@"));
#endif

    return 0;
}

bool MainWindow::copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist)
{
    toDir.replace("\\","/");
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir)){
        return false;
    }
    QDir *createfile     = new QDir;
    bool exist = createfile->exists(toDir);
    if (exist){
        if(coverFileIfExist){
            createfile->remove(toDir);
        }
    }//end if

    if(!QFile::copy(sourceDir, toDir))
    {
        return false;
    }
    return true;
}

bool MainWindow::saveBinaryFiles(QString &outMsg)
{
	QString msg;
	QFile *outFile;
	qint64 v;
	QByteArray ba;

    if (m_baAppInfo.isEmpty()) {
        outMsg = tr("Please load the full content S19 file first.");
        return false;
    }

    msg.clear();
    ba = m_pAppHeaderFile->getHdrBinData(HDRFILE_TYPE_APP, msg);
    if (ba.isEmpty())
    {
    	outMsg = tr("Header file %1 save binary wrong: ").arg(m_pAppHeaderFile->m_fileName);
    	outMsg += msg;
        return false;
    }
    QString fileNameAppBin("APP.bin");
    if ((v = m_pAppHeaderFile->getHexPartNumber()) != -1)
    {
        fileNameAppBin = QString::number(v) + ".bin";
    }
    outFile = new QFile(fileNameAppBin);
    if (!outFile->open(QIODevice::WriteOnly))
    {
        delete outFile;
        return false;
    }
    outFile->write(ba);
    ba = HeaderFile::getBlockHeader(SECTION_INFO);
    outFile->write(ba);
    outFile->write(m_baAppInfo);
    outFile->write(m_baAppBlock);
    outFile->close();
    delete outFile;

    qInfo() << tr("Binary file %1 generated successfully.").arg(fileNameAppBin);

    msg.clear();
    ba = m_pCal1HeaderFile->getHdrBinData(HDRFILE_TYPE_CAL, msg);
    if (ba.isEmpty())
    {
        outMsg = tr("Header file %1 save binary wrong: ").arg(m_pCal1HeaderFile->m_fileName);
        outMsg += msg;
        return false;
    }
    QString fileNameCal1Bin("CAL1.bin");
    if ((v = m_pCal1HeaderFile->getHexPartNumber()) != -1)
    {
        fileNameCal1Bin = QString::number(v) + ".bin";
    }
    outFile = new QFile(fileNameCal1Bin);
    if (!outFile->open(QIODevice::WriteOnly))
    {
        delete outFile;
        return false;
    }
    outFile->write(ba);
    ba = HeaderFile::getBlockHeader(SECTION_INFO);
    outFile->write(ba);
    outFile->write(m_baCal1Info);
    outFile->write(m_baCal1Block);
    outFile->close();
    delete outFile;

    qInfo() << tr("Binary file %1 generated successfully.").arg(fileNameCal1Bin);

    msg.clear();
    ba = m_pCal2HeaderFile->getHdrBinData(HDRFILE_TYPE_CAL, msg);
    if (ba.isEmpty())
    {
        outMsg = tr("Header file %1 save binary wrong: ").arg(m_pCal2HeaderFile->m_fileName);
        outMsg += msg;
        return false;
    }
    QString fileNameCal2Bin("CAL2.bin");
    if ((v = m_pCal2HeaderFile->getHexPartNumber()) != -1)
    {
        fileNameCal2Bin = QString::number(v) + ".bin";
    }
    outFile = new QFile(fileNameCal2Bin);
    if (!outFile->open(QIODevice::WriteOnly))
    {
        delete outFile;
        return false;
    }
    outFile->write(ba);
    ba = HeaderFile::getBlockHeader(SECTION_INFO);
    outFile->write(ba);
    outFile->write(m_baCal2Info);
    outFile->write(m_baCal2Block);
    outFile->close();
    delete outFile;

    qInfo() << tr("Binary file %1 generated successfully.").arg(fileNameCal2Bin);

    outMsg = tr("Binary files saved success.");
    return true;
}

bool MainWindow::saveHexFiles(QString &outMsg)
{
    QByteArray ba, baHeader;
    qint64 v;
    quint32 baseAddr;
    bool ok;

    if (m_baAppInfo.isEmpty()) {
        outMsg = tr("Please load the full content S19 file first.");
        return false;
    }

    if (m_parameters["HEX_HEADER_ENABLE"].toInt(&ok, 16)) {
        baHeader = HeaderFile::getBlockHeader(SECTION_INFO);
    }

    QString fileNameAppHex("APP.hex");
    if ((v = m_pAppHeaderFile->getHexPartNumber()) != -1)
    {
        fileNameAppHex = QString::number(v) + ".hex";
    }
    ba.clear();
    ba = baHeader + m_baAppInfo + m_baAppBlock;
    baseAddr = m_parameters["ADDR_APPL_SWINFO"].toULong(&ok, 16);
    IHexFile::saveFileFromByteArray(ba, fileNameAppHex, baseAddr, IHEXFILE_HEX86);

    qInfo() << tr("Hex file %1 generated successfully.").arg(fileNameAppHex);

    QString fileNameCal1Hex("CAL1.hex");
    if ((v = m_pCal1HeaderFile->getHexPartNumber()) != -1)
    {
        fileNameCal1Hex = QString::number(v) + ".hex";
    }
    ba.clear();
    ba = baHeader + m_baCal1Info + m_baCal1Block;
    baseAddr = m_parameters["ADDR_CAL1_DATAINFO"].toULong(&ok, 16);
    IHexFile::saveFileFromByteArray(ba, fileNameCal1Hex, baseAddr, IHEXFILE_HEX86);

    qInfo() << tr("Hex file %1 generated successfully.").arg(fileNameCal1Hex);

    QString fileNameCal2Hex("CAL2.hex");
    if ((v = m_pCal2HeaderFile->getHexPartNumber()) != -1)
    {
        fileNameCal2Hex = QString::number(v) + ".hex";
    }
    ba.clear();
    ba = baHeader + m_baCal2Info + m_baCal2Block;
    baseAddr = m_parameters["ADDR_CAL2_DATAINFO"].toULong(&ok, 16);
    IHexFile::saveFileFromByteArray(ba, fileNameCal2Hex, baseAddr, IHEXFILE_HEX86);

    qInfo() << tr("Hex file %1 generated successfully.").arg(fileNameCal2Hex);

    outMsg = tr("Hex files saved success.");

    return true;
}


int MainWindow::loadS19File(SrecFile *pSrecordFile)
{
    return 0;
}

void MainWindow::updateTableView()
{
	int row = 0, col = 0;
	QStandardItem *item;

    m_model->removeRows(0, m_model->rowCount());
    ui->m_tblFileList->reset();

    m_model->setColumnCount(2);
    m_model->setHeaderData(col++, Qt::Horizontal, QStringLiteral("Type"));
    m_model->setHeaderData(col++, Qt::Horizontal, QStringLiteral("Name"));

    if (!m_pAppBinFile->m_fileName.isEmpty()) {
        col = 0;

        item = new QStandardItem(QIcon(QStringLiteral(":iconBin")), QStringLiteral("BIN-APP"));
        m_model->setItem(row, col++, item);

        item = new QStandardItem(m_pAppBinFile->m_fileName);
        m_model->setItem(row, col++, item);

        row++;
    }

    if (!m_pCal1BinFile->m_fileName.isEmpty()) {
        col = 0;

        item = new QStandardItem(QIcon(QStringLiteral(":iconBin")), QStringLiteral("BIN-CAL1"));
        m_model->setItem(row, col++, item);

        item = new QStandardItem(m_pCal1BinFile->m_fileName);
        m_model->setItem(row, col++, item);

        row++;
    }

    if (!m_pCal2BinFile->m_fileName.isEmpty()) {
        col = 0;

        item = new QStandardItem(QIcon(QStringLiteral(":iconBin")), QStringLiteral("BIN-CAL2"));
        m_model->setItem(row, col++, item);

        item = new QStandardItem(m_pCal2BinFile->m_fileName);
        m_model->setItem(row, col++, item);

        row++;
    }

    if (!m_pFullSrecordFile->m_fileName.isEmpty()) {
        col = 0;

        item = new QStandardItem(QIcon(QStringLiteral(":iconS19")), QStringLiteral("S19-FULL"));
        m_model->setItem(row, col++, item);

        item = new QStandardItem(m_pFullSrecordFile->m_fileName);
        m_model->setItem(row, col++, item);

        row++;
    }

    if (!m_pAppSrecordFile->m_fileName.isEmpty()) {
    	col = 0;

    	item = new QStandardItem(QIcon(QStringLiteral(":iconS19")), QStringLiteral("S19-APP"));
    	m_model->setItem(row, col++, item);

    	item = new QStandardItem(m_pAppSrecordFile->m_fileName);
    	m_model->setItem(row, col++, item);

    	row++;
    }

    if (!m_pCal1SrecordFile->m_fileName.isEmpty()) {
    	col = 0;

    	item = new QStandardItem(QIcon(QStringLiteral(":iconS19")), QStringLiteral("S19-CAL1"));
    	m_model->setItem(row, col++, item);

    	item = new QStandardItem(m_pCal1SrecordFile->m_fileName);
    	m_model->setItem(row, col++, item);

    	row++;
    }

    if (!m_pCal2SrecordFile->m_fileName.isEmpty()) {
    	col = 0;

    	item = new QStandardItem(QIcon(QStringLiteral(":iconS19")), QStringLiteral("S19-CAL2"));
    	m_model->setItem(row, col++, item);

    	item = new QStandardItem(m_pCal2SrecordFile->m_fileName);
    	m_model->setItem(row, col++, item);

    	row++;
    }

    ui->m_tblFileList->setModel(m_model);
    //ui->m_tblFileList->resizeColumnsToContents();
    //ui->m_tblMain->setColumnWidth(0,180);
    //ui->m_tblMain->setColumnWidth(1,240);
}

void MainWindow::on_m_pbClose_clicked()
{
    close();
}

void MainWindow::on_m_pbSaveHdr_clicked()
{
	QString msg;

    msg.clear();
    if (!m_pAppHeaderFile || !m_pCal1HeaderFile ||!m_pCal2HeaderFile)
    {
    	// should not comes here
    	return;
    }

    updateAppHeaderFile();
    updateCal1HeaderFile();
    updateCal2HeaderFile();

    if (m_pAppHeaderFile->save(FILENAME_APP_SIGNED_HEADER) != 0)
    {
    	msg += tr("File %1 saved failure.\n").arg(FILENAME_APP_SIGNED_HEADER);
    }

    if (m_pCal1HeaderFile->save(FILENAME_CAL1_SIGNED_HEADER) != 0)
    {
    	msg += tr("File %1 saved failure.\n").arg(FILENAME_CAL1_SIGNED_HEADER);
    }

    if (m_pCal2HeaderFile->save(FILENAME_CAL2_SIGNED_HEADER) != 0)
    {
    	msg += tr("File %1 saved failure.\n").arg(FILENAME_CAL2_SIGNED_HEADER);
    }

    if (!msg.isEmpty())
    {
    	messageBoxAlert(msg);
    }
    else
    {
    	statusBar()->showMessage(tr("Header files are saved success."), 2000);
    }
}

void MainWindow::on_m_pbSaveBin_clicked()
{
    QString outMsg;

    if (!saveBinaryFiles(outMsg)) {
        messageBoxAlert(outMsg);
        return;
    }
    statusBar()->showMessage(outMsg, 2000);

    if (!saveHexFiles(outMsg)) {
        messageBoxAlert(outMsg);
        return;
    }
    statusBar()->showMessage(outMsg, 2000);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
/*
    QMessageBox message(
        QMessageBox::Information,
        "About",
        "Motorola S-record to Binary Utility Version 1.2.01",
        QMessageBox::Ok,
        NULL);
    message.exec();
*/
    m_aboutDialog = new AboutDialog(this);

    m_aboutDialog->show();
}

void MainWindow::on_actionReLoad_triggered()
{
    loadAppHeaderFile();
    loadCal1HeaderFile();
    loadCal2HeaderFile();
}

void MainWindow::on_m_cbASLInfoSub0_currentIndexChanged(int index)
{
	switch(index)
	{
		case 0:
			ui->m_wgASLInfoSub1->setVisible(true);
			ui->m_wgASLInfoSub2->setVisible(false);
            ui->m_wgASLInfoSub3->setVisible(false);
            ui->m_wgASLInfoSub4->setVisible(false);
			break;
		case 1:
			ui->m_wgASLInfoSub1->setVisible(true);
			ui->m_wgASLInfoSub2->setVisible(true);
            ui->m_wgASLInfoSub3->setVisible(false);
            ui->m_wgASLInfoSub4->setVisible(false);
			break;
        case 2:
            ui->m_wgASLInfoSub1->setVisible(true);
            ui->m_wgASLInfoSub2->setVisible(true);
            ui->m_wgASLInfoSub3->setVisible(true);
            ui->m_wgASLInfoSub4->setVisible(false);
            break;
        case 3:
            ui->m_wgASLInfoSub1->setVisible(true);
            ui->m_wgASLInfoSub2->setVisible(true);
            ui->m_wgASLInfoSub3->setVisible(true);
            ui->m_wgASLInfoSub4->setVisible(true);
            break;
	}
}

bool MainWindow::loadFullFile(const QString &fileName, QString &outMsg)
{
    bool ok;
    quint32 addr;
    int size;
    QFileInfo fi(fileName);

    if (m_pFullSrecordFile->load(fileName) == -1)
    {
        outMsg = tr("File %1 load failure.").arg(fileName);
        return false;
    }

    addr = m_parameters["ADDR_CAL1_DATAINFO"].toULong(&ok, 16);
    size = m_parameters["SIZE_CAL1_DATAINFO"].toInt(&ok, 16);
    m_baCal1Info = m_pFullSrecordFile->getBinData(addr, size);
    if (m_baCal1Info.isEmpty()) {
        outMsg = tr("File %1 load failure: cal1 info read error.").arg(fileName);
        return false;
    }
#ifndef F_NO_DEBUG
    //qDebug() << "CAL1_INFO(" << ba.count() << "):" << ba.toHex();
#endif
    //m_pCal1HeaderFile->loadInfoSection(m_baCal1Info);

    // read cal2 info header
    addr = m_parameters["ADDR_CAL2_DATAINFO"].toULong(&ok, 16);
    size = m_parameters["SIZE_CAL2_DATAINFO"].toInt(&ok, 16);
    m_baCal2Info = m_pFullSrecordFile->getBinData(addr, size);
    if (m_baCal2Info.isEmpty()) {
        outMsg = tr("File %1 load failure: cal2 info read error.").arg(fileName);
        return false;
    }
#ifndef F_NO_DEBUG
    //qDebug() << "CAL2_INFO(" << ba.count() << "):" << ba.toHex();
#endif
    //m_pCal2HeaderFile->loadInfoSection(m_baCal2Info);

    // read app info header
    addr = m_parameters["ADDR_APPL_SWINFO"].toULong(&ok, 16);
    size = m_parameters["SIZE_APPL_SWINFO"].toInt(&ok, 16);
    m_baAppInfo = m_pFullSrecordFile->getBinData(addr, size);
    if (m_baAppInfo.isEmpty()) {
        outMsg = tr("File %1 load failure: app info read error.").arg(fileName);
        return false;
    }
#ifndef F_NO_DEBUG
    qDebug() << "APP_INFO(" << m_baAppInfo.count() << "):" << m_baAppInfo.toHex();
#endif
    //m_pAppHeaderFile->loadInfoSection(m_baAppInfo);

    // read cal2 block
    addr = m_parameters["ADDR_CAL2_BLOCK"].toULong(&ok, 16);
    size = m_parameters["SIZE_CAL2_BLOCK"].toInt(&ok, 16);
    m_baCal2Block = m_pFullSrecordFile->getBinData(addr, size);
    if (m_baCal2Block.isEmpty()) {
        outMsg = tr("File %1 load failure: cal2 block read error.").arg(fileName);
        return false;
    }
#ifndef F_NO_DEBUG
    //qDebug() << "CAL2_INFO(" << baCal2Block.count() << "):" << baCal2Block.toHex();
#endif

    // read cal1 block
    addr = m_parameters["ADDR_CAL1_BLOCK"].toULong(&ok, 16);
    size = m_parameters["SIZE_CAL1_BLOCK"].toInt(&ok, 16);
    m_baCal1Block = m_pFullSrecordFile->getBinData(addr, size);
    if (m_baCal1Block.isEmpty()) {
        outMsg = tr("File %1 load failure: cal1 block read error.").arg(fileName);
        return false;
    }
#ifndef F_NO_DEBUG
    //qDebug() << "CAL1_INFO(" << baCal1Block.count() << "):" << baCal1Block.toHex();
#endif

    // read app block
    addr = m_parameters["ADDR_APPL_BLOCK"].toULong(&ok, 16);
    size = m_parameters["SIZE_APPL_BLOCK"].toInt(&ok, 16);
    m_baAppBlock = m_pFullSrecordFile->getBinData(addr, size);
    if (m_baAppBlock.isEmpty()) {
        outMsg = tr("File %1 load failure: app block read error.").arg(fileName);
        return false;
    }
#ifndef F_NO_DEBUG
    {
        QByteArray tmpBa = m_baAppBlock.mid(0, 128);
        qDebug() << "APP_BLOCK(" << tmpBa.count() << "):" << tmpBa.toHex();
    }
#endif

    outMsg = tr("File %1 loaded success.").arg(fileName);

    return true;
}

void MainWindow::on_actionLoad_S19_File_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open"), QString(), tr("Motorola S-record Files (*.s19);;Binary Files (*.bin);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;

    QString outMsg;
    if (!loadFullFile(fileName, outMsg)) {
        messageBoxAlert(outMsg);
        return;
    }

    updateCal1HeaderOnGui();
    updateCal2HeaderOnGui();
    updateAppHeaderOnGui();
    updateTableView();

    statusBar()->showMessage(outMsg, 2000);
}

void MainWindow::on_actionExtract_App_Header_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open"), QString(), tr("Binary Files (*.bin);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;

    QFileInfo fi(fileName);
    QString ext = fi.suffix();
    if (ext.toLower() != "bin") {
        messageBoxAlert("File type is wrong, please select a right file to process.");
        return;
    }

    HeaderFile *pHeaderFile = new HeaderFile(HDRFILE_TYPE_APP);
    BinFile *pBinFile = new BinFile(pHeaderFile, NULL);
    if (pBinFile->load(fileName, HDRFILE_TYPE_APP) == -1)
    {
        messageBoxAlert(tr("File %1 load failure.").arg(fileName));
        return;
    }

    pHeaderFile->save(FILENAME_EXTRACT_HDR_APP);

    statusBar()->showMessage(tr("APPL header has saved to %1.").arg(FILENAME_EXTRACT_HDR_APP), 2000);
}

void MainWindow::on_actionExtract_Cal1_Header_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open"), QString(), tr("Binary Files (*.bin);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;

    QFileInfo fi(fileName);
    QString ext = fi.suffix();
    if (ext.toLower() != "bin") {
        messageBoxAlert("File type is wrong, please select a right file to process.");
        return;
    }

    HeaderFile *pHeaderFile = new HeaderFile(HDRFILE_TYPE_CAL);
    BinFile *pBinFile = new BinFile(pHeaderFile, NULL);
    if (pBinFile->load(fileName, HDRFILE_TYPE_CAL) == -1)
    {
        messageBoxAlert(tr("File %1 load failure.").arg(fileName));
        return;
    }

    pHeaderFile->save(FILENAME_EXTRACT_HDR_CAL1);

    statusBar()->showMessage(tr("CAL1 header has saved to %1.").arg(FILENAME_EXTRACT_HDR_CAL1), 2000);
}

void MainWindow::on_actionExtract_Cal2_Header_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open"), QString(), tr("Binary Files (*.bin);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;

    QFileInfo fi(fileName);
    QString ext = fi.suffix();
    if (ext.toLower() != "bin") {
        messageBoxAlert("File type is wrong, please select a right file to process.");
        return;
    }

    HeaderFile *pHeaderFile = new HeaderFile(HDRFILE_TYPE_CAL);
    BinFile *pBinFile = new BinFile(pHeaderFile, NULL);
    if (pBinFile->load(fileName, HDRFILE_TYPE_CAL) == -1)
    {
        messageBoxAlert(tr("File %1 load failure.").arg(fileName));
        return;
    }

    pHeaderFile->save(FILENAME_EXTRACT_HDR_CAL2);

    statusBar()->showMessage(tr("CAL2 header has saved to %1.").arg(FILENAME_EXTRACT_HDR_CAL2), 2000);
}

void MainWindow::on_actionPreferences_triggered()
{
    m_configDialog = new ConfigDialog(&m_parameters, this);
    m_configDialog->show();
}
