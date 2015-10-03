/*
*This file is part of UThrottle.
*
*UThrottle is not a free software: you can't redistribute it nor modify
*it without the author <keshavnrj@gmail.com> Keshav Bhatt.
*
*UThrottle is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*Debian distro dependencies - wondershaper .
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <qdebug.h>
#include <QModelIndex>  //when treewidget item changed
//tray
#include <QSystemTrayIcon>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setVisible(bool visible);
    QString inputstring();

    void on_checkbox_action_triggered_from_downloadCheckbox();
    void on_checkbox_action_triggered_from_uplinkCheckbox();


private slots:
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void refreshinterfaceCombo();
    void populateCombo();

    void on_refreshInterfaceBtn_clicked();

    void on_treeWidget_clicked();

    void enabledisableButtons();

    void on_limitDownloadCheckBox_stateChanged(int arg1);

    void on_limitUploadCheckBox_stateChanged(int arg1);

    void on_startThrottleBtn_clicked();

    void on_stopThrottleBtn_clicked();

    void on_downloadlimit_textChanged(const QString &arg1);

    void on_uploadlimit_textChanged(const QString &arg1);

    void enableGUI();

    void disableGUI();

    void on_check();

    void startThrottler();
    void stopThrottler();
   // void printOutput();
    void printOutputInTerminal1();

    void on_interfaceCombo_currentIndexChanged(int index);

    void on_showterminal_clicked(bool checked);

    void on_actionRuninBackground_triggered();

    void check_throttling_interface(); //test
    void printthestatus();//test
    void finished_check_change_index(); //test

    void on_pushButton_clicked();



protected:
    //tray
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QProcess *interfaceInfo;
    //tray
    void createActions();
    void createTrayIcon();
    QAction *minimizeAction;
    QAction *runinbackground;
//  QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    //main Process
    QProcess *throttler;
    QProcess *stopthrottler;
    QProcess *status;


};

#endif // MAINWINDOW_H
