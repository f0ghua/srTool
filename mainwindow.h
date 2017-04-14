#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "SrecFile.h"
#include "HeaderFile.h"
#include "BinFile.h"
#include "IHexFile.h"
#include "AboutDialog.h"
#include "ConfigDialog.h"

#define FILENAME_APP_SIGNED_HEADER      "AppHeaderForGeneration.txt"
#define FILENAME_CAL1_SIGNED_HEADER     "Cal1HeaderForGeneration.txt"
#define FILENAME_CAL2_SIGNED_HEADER     "Cal2HeaderForGeneration.txt"

#define FILENAME_EXTRACT_HDR_APP        "AppHeaderInfo.txt"
#define FILENAME_EXTRACT_HDR_CAL1       "Cal1HeaderInfo.txt"
#define FILENAME_EXTRACT_HDR_CAL2       "Cal2HeaderInfo.txt"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static bool copyFileToPath(QString sourceDir ,QString toDir, bool coverFileIfExist);
    bool loadFullFile(const QString &fileName, QString &outMsg);
    bool saveBinaryFiles(QString &outMsg);
    bool saveHexFiles(QString &outMsg);

public slots:
	void on_actionMergeFiles(QString &hexFileName, QString &binFileName, int type);
	void on_actionMergeHexHdrFiles(QString &hexFileName, QString &hdrFileName, int type);
	
private slots:
    void on_m_pbClose_clicked();
    void on_m_pbSaveHdr_clicked();
    void on_m_pbSaveBin_clicked();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionReLoad_triggered();
    void on_m_cbASLInfoSub0_currentIndexChanged(int index);
    void on_actionLoad_S19_File_triggered();
    void on_actionExtract_App_Header_triggered();
    void on_actionExtract_Cal1_Header_triggered();
    void on_actionExtract_Cal2_Header_triggered();
    void on_actionPreferences_triggered();

private:
    Ui::MainWindow *ui;
    QList<QLineEdit *> m_listAppDLS;
    QList<QLineEdit *> m_listAppHPN;
    QList<QLineEdit *> m_listAppBCID;
    QList<QLineEdit *> m_listAppNBID;
    QList<QLineEdit *> m_listAppASLInfo;
    QList<QLineEdit *> m_listAppASLInfoSub1;
    QList<QLineEdit *> m_listAppASLInfoSub2;
    QList<QLineEdit *> m_listCal1DLS;
    QList<QLineEdit *> m_listCal1HPN;
    QList<QLineEdit *> m_listCal1CCID;
    QList<QLineEdit *> m_listCal2DLS;
    QList<QLineEdit *> m_listCal2HPN;
    QList<QLineEdit *> m_listCal2CCID;
    AboutDialog *m_aboutDialog;
    ConfigDialog *m_configDialog;

    QMap<QString, QString> m_parameters;
    HeaderFile *m_pAppHeaderFile;
    HeaderFile *m_pCal1HeaderFile;
    HeaderFile *m_pCal2HeaderFile;
    SrecFile *m_pFullSrecordFile;
    SrecFile *m_pAppSrecordFile;
    SrecFile *m_pCal1SrecordFile;
    SrecFile *m_pCal2SrecordFile;
    BinFile *m_pAppBinFile;
    BinFile *m_pCal1BinFile;
    BinFile *m_pCal2BinFile;
    QByteArray m_baAppInfo, m_baCal1Info, m_baCal2Info;
	QByteArray m_baAppInfoPadding, m_baCal1InfoPadding, m_baCal2InfoPadding;
    QByteArray m_baAppBlock, m_baCal1Block, m_baCal2Block;
    QStandardItemModel* m_model;

    int loadS19File(SrecFile *pSrecordFile);
    int loadAppHeaderFile();
    int loadCal1HeaderFile();
    int loadCal2HeaderFile();
    void updateAppHeaderOnGui();
    void updateCal1HeaderOnGui();
    void updateCal2HeaderOnGui();
    void updateHeadersOnGui();
    int updateAppHeaderFile();
    int updateCal1HeaderFile();
    int updateCal2HeaderFile();
    void messageBoxAlert(QString s);
    void updateTableView();
    void readConfigSettings();
    int updateIntegrityWordAsChecksum(QByteArray &ba, quint32 chksumOffset, 
        quint32 startOffset, quint32 computeLen);
};

#endif // MAINWINDOW_H
