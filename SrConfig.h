#ifndef SRCONFIG_H
#define SRCONFIG_H

#include <QMap>
#include <QSettings>

#define CFGPARAM_LAYOUT_START   0
#define CFGPARAM_LAYOUT_END     11
#define CFGPARAM_SETTING_START  12
#define CFGPARAM_SETTING_END    12

//extern const QPair<QString, QString> g_configParamsList[];

class SrConfig
{
public:
    SrConfig();

    static void readConfigSettings(QMap<QString, QString> &parameters);
    static bool validConfigSettings(QMap<QString, QString> &parameters);
    static void writeConfigSettings(QMap<QString, QString> &parameters);
};

#endif // SRCONFIG_H