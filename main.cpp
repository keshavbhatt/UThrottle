/*
*This file is part of UThrottle.
*
*UThrottle is not a free software: you can't redistribute it nor modify
*it without the author <keshavnrj@gmail.com> Keshav Bhatt.
*
*UThrottle is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //systemtray shit
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
     QApplication::setQuitOnLastWindowClosed(false);

   //  a.setWindowIcon(QIcon("qrc:///icons180.png"));
     QApplication::setApplicationName("Uthrottle");
     QApplication::setApplicationVersion("1.0");
//     QApplication::setOrganizationName("org.keshavnrjUthrottle.ubuntu");  // do not add when using desktopservices
//    QApplication::setApplicationName("Uthrottle");

    MainWindow w;
    w.show();

    return a.exec();
}
