#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Utility.h"

#include <QDesktopWidget>
#include <QFileDialog>
#include <QTextCursor>
#include <QTextBlock>
#include <QRegExp>

#define F_NO_DEBUG

#ifndef F_NO_DEBUG
#include <QTime>
#include <QDebug>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pSrecordFile()
{
    ui->setupUi(this);

    const QRect ag = QApplication::desktop()->availableGeometry(this);
    qint32 w = ag.width() * 1 / 2;
    resize(w , w * 3 / 4);
    //move(ag.width() / 8, 0);

    //ui->m_pteSignature->document()->setMaximumBlockCount(30000);

    //QWidget *pWin = QApplication::activeWindow;

    QGroupBox *gb;

    gb = this->findChild<QGroupBox *>("m_gbModuleId");
    if (gb != NULL)
    {
        m_listModuleId = gb->findChildren<QLineEdit *>();
    }

    gb = this->findChild<QGroupBox *>("m_gbBcId");
    if (gb != NULL)
    {
        m_listBcId = gb->findChildren<QLineEdit *>();
    }

    gb = this->findChild<QGroupBox *>("m_gbEcuName");
    if (gb != NULL)
    {
        m_listEcuName = gb->findChildren<QLineEdit *>();
#if 0
        for (int i = 0; i < m_listEcuName.count(); i++)
        {
            qDebug() << m_listEcuName.at(i)->objectName();
        }
#endif
    }

    gb = this->findChild<QGroupBox *>("m_gbEcuId");
    if (gb != NULL)
    {
        m_listEcuId = gb->findChildren<QLineEdit *>();
    }

    gb = this->findChild<QGroupBox *>("m_gbApNbid");
    if (gb != NULL)
    {
        m_listApNbid = gb->findChildren<QLineEdit *>();
    }

    gb = this->findChild<QGroupBox *>("m_gbMsgDig");
    if (gb != NULL)
    {
        m_listMsgDig = gb->findChildren<QLineEdit *>();
    }

    QRegExp rx("^[0-9a-fA-F]{2}$");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    QList<QLineEdit *> list = this->findChildren<QLineEdit *>();
    for (int i = 0; i < list.count(); i++)
        list.at(i)->setValidator(pReg);

    connect(ui->m_pteASLInfo, &QPlainTextEdit::cursorPositionChanged, this, &pteASLInfoLineAction);
    connect(ui->m_pteSignerInfo, &QPlainTextEdit::cursorPositionChanged, this, &pteSignerInfoLineAction);
    connect(ui->m_pteSignature, &QPlainTextEdit::cursorPositionChanged, this, &pteSignatureLineAction);

    loadAppHeaderFile();

    //ui->m_pbSave->setCheckable(true);
    //connect(ui->m_pbSave, &QPushButton::toggled, ui->m_gbEcuId, &QGroupBox::setVisible);

}

MainWindow::~MainWindow()
{
    delete ui;

    if (m_pSrecordFile) delete m_pSrecordFile;
    if (m_pAppHeaderFile) delete m_pAppHeaderFile;
}

int MainWindow::loadAppHeaderFile()
{
    m_pAppHeaderFile = new AppHeaderFile();
    m_pAppHeaderFile->load(FILENAME_APP_SIGNED_HEADER);
    //qDebug() << m_pAppHeaderFile->m_ecuName.toHex();
    //qDebug() << m_pAppHeaderFile->m_msgDigest.toHex();

#define LISTWIDGET_SETTEXT(l, a) \
    if ((l).count() == (a).count()) \
    { \
        for (int i = 0; i < (l).count(); i++) \
            (l).at(i)->setText(Utility::formatHexNumber((a).at(i)&0xFF)); \
    }

    LISTWIDGET_SETTEXT(m_listModuleId, m_pAppHeaderFile->m_moduleId)
    LISTWIDGET_SETTEXT(m_listBcId, m_pAppHeaderFile->m_bcid)
    LISTWIDGET_SETTEXT(m_listEcuName, m_pAppHeaderFile->m_ecuName)
    LISTWIDGET_SETTEXT(m_listEcuId, m_pAppHeaderFile->m_ecuId)
    LISTWIDGET_SETTEXT(m_listApNbid, m_pAppHeaderFile->m_appNbid)
    LISTWIDGET_SETTEXT(m_listMsgDig, m_pAppHeaderFile->m_msgDigest)

    ui->m_pteASLInfo->clear();
    ui->m_pteASLInfo->appendPlainText(
        Utility::formatByteArray(&m_pAppHeaderFile->m_aslInfo)
        );
    ui->m_pteSignerInfo->clear();
    ui->m_pteSignerInfo->appendPlainText(
        Utility::formatByteArray(&m_pAppHeaderFile->m_signerInfo)
        );
    ui->m_pteSignature->clear();
    ui->m_pteSignature->appendPlainText(
        Utility::formatByteArray(&m_pAppHeaderFile->m_signature)
        );

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
    QByteArray ba, baAslInfo, baSignerInfo, baSignature;
    QString s;

    s = ui->m_pteASLInfo->toPlainText().simplified().replace(" ", "");
    baAslInfo = QByteArray::fromHex(s.toLatin1());
    if ((baAslInfo.count()-2)%8 != 0)
    {
        messageBoxAlert("App SW Location Info input length is wrong!");
        return -1;
    }

    s = ui->m_pteSignerInfo->toPlainText().simplified().replace(" ", "");
    baSignerInfo = QByteArray::fromHex(s.toLatin1());
    if (baSignerInfo.count() != SIZE_SINFO)
    {
        messageBoxAlert("Signer Info input length is wrong!");
        return -1;
    }

    s = ui->m_pteSignature->toPlainText().simplified().replace(" ", "");
    baSignature = QByteArray::fromHex(s.toLatin1());
    if (baSignature.count() != SIZE_SIGNATURE)
    {
        messageBoxAlert("Signature input length is wrong!");
        return -1;
    }

#define LISTWIDGET_TEXTSET(l, a) \
    ba.clear(); \
    for (int i = 0; i < (l).count(); i++) \
    { \
        ba.append((char)(l).at(i)->text().toUInt(&ok, 16)); \
    } \
    (a) = ba;

    LISTWIDGET_TEXTSET(m_listModuleId, m_pAppHeaderFile->m_moduleId)
    LISTWIDGET_TEXTSET(m_listBcId, m_pAppHeaderFile->m_bcid)
    LISTWIDGET_TEXTSET(m_listEcuName, m_pAppHeaderFile->m_ecuName)
    LISTWIDGET_TEXTSET(m_listEcuId, m_pAppHeaderFile->m_ecuId)
    LISTWIDGET_TEXTSET(m_listApNbid, m_pAppHeaderFile->m_appNbid)
    LISTWIDGET_TEXTSET(m_listMsgDig, m_pAppHeaderFile->m_msgDigest)

    m_pAppHeaderFile->m_aslInfo = baAslInfo;
    m_pAppHeaderFile->m_signerInfo = baSignerInfo;
    m_pAppHeaderFile->m_signature = baSignature;

    return 0;
}

int MainWindow::getLineNumberByCursor(QPlainTextEdit *pte)
{
    //当前光标
    QTextCursor tc = pte->textCursor();
    QTextLayout *pLayout = tc.block().layout();

    //当前光标在本BLOCK内的相对位置
    int nCurpos = tc.position() - tc.block().position();
    int nTextline = pLayout->lineForTextPosition(nCurpos).lineNumber() + tc.block().firstLineNumber();

    return nTextline+1;
}

void MainWindow::pteASLInfoLineAction()
{
    int line = getLineNumberByCursor(ui->m_pteASLInfo);
    statusBar()->showMessage(tr("line %1 selected").arg(line), 2000);
}

void MainWindow::pteSignerInfoLineAction()
{
    int line = getLineNumberByCursor(ui->m_pteSignerInfo);
    statusBar()->showMessage(tr("line %1 selected").arg(line), 2000);
}

void MainWindow::pteSignatureLineAction()
{
    int line = getLineNumberByCursor(ui->m_pteSignature);
    statusBar()->showMessage(tr("line %1 selected").arg(line), 2000);
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

void MainWindow::saveAppHeaderFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open"), QString(), tr("Motorola S-record Files (*.s19);;All Files (*.*)"));

    if (fileName.isEmpty())
        return;

    m_pSrecordFile = new SrecFile(fileName);

    if (m_pSrecordFile == NULL)
    {
        statusBar()->showMessage(tr("File %1 load failure.").arg(fileName), 2000);
        return;
    }

    statusBar()->showMessage(tr("File %1 loaded success.").arg(fileName), 2000);
}

void MainWindow::on_actionLoad_File_triggered()
{
    saveAppHeaderFile();
}

void MainWindow::on_m_pbClose_clicked()
{
    close();
}

void MainWindow::on_m_pbSaveHdr_clicked()
{
    updateAppHeaderFile();

    if (m_pAppHeaderFile)
    {
        if (m_pAppHeaderFile->save(FILENAME_APP_SIGNED_HEADER) == 0)
        {
            statusBar()->showMessage(tr("File %1 saved success.").arg(FILENAME_APP_SIGNED_HEADER), 2000);
            return;
        }
    }
    statusBar()->showMessage(tr("File %1 saved failure.").arg(FILENAME_APP_SIGNED_HEADER), 2000);
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
        "Motorola S-record to Binary Utility Version 1.0.01",
        QMessageBox::Ok,
        NULL);
    message.exec();
}

void MainWindow::on_actionReLoad_triggered()
{
    loadAppHeaderFile();
}
