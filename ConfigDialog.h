#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QMap<QString, QString> *pParameters, QWidget *parent = 0);
    ~ConfigDialog();

private slots:
    void on_m_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_buttonBox_accepted();

private:
    Ui::ConfigDialog *ui;
    QMap<QString, QString> *m_pParameters;

	QString getParameter(QString name);
	void loadParameters();
	void saveConfigParameters();
};

#endif // CONFIGDIALOG_H
