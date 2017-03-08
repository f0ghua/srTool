#include "mainwindow.h"
#include <QApplication>
#include <QFont>
#include <QMutex>

double g_dpiScaleValue = 1;

static int mainConsole(QApplication &a, MainWindow &w, int argc, char *argv[]);

/**
 * Function registed for log to file with qDebug/qWarning/qCritical/qFatal
 * @param type    [description]
 * @param context [description]
 * @param msg     [description]
 */
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	Q_UNUSED(context);
	
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
        case QtInfoMsg:
            text = QString("Info:");
            break;

        case QtDebugMsg:
            text = QString("Debug:");
            break;

        case QtWarningMsg:
            text = QString("Warning:");
            break;

        case QtCriticalMsg:
            text = QString("Critical:");
            break;

        case QtFatalMsg:
            text = QString("Fatal:");
            break;
    }

    //QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    //QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    //QString current_date = QString("(%1)").arg(current_date_time);
    //QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString message = QString("[%1] %2 %3").arg(current_date_time).arg(text).arg(msg);

    QFile file("log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message.toUtf8().constData() << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

int main(int argc, char *argv[])
{
#if QT_VERSION >= QT_VERSION_CHECK(5,6,0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif // QT_VERSION

    QApplication a(argc, argv);
    qInstallMessageHandler(outputMessage);

    QScreen *screen = a.primaryScreen();
    g_dpiScaleValue = (double)screen->logicalDotsPerInch()/96;
    QFont font = a.font();
    font.setPointSize(9*g_dpiScaleValue);

    MainWindow w;

    if (argc > 1)
    {
        mainConsole(a, w, argc, argv);
        return 0;
    }

    w.show();

    return a.exec();
}


#include <QTextStream>
#include <QFile>
#include <QByteArray>

static int mainConsole(QApplication &a, MainWindow &w, int argc, char *argv[])
{
	Q_UNUSED(argc);
	Q_UNUSED(argv);
	
    QCoreApplication::setApplicationName("srTool");
    QCoreApplication::setApplicationVersion("1.2.05");
    
    QCommandLineParser parser;
    parser.setApplicationDescription("Motorola S19 Split Tool");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption splitePtpFileOption(QStringList() << "f" << "ptpfile",
            QCoreApplication::translate("main", "indicate <ptpfile> to split to hex and bin files."),
            QCoreApplication::translate("main", "ptpfile"));
    QCommandLineOption appBinFileOption(QStringList() << "a" << "appbinfile",
            QCoreApplication::translate("main", "indicate <app binfile> to be merged"),
            QCoreApplication::translate("main", "appbinfile"));
    QCommandLineOption calBinFileOption(QStringList() << "c" << "calbinfile",
            QCoreApplication::translate("main", "indicate <cal binfile> to be merged"),
            QCoreApplication::translate("main", "calbinfile"));
    QCommandLineOption hexFileOption(QStringList() << "x" << "hexfile",
            QCoreApplication::translate("main", "indicate <hexfile> to be merged"),
            QCoreApplication::translate("main", "hexfile"));

    parser.addOption(splitePtpFileOption);
    parser.addOption(hexFileOption);
    parser.addOption(appBinFileOption);
    parser.addOption(calBinFileOption);
    parser.process(a);
    
    QStringList ptpFileNameList = parser.values(splitePtpFileOption);
    QStringList hexFileNameList = parser.values(hexFileOption);
    QStringList appBinFileNameList = parser.values(appBinFileOption);
	QStringList calBinFileNameList = parser.values(calBinFileOption);
	
	if (!ptpFileNameList.isEmpty()) {
    	QString fullFileName = ptpFileNameList.at(0);
    	QString errMsg;

    	if (fullFileName.isEmpty() || (!QFile::exists(fullFileName)))
        	return -1;

    	if (!w.loadFullFile(fullFileName, errMsg)) {
        	qWarning() << errMsg;
        	return -1;
    	}
    	if (!w.saveBinaryFiles(errMsg)) {
        	qWarning() << errMsg;
        	return -1;
    	}
    	if (!w.saveHexFiles(errMsg)) {
        	qWarning() << errMsg;
        	return -1;
    	}

		return 0;
	}


	if (!hexFileNameList.isEmpty()) {
		QString hexFileName = hexFileNameList.at(0);
		if (!QFile::exists(hexFileName)) {
			qWarning() << QObject::tr("hex file %1 doesn't exist").arg(hexFileName);
			return -1;
		}

		if (!appBinFileNameList.isEmpty()) {
			QString appBinFileName = appBinFileNameList.at(0);
		
			if (!QFile::exists(appBinFileName)) {
				qWarning() << QObject::tr("bin file %1 doesn't exist").arg(appBinFileName);
				return -1;
			}

			w.on_actionMergeFiles(hexFileName, appBinFileName, HDRFILE_TYPE_APP);

		} else if (!calBinFileNameList.isEmpty()) {
			QString calBinFileName = calBinFileNameList.at(0);

			if (!QFile::exists(calBinFileName)) {
				qWarning() << QObject::tr("bin file %1 doesn't exist").arg(calBinFileName);
				return -1;
			}
		
			w.on_actionMergeFiles(hexFileName, calBinFileName, HDRFILE_TYPE_CAL);
		}
	}
	
    return 0;
}
