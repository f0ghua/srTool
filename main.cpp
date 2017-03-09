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
    QCommandLineOption ptpFileOption(QStringList() << "f" << "ptpfile",
            QCoreApplication::translate("main", "indicate ptp file which include all data"),
            QCoreApplication::translate("main", "ptpfile"));
    QCommandLineOption appFileOption(QStringList() << "a" << "appbinfile",
            QCoreApplication::translate("main", "indicate application binary(.bin)/header(.txt) file"),
            QCoreApplication::translate("main", "appbinfile"));
    QCommandLineOption calFileOption(QStringList() << "c" << "calbinfile",
            QCoreApplication::translate("main", "indicate calibration binary(.bin)/header(.txt) file"),
            QCoreApplication::translate("main", "calbinfile"));
    QCommandLineOption hexFileOption(QStringList() << "x" << "hexfile",
            QCoreApplication::translate("main", "indicate intel hex format file"),
            QCoreApplication::translate("main", "hexfile"));
            
    QCommandLineOption spliteFileOption(QStringList() << "s" << "split",
            QCoreApplication::translate("main", "split ptp file to hex and bin files"));
    QCommandLineOption mergeFileOption(QStringList() << "m" << "merge",
            QCoreApplication::translate("main", "merge hex file content with header extract from bin/header file"));

    parser.addOption(ptpFileOption);
    parser.addOption(hexFileOption);
    parser.addOption(appFileOption);
    parser.addOption(calFileOption);
    
    parser.addOption(spliteFileOption);
    parser.addOption(mergeFileOption);
    parser.process(a);
    
    const QStringList ptpFileNameList = parser.values(ptpFileOption);
    const QStringList hexFileNameList = parser.values(hexFileOption);
    const QStringList appFileNameList = parser.values(appFileOption);
	const QStringList calFileNameList = parser.values(calFileOption);
	QString ptpFileName, hexFileName;
	QString appBinFileName, calBinFileName;
	QString appHdrFileName, calHdrFileName;
		
	if (!ptpFileNameList.isEmpty())
    	ptpFileName = ptpFileNameList.at(0);
	if (!hexFileNameList.isEmpty())
    	hexFileName = hexFileNameList.at(0);

	if (!appFileNameList.isEmpty()) {
		QFileInfo fi(appFileNameList.at(0));
		QString ext = fi.suffix();
		if (ext.toLower() == "bin")
    		appBinFileName = appFileNameList.at(0);
    	else if (ext.toLower() == "txt")
    		appHdrFileName = appFileNameList.at(0);
	}

	if (!calFileNameList.isEmpty()) {
		QFileInfo fi(calFileNameList.at(0));
		QString ext = fi.suffix();
		if (ext.toLower() == "bin")
    		calBinFileName = calFileNameList.at(0);
    	else if (ext.toLower() == "txt")
    		calHdrFileName = calFileNameList.at(0);
	}
	
	if (parser.isSet(spliteFileOption)) {
		QString errMsg;
		if (ptpFileName.isEmpty() || (!QFile::exists(ptpFileName))) {
			qWarning() << "The ptp file has not been assigned or not exist.";
			return -1;
		}

    	if (!w.loadFullFile(ptpFileName, errMsg)) {
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

	} else if (parser.isSet(mergeFileOption)) {
		if (hexFileName.isEmpty() || (!QFile::exists(hexFileName))) {
			qWarning() << "The ptp file has not been assigned or not exist.";
			return -1;
		}

		if (!appHdrFileName.isEmpty() && QFile::exists(appHdrFileName)) {
			w.on_actionMergeHexHdrFiles(hexFileName, appHdrFileName, HDRFILE_TYPE_APP);
			return 0;
		}

		if (!calHdrFileName.isEmpty() && QFile::exists(calHdrFileName)) {
			w.on_actionMergeHexHdrFiles(hexFileName, calHdrFileName, HDRFILE_TYPE_CAL);
			return 0;
		}

		if (!appBinFileName.isEmpty() && QFile::exists(appBinFileName)) {
			w.on_actionMergeFiles(hexFileName, appBinFileName, HDRFILE_TYPE_APP);
			return 0;
		}

		if (!calBinFileName.isEmpty() && QFile::exists(calBinFileName)) {
			w.on_actionMergeFiles(hexFileName, calBinFileName, HDRFILE_TYPE_CAL);
			return 0;
		}
	} else {
		qWarning() << "You must set -s or -m for function use";
		return -1;
	}
	
    return 0;
}
