#include "AboutDialog.h"
#include "ui_AboutDialog.h"

#include <QDateTime>

#define APP_VERSION	"1.2.01"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->m_lblVersion->setText(QString("Version %1").arg(APP_VERSION));
    QString dateStr = QString("Release Date: ") + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->m_lblDate->setText(dateStr);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
