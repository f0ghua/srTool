#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "SrecFile.h"
#include "AppHeaderFile.h"

#define FILENAME_APP_SIGNED_HEADER      "App_Signed_Header.txt"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void pteASLInfoLineAction();
    void pteSignerInfoLineAction();
    void pteSignatureLineAction();
    void on_actionLoad_File_triggered();
    void on_m_pbClose_clicked();
    void on_m_pbSaveHdr_clicked();
    void on_m_pbSaveBin_clicked();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    AppHeaderFile *m_pAppHeaderFile;
    SrecFile *m_pSrecordFile;
    QList<QLineEdit *> m_listModuleId;
    QList<QLineEdit *> m_listBcId;
    QList<QLineEdit *> m_listEcuName;
    QList<QLineEdit *> m_listEcuId;
    QList<QLineEdit *> m_listApNbid;
    QList<QLineEdit *> m_listMsgDig;

    int getLineNumberByCursor(QPlainTextEdit *pte);
    void saveAppHeaderFile();
    void saveBinaryFile();
};

#endif // MAINWINDOW_H
