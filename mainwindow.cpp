#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Utility.h"

#include <QDesktopWidget>
#include <QFileDialog>
#include <QTextCursor>
#include <QTextBlock>
#include <QRegExp>

//#define F_NO_DEBUG

#ifndef F_NO_DEBUG
#include <QTime>
#include <QDebug>
#endif

#define LISTWIDGET_SETTEXT(l, a) \
    if ((l).count() == (a)->count()) \
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
    m_pSrecordFile()
{
    ui->setupUi(this);

    const QRect ag = QApplication::desktop()->availableGeometry(this);
    qint32 w = ag.width() * 1 / 2;
    resize(w , w * 3 / 4);

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

    on_actionReLoad_triggered();

}

MainWindow::~MainWindow()
{
    delete ui;

    if (m_pSrecordFile) delete m_pSrecordFile;
    if (m_pAppHeaderFile) delete m_pAppHeaderFile;
    if (m_pCal1HeaderFile) delete m_pCal1HeaderFile;
}

int MainWindow::loadAppHeaderFile()
{
    m_pAppHeaderFile = new HeaderFile();
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
    }

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
    if (!pBa->isEmpty() && (pBa->at(0) == 0) &&
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

    return 0;
}

int MainWindow::loadCal1HeaderFile()
{
    m_pCal1HeaderFile = new HeaderFile();
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
    }

    LISTWIDGET_SETTEXT(m_listCal1DLS,
    	m_pCal1HeaderFile->getSectionDataByName("$DLS$"));
    LISTWIDGET_SETTEXT(m_listCal1HPN,
    	m_pCal1HeaderFile->getSectionDataByName("$Hex Part Number$"))
    LISTWIDGET_SETTEXT(m_listCal1CCID,
    	m_pCal1HeaderFile->getSectionDataByName("$CCID$"))

    return 0;
}

int MainWindow::loadCal2HeaderFile()
{
    m_pCal2HeaderFile = new HeaderFile();
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
    }

    LISTWIDGET_SETTEXT(m_listCal2DLS,
    	m_pCal2HeaderFile->getSectionDataByName("$DLS$"));
    LISTWIDGET_SETTEXT(m_listCal2HPN,
    	m_pCal2HeaderFile->getSectionDataByName("$Hex Part Number$"))
    LISTWIDGET_SETTEXT(m_listCal2CCID,
    	m_pCal2HeaderFile->getSectionDataByName("$CCID$"))

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
    LISTWIDGET_TEXTSET(m_listAppBCID,
    	m_pAppHeaderFile->getSectionDataByName("@BCID@"));
    LISTWIDGET_TEXTSET(m_listAppNBID,
    	m_pAppHeaderFile->getSectionDataByName("@App-NBID@"));

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
    pBa = m_pAppHeaderFile->getSectionDataByName("@App SW Location Info@");
    if (pBa) *pBa = ba;

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
    LISTWIDGET_TEXTSET(m_listCal1CCID,
    	m_pCal1HeaderFile->getSectionDataByName("@CCID@"));

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
    LISTWIDGET_TEXTSET(m_listCal2CCID,
    	m_pCal2HeaderFile->getSectionDataByName("@CCID@"));

    return 0;
}

void MainWindow::saveBinaryFile()
{
    //qDebug() << "m_pSrecordFile = " << m_pSrecordFile;
    if (!m_pSrecordFile)
    {
        QMessageBox message(
            QMessageBox::Warning,
            "Warning","Please load the Motorola S-record file first.",
            QMessageBox::Ok,
            NULL);
        message.exec();
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save"), QString(), tr("Binary Files (*.bin);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;

    //qDebug() << fileName;

    QFile *outFile = new QFile(fileName);
    if (!outFile->open(QIODevice::WriteOnly))
    {
        delete outFile;
        return;
    }

    const char dataType[] = {0x03, 0x01};
#if 0
    //QByteArray baDataType(QByteArray::fromRawData(dataType, sizeof(dataType)));
    outFile->write(dataType, sizeof(dataType));
    outFile->write(m_pAppHeaderFile->m_moduleId);
    outFile->write(m_pAppHeaderFile->m_bcid);
    outFile->write(m_pAppHeaderFile->m_ecuName);
    outFile->write(m_pAppHeaderFile->m_ecuId);
    outFile->write(m_pAppHeaderFile->m_appNbid);
    outFile->write(m_pAppHeaderFile->m_aslInfo);
    outFile->write(m_pAppHeaderFile->m_msgDigest);
    outFile->write(m_pAppHeaderFile->m_signerInfo);
    outFile->write(m_pAppHeaderFile->m_signature);
#endif

    // then s19 file in binary format
    for (auto d : m_pSrecordFile->m_dataRecords)
    {
        QByteArray &ba = d.binData;

#ifndef F_NO_DEBUG
        qDebug() << ba.count() << ":" << ba.toHex();
#endif
        outFile->write(ba);
    }

    outFile->close();
    delete outFile;

    statusBar()->showMessage(tr("Binary file %1 saved success.").arg(fileName), 2000);
}

void MainWindow::loadS19File(SrecFile *pSrecordFile)
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open"), QString(), tr("Motorola S-record Files (*.s19);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;

    pSrecordFile = new SrecFile(fileName);

    if (pSrecordFile == NULL)
    {
        statusBar()->showMessage(tr("File %1 load failure.").arg(fileName), 2000);
        return;
    }

    statusBar()->showMessage(tr("File %1 loaded success.").arg(fileName), 2000);
}

void MainWindow::on_actionLoad_File_triggered()
{
    loadS19File(m_pSrecordFile);
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
    saveBinaryFile();
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
        "Motorola S-record to Binary Utility Version 1.1.01",
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
