#include "widget.h"
#include <QApplication>
#include <QDebug>
#include <QSplashScreen>
#include <QThread>

//#include <QTest>
//#include "unit_test.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSplashScreen splash(QPixmap(":/icon/images/Headphones.png"));
    splash.show();
    Widget w;
    QThread::currentThread()->sleep(1);
    splash.finish(&w);
    w.show();
    //QTest::qExec(new unit_test, argc, argv);
    return a.exec();
}
