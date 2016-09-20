#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "SrecFile.h"
#include "HeaderFile.h"
#include "BinFile.h"

#define FILENAME_APP_SIGNED_HEADER      "App_Signed_Header.txt"
#define FILENAME_CAL1_SIGNED_HEADER     "Cal1_Signed_Header.txt"
#define FILENAME_CAL2_SIGNED_HEADER     "Cal2_Signed_Header.txt"

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

private slots:
    void on_m_pbClose_clicked();
    void on_m_pbSaveHdr_clicked();
    void on_m_pbSaveBin_clicked();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionReLoad_triggered();
    void on_m_cbASLInfoSub0_currentIndexChanged(int index);
    void on_actionLoad_Cal1_Srec_File_triggered();
    void on_actionLoad_Cal2_Srec_File_triggered();
    void on_actionLoad_App_Srec_File_triggered();

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

    HeaderFile *m_pAppHeaderFile;
    HeaderFile *m_pCal1HeaderFile;
    HeaderFile *m_pCal2HeaderFile;
    SrecFile *m_pAppSrecordFile;
    SrecFile *m_pCal1SrecordFile;
    SrecFile *m_pCal2SrecordFile;
    BinFile *m_pAppBinFile;
    BinFile *m_pCal1BinFile;
    BinFile *m_pCal2BinFile;

    QStandardItemModel* m_model;

    int loadS19File(SrecFile *pSrecordFile);
    void saveBinaryFiles();
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
};

#endif // MAINWINDOW_H
