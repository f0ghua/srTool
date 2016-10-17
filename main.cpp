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
    QString fullFileName = argv[1];
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
