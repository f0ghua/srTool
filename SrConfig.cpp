#include "SrConfig.h"

#include <QDebug>

const QPair<QString, QString> g_configParamsList[] = {
    {"ADDR_APPL_SWINFO", "0x00010000"},
    {"SIZE_APPL_SWINFO", "0x200"},
    {"ADDR_APPL_BLOCK", "0x00010200"},
    {"SIZE_APPL_BLOCK", "0xCFDF8"},
    {"ADDR_CAL1_DATAINFO", "0x00008000"},
    {"SIZE_CAL1_DATAINFO", "0x20"},
    {"ADDR_CAL1_BLOCK", "0x00008020"},
    {"SIZE_CAL1_BLOCK", "0x3FD8"},
    {"ADDR_CAL2_DATAINFO", "0x00000000"},
    {"SIZE_CAL2_DATAINFO", "0x20"},
    {"ADDR_CAL2_BLOCK", "0x00000020"},
    {"SIZE_CAL2_BLOCK", "0x7FD8"},
    {"HEX_HEADER_ENABLE", "0x00"}
};

SrConfig::SrConfig()
{

}

void SrConfig::readConfigSettings(QMap<QString, QString> &parameters)
{
    QSettings *cfg = new QSettings("config.ini", QSettings::IniFormat);
    cfg->setIniCodec("UTF-8");

    for (int i = CFGPARAM_LAYOUT_START; i <= CFGPARAM_LAYOUT_END; i++) {
        QString key = "Layout/" + g_configParamsList[i].first;
#ifndef F_NO_DEBUG
    qDebug() << "process " << key;
#endif
        parameters[g_configParamsList[i].first] =
            cfg->value(key, g_configParamsList[i].second).toString();
    }

    for (int i = CFGPARAM_SETTING_START; i <= CFGPARAM_SETTING_END; i++) {
        QString key = "Setting/" + g_configParamsList[i].first;
#ifndef F_NO_DEBUG
		qDebug() << "process " << key;
#endif
        parameters[g_configParamsList[i].first] =
            cfg->value(key, g_configParamsList[i].second).toString();
#ifndef F_NO_DEBUG
        qDebug() << g_configParamsList[i].first << ":" << parameters[g_configParamsList[i].first];
#endif
    }
}

bool SrConfig::validConfigSettings(QMap<QString, QString> &parameters)
{
	bool ok;

    for (int i = 0;
        i < sizeof(g_configParamsList)/sizeof(g_configParamsList[0]);
        i++) {
        bool ok;
        quint32 v = parameters[g_configParamsList[i].first].toULong(&ok, 16);
        if (!ok) {
            return false;
        }
    }

    return true;
}

void SrConfig::writeConfigSettings(QMap<QString, QString> &parameters)
{
    QSettings *cfg = new QSettings("config.ini", QSettings::IniFormat);
    cfg->setIniCodec("UTF-8");

    for (int i = CFGPARAM_LAYOUT_START; i <= CFGPARAM_LAYOUT_END; i++) {
        QString key = "Layout/" + g_configParamsList[i].first;
        cfg->setValue(key, parameters[g_configParamsList[i].first]);
    }

    for (int i = CFGPARAM_SETTING_START; i <= CFGPARAM_SETTING_END; i++) {
        QString key = "Setting/" + g_configParamsList[i].first;
        cfg->setValue(key, parameters[g_configParamsList[i].first]);
    }
}
