#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
     a.setWindowIcon(QIcon("qrc:///icons180.png"));
     QApplication::setApplicationName("Uthrottle");
     QApplication::setApplicationVersion("1.0");
//     QApplication::setOrganizationName("org.keshavnrjUthrottle.ubuntu");  // do not add when using desktopservices
//    QApplication::setApplicationName("Uthrottle");

    MainWindow w;
    w.show();

    return a.exec();
}
