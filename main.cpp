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
#include <QFile>
#include <QByteArray>

#include "HeaderFile.h"
#include "SrecFile.h"

static int mainConsole(QApplication &a, int argc, char *argv[])
{
	QTextStream out(stdout);
	QByteArray ba;

	QCommandLineOption inOp1(QStringList() << "t" << "header", "Header file with txt format", "file");
	QCommandLineOption inOp2(QStringList() << "s" << "s19", "S19 file with ascii hex format", "file");
	QCommandLineOption outOp(QStringList() << "o" << "output", "Write generated binary data into <file>.", "file");

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addOption(inOp1);
	parser.addOption(inOp2);
	parser.addOption(outOp);

	parser.process(a);

#ifndef F_NO_DEBUG
	qDebug() << parser.isSet(inOp1);
	qDebug() << parser.value(inOp1);
	qDebug() << parser.isSet(inOp2);
	qDebug() << parser.value(inOp2);
	qDebug() << parser.isSet(outOp);
	qDebug() << parser.value(outOp);
#endif

	QString fileNameHeader = parser.value(inOp1);
	QString fileNameS19 = parser.value(inOp2);

	if (fileNameHeader.isEmpty() || fileNameS19.isEmpty())
		return -1;

	if (!QFile::exists(fileNameHeader) || !QFile::exists(fileNameS19))
		return -1;

	HeaderFile *pHeaderFile = new HeaderFile();
    if (pHeaderFile->load(fileNameHeader) == -1) {
    	return -1;
    }

	SrecFile *pSrecFile = new SrecFile();
    if (pSrecFile->load(fileNameS19) == -1) {
    	delete pHeaderFile;
    	return -1;
    }

    quint64 v;
    if ((v = pHeaderFile->getHexPartNumber()) == -1)
    {
    	delete pHeaderFile;
    	delete pSrecFile;
    	return -1;
    }

    QString fileNameBin = QString::number(v) + ".bin";
    QString fileNameHex = QString::number(v) + ".hex";

    QFile *outFile = new QFile(fileNameBin);
    if (!outFile->open(QIODevice::WriteOnly))
    {
    	delete pHeaderFile;
    	delete pSrecFile;
        delete outFile;
        return -1;
    }

    ba = pHeaderFile->getBinDataWithOutCheck();
    outFile->write(ba);

    for (auto d : pSrecFile->m_dataRecords)
    {
        QByteArray &rBa = d.binData;
        outFile->write(rBa);
    }

    outFile->close();
    delete outFile;

    MainWindow::copyFileToPath(pSrecFile->m_fileFullPath, fileNameHex, true);

	return 0;
}

