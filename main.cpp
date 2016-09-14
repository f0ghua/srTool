#include "mainwindow.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

/*
    const QRect ag = QApplication::desktop()->availableGeometry();
    if (ag.width() > 1024) {
    	QFont font = a.font();
    	font.setPointSize(10);
    	a.setFont(font);
    }
*/
    // enable High DPI Support in Qt
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    MainWindow w;
    w.show();

    return a.exec();
}
