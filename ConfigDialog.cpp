#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"
#include "SrConfig.h"

#include <QSettings>

ConfigDialog::ConfigDialog(QMap<QString, QString> *pParameters, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    m_pParameters = pParameters;

    loadParameters();
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

QString ConfigDialog::getParameter(QString name)
{
	QMap<QString, QString>::iterator it;
	if ((it = m_pParameters->find(name)) != m_pParameters->end()) {
		return it.value();
	}

	return QString();
}

void ConfigDialog::loadParameters()
{
	ui->m_leAppInfoAddr->setText(getParameter("ADDR_APPL_SWINFO"));
	ui->m_leAppInfoSize->setText(getParameter("SIZE_APPL_SWINFO"));
	ui->m_leAppBlockAddr->setText(getParameter("ADDR_APPL_BLOCK"));
	ui->m_leAppBlockSize->setText(getParameter("SIZE_APPL_BLOCK"));

	ui->m_leCal1InfoAddr->setText(getParameter("ADDR_CAL1_DATAINFO"));
	ui->m_leCal1InfoSize->setText(getParameter("SIZE_CAL1_DATAINFO"));
	ui->m_leCal1BlockAddr->setText(getParameter("ADDR_CAL1_BLOCK"));
	ui->m_leCal1BlockSize->setText(getParameter("SIZE_CAL1_BLOCK"));

	ui->m_leCal2InfoAddr->setText(getParameter("ADDR_CAL2_DATAINFO"));
	ui->m_leCal2InfoSize->setText(getParameter("SIZE_CAL2_DATAINFO"));
	ui->m_leCal2BlockAddr->setText(getParameter("ADDR_CAL2_BLOCK"));
	ui->m_leCal2BlockSize->setText(getParameter("SIZE_CAL2_BLOCK"));

	bool ok;
	if ((!getParameter("HEX_HEADER_ENABLE").isEmpty()) &&
		getParameter("HEX_HEADER_ENABLE").toInt(&ok, 16)) {
		ui->m_ckbHexHeader->setCheckState(Qt::Checked);
	}
}

void ConfigDialog::on_m_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    ui->m_stackedWidget->setCurrentIndex(ui->m_listWidget->row(current));
}

void ConfigDialog::saveConfigParameters()
{
    QSettings *cfg = new QSettings("config.ini", QSettings::IniFormat);
    cfg->setIniCodec("UTF-8");

    (*m_pParameters)["ADDR_APPL_SWINFO"] = ui->m_leAppInfoAddr->text();
    (*m_pParameters)["SIZE_APPL_SWINFO"] = ui->m_leAppInfoSize->text();
    (*m_pParameters)["ADDR_APPL_BLOCK"] = ui->m_leAppBlockAddr->text();
    (*m_pParameters)["SIZE_APPL_BLOCK"] = ui->m_leAppBlockSize->text();

    (*m_pParameters)["ADDR_CAL1_DATAINFO"] = ui->m_leCal1InfoAddr->text();
    (*m_pParameters)["SIZE_CAL1_DATAINFO"] = ui->m_leCal1InfoSize->text();
    (*m_pParameters)["ADDR_CAL1_BLOCK"] = ui->m_leCal1BlockAddr->text();
    (*m_pParameters)["SIZE_CAL1_BLOCK"] = ui->m_leCal1BlockSize->text();    

    (*m_pParameters)["ADDR_CAL2_DATAINFO"] = ui->m_leCal2InfoAddr->text();
    (*m_pParameters)["SIZE_CAL2_DATAINFO"] = ui->m_leCal2InfoSize->text();
    (*m_pParameters)["ADDR_CAL2_BLOCK"] = ui->m_leCal2BlockAddr->text();
    (*m_pParameters)["SIZE_CAL2_BLOCK"] = ui->m_leCal2BlockSize->text();  

    (*m_pParameters)["HEX_HEADER_ENABLE"] = ui->m_ckbHexHeader->isChecked()?"1":"0";

    SrConfig::writeConfigSettings((*m_pParameters));
}

void ConfigDialog::on_buttonBox_accepted()
{
	QDialog::accept();
	saveConfigParameters();

}
