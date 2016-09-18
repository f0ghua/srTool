#include "mainwindow.h"
#include <QApplication>
#include <QFont>

static int mainConsole(QApplication &a, int argc, char *argv[]);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (argc > 1)
    {
        mainConsole(a, argc, argv);
        return 0;
    }

    const QRect ag = QApplication::desktop()->availableGeometry();
    if (ag.width() > 1024) {
    	QFont font = a.font();
    	font.setPointSize(10);
    	a.setFont(font);
    }

    // enable High DPI Support in Qt (seems not work)
    //QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    MainWindow w;
    w.show();

    return a.exec();
}


#include <QTextStream>
#include <QCommandLineParser>
static int mainConsole(QApplication &a, int argc, char *argv[])
{
	QTextStream out(stdout);

	QCommandLineOption inOp1(QStringList() << "t" << "header", "Header file with txt format", "file");
	QCommandLineOption inOp2(QStringList() << "s" << "s19", "S19 file with ascii hex format", "file");
	QCommandLineOption outOp(QStringList() << "o" << "output", "Write generated binary data into <file>.", "file");

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addOption(inOp1);
	parser.addOption(inOp2);
	parser.addOption(outOp);

	parser.process(a);

	qDebug() << parser.isSet(inOp1);
	qDebug() << parser.value(inOp1);
	qDebug() << parser.isSet(inOp2);
	qDebug() << parser.value(inOp2);
	qDebug() << parser.isSet(outOp);
	qDebug() << parser.value(outOp);

	return 0;
}