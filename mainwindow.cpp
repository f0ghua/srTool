#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Utility.h"

#include <QDesktopWidget>
#include <QFileDialog>
#include <QDir>
#include <QTextCursor>
#include <QTextBlock>
#include <QRegExp>
#include <QIcon>

//#define F_NO_DEBUG

#ifndef F_NO_DEBUG
#include <QTime>
#include <QDebug>
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
    gb = this->findChild<QGroupBox *>("m_gbAppASLInfo");
    if (gb != NULL)
    {
        m_listAppASLInfo = gb->findChildren<QLineEdit *>();
#ifndef F_NO_DEBUG
        for (int i = 0; i < m_listAppASLInfo.count(); i++)
        {
            qDebug() << m_listAppASLInfo.at(i)->objectName();
        }
#endif
    }
    QWidget *wd;
    wd = this->findChild<QWidget *>("m_wgASLInfoSub1");
    if (wd != NULL)
    {
        m_listAppASLInfoSub1 = wd->findChildren<QLineEdit *>();
    }
    wd = this->findChild<QWidget *>("m_wgASLInfoSub2");
    if (wd != NULL)
    {
        m_listAppASLInfoSub2 = wd->findChildren<QLineEdit *>();
    }

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

    m_pAppHeaderFile = new HeaderFile();
    m_pCal1HeaderFile = new HeaderFile();
    m_pCal2HeaderFile = new HeaderFile();
    m_pAppSrecordFile = new SrecFile();
    m_pCal1SrecordFile = new SrecFile();
    m_pCal2SrecordFile = new SrecFile();

    m_pAppBinFile = new BinFile(m_pAppHeaderFile, m_pAppSrecordFile);
    m_pCal1BinFile = new BinFile(m_pCal1HeaderFile, m_pCal1SrecordFile);
    m_pCal2BinFile = new BinFile(m_pCal2HeaderFile, m_pCal2SrecordFile);

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
    	(pBa->at(1) == 1 || pBa->at(1) == 2)) {
    	if (pBa->at(1) == 1) {
    		ui->m_cbASLInfoSub0->setCurrentIndex(0);
    		on_m_cbASLInfoSub0_currentIndexChanged(0);
    	}
    	else {
    		ui->m_cbASLInfoSub0->setCurrentIndex(1);
    		on_m_cbASLInfoSub0_currentIndexChanged(1);
    	}

        for (int i = 0; i < 8*pBa->at(1); i++)
            m_listAppASLInfo.at(i)->setText(Utility::formatHexNumber(pBa->at(i+2)&0xFF));
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

#ifndef F_NO_DEBUG
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
    /*
    LISTWIDGET_TEXTSET(m_listAppBCID,
    	m_pAppHeaderFile->getSectionDataByName("@BCID@"));
    LISTWIDGET_TEXTSET(m_listAppNBID,
    	m_pAppHeaderFile->getSectionDataByName("@App-NBID@"));
    */

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
	/*
    pBa = m_pAppHeaderFile->getSectionDataByName("@App SW Location Info@");
    if (pBa) *pBa = ba;
	*/
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
    /*
    LISTWIDGET_TEXTSET(m_listCal1CCID,
    	m_pCal1HeaderFile->getSectionDataByName("@CCID@"));
	*/
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
    /*
    LISTWIDGET_TEXTSET(m_listCal2CCID,
    	m_pCal2HeaderFile->getSectionDataByName("@CCID@"));
    */

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

void MainWindow::saveBinaryFiles()
{
	QString msg;
	QFile *outFile;
	qint64 v;
	QByteArray ba;

    if (!m_pAppSrecordFile || !m_pCal1SrecordFile || !m_pCal2SrecordFile)
    {
        QMessageBox message(
            QMessageBox::Warning,
            "Warning","Please load the Motorola S-record files first.",
            QMessageBox::Ok,
            NULL);
        message.exec();
        return;
    }
/*
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save"), QString(), tr("Binary Files (*.bin);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;
*/
    QString fileNameAppBin("APP.bin");
    QString fileNameAppHex("APP.hex");
    if ((v = m_pAppHeaderFile->getHexPartNumber()) != -1)
    {
    	fileNameAppBin = QString::number(v) + ".bin";
    	fileNameAppHex = QString::number(v) + ".hex";
    }
    outFile = new QFile(fileNameAppBin);
    if (!outFile->open(QIODevice::WriteOnly))
    {
        delete outFile;
        return;
    }
    msg.clear();
    ba = m_pAppHeaderFile->getBinData(HDRFILE_TYPE_APP, msg);
    if (ba.isEmpty())
    {
    	QString tempStr;

    	tempStr = tr("Header file %1 save binary wrong:\n").arg(m_pAppHeaderFile->m_fileName);
    	tempStr += msg;
    	messageBoxAlert(tempStr);
        delete outFile;
        return;
    }
    outFile->write(ba);
    // then s19 file in binary format
    for (auto d : m_pAppSrecordFile->m_dataRecords)
    {
        QByteArray &rBa = d.binData;

#if 0//ndef F_NO_DEBUG
        qDebug() << ba.count() << ":" << ba.toHex();
#endif
        outFile->write(rBa);
    }
    outFile->close();
    delete outFile;
    copyFileToPath(m_pAppSrecordFile->m_fileFullPath, fileNameAppHex, true);

    QString fileNameCal1Bin("CAL1.bin");
    QString fileNameCal1Hex("CAL1.hex");
    if ((v = m_pCal1HeaderFile->getHexPartNumber()) != -1)
    {
    	fileNameCal1Bin = QString::number(v) + ".bin";
    	fileNameCal1Hex = QString::number(v) + ".hex";
    }
    outFile = new QFile(fileNameCal1Bin);
    if (!outFile->open(QIODevice::WriteOnly))
    {
        delete outFile;
        return;
    }
    msg.clear();
    ba = m_pCal1HeaderFile->getBinData(HDRFILE_TYPE_CAL, msg);
    if (ba.isEmpty())
    {
    	QString tempStr;

    	tempStr = tr("Header file %1 save binary wrong:\n").arg(m_pCal1HeaderFile->m_fileName);
    	tempStr += msg;
    	messageBoxAlert(tempStr);
        delete outFile;
        return;
    }
    outFile->write(ba);
    // then s19 file in binary format
    for (auto d : m_pCal1SrecordFile->m_dataRecords)
    {
        QByteArray &rBa = d.binData;

#if 0//ndef F_NO_DEBUG
        qDebug() << ba.count() << ":" << ba.toHex();
#endif
        outFile->write(rBa);
    }
    outFile->close();
    delete outFile;
    copyFileToPath(m_pCal1SrecordFile->m_fileFullPath, fileNameCal1Hex, true);

    QString fileNameCal2Bin("CAL2.bin");
    QString fileNameCal2Hex("CAL2.hex");

    if ((v = m_pCal2HeaderFile->getHexPartNumber()) != -1)
    {
    	fileNameCal2Bin = QString::number(v) + ".bin";
    	fileNameCal2Hex = QString::number(v) + ".hex";
    }
    outFile = new QFile(fileNameCal2Bin);
    if (!outFile->open(QIODevice::WriteOnly))
    {
        delete outFile;
        return;
    }
    msg.clear();
    ba = m_pCal2HeaderFile->getBinData(HDRFILE_TYPE_CAL, msg);
    if (ba.isEmpty())
    {
    	QString tempStr;

    	tempStr = tr("Header file %1 save binary wrong:\n").arg(m_pCal2HeaderFile->m_fileName);
    	tempStr += msg;
    	messageBoxAlert(tempStr);
        delete outFile;
        return;
    }
    outFile->write(ba);
    // then s19 file in binary format
    for (auto d : m_pCal2SrecordFile->m_dataRecords)
    {
        QByteArray &rBa = d.binData;

#if 0//ndef F_NO_DEBUG
        qDebug() << ba.count() << ":" << ba.toHex();
#endif
        outFile->write(rBa);
    }
    outFile->close();
    delete outFile;
    copyFileToPath(m_pCal2SrecordFile->m_fileFullPath, fileNameCal2Hex, true);


    statusBar()->showMessage(tr("Binary files saved success."), 2000);
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
    saveBinaryFiles();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox message(
        QMessageBox::Information,
        "About",
        "Motorola S-record to Binary Utility Version 1.1.04",
        QMessageBox::Ok,
        NULL);
    message.exec();
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
			break;
		case 1:
			ui->m_wgASLInfoSub1->setVisible(true);
			ui->m_wgASLInfoSub2->setVisible(true);
			break;
	}
}

void MainWindow::on_actionLoad_Cal1_Srec_File_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open"), QString(), tr("Motorola S-record Files (*.s19);;Binary Files (*.bin);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;

    QFileInfo fi(fileName);
    QString ext = fi.suffix();

    if (ext.toLower() == "bin")
    {
        if (m_pCal1BinFile->load(fileName, HDRFILE_TYPE_CAL) == -1)
        {
            statusBar()->showMessage(tr("File %1 load failure.").arg(fileName), 2000);
            return;
        }
    	updateCal1HeaderOnGui();

    	statusBar()->showMessage(tr("Update app signed header from %1.").arg(fileName), 2000);
	}
	else
	{
		if (m_pCal1SrecordFile->load(fileName) == -1)
		{
			statusBar()->showMessage(tr("File %1 load failure.").arg(fileName), 2000);
			return;
		}

		statusBar()->showMessage(tr("File %1 loaded success.").arg(fileName), 2000);
	}
    updateTableView();
}

void MainWindow::on_actionLoad_Cal2_Srec_File_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open"), QString(), tr("Motorola S-record Files (*.s19);;Binary Files (*.bin);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;

    QFileInfo fi(fileName);
    QString ext = fi.suffix();

    if (ext.toLower() == "bin")
    {
        if (m_pCal2BinFile->load(fileName, HDRFILE_TYPE_CAL) == -1)
        {
            statusBar()->showMessage(tr("File %1 load failure.").arg(fileName), 2000);
            return;
        }
    	updateCal2HeaderOnGui();

    	statusBar()->showMessage(tr("Update app signed header from %1.").arg(fileName), 2000);
	}
	else
	{
		if (m_pCal2SrecordFile->load(fileName) == -1)
		{
			statusBar()->showMessage(tr("File %1 load failure.").arg(fileName), 2000);
			return;
		}
		statusBar()->showMessage(tr("File %1 loaded success.").arg(fileName), 2000);
	}
    updateTableView();
}

void MainWindow::on_actionLoad_App_Srec_File_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open"), QString(), tr("Motorola S-record Files (*.s19);;Binary Files (*.bin);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;

    QFileInfo fi(fileName);
    QString ext = fi.suffix();

    if (ext.toLower() == "bin")
    {
    	if (m_pAppBinFile->load(fileName, HDRFILE_TYPE_APP) == -1)
        {
            statusBar()->showMessage(tr("File %1 load failure.").arg(fileName), 2000);
            return;
        }
    	updateAppHeaderOnGui();

    	statusBar()->showMessage(tr("Update app signed header from %1.").arg(fileName), 2000);
	}
	else
	{
		if (m_pAppSrecordFile->load(fileName) == -1)
		{
			statusBar()->showMessage(tr("File %1 load failure.").arg(fileName), 2000);
			return;
		}

		statusBar()->showMessage(tr("File %1 loaded success.").arg(fileName), 2000);
	}

    updateTableView();
}
