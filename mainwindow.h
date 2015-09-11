#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <qdebug.h>
#include <QModelIndex>  //when treewidget item changed

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString inputstring();

private slots:
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void refreshinterfaceCombo();
    void populateCombo();


    void on_refreshInterfaceBtn_clicked();

    void on_treeWidget_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QProcess *interfaceInfo;
};

#endif // MAINWINDOW_H
