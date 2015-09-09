#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include <QApplication>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
   qApp->quit();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutDialog;
    //Set location of player in center of display
    aboutDialog.move(QApplication::desktop()->screen()->rect().center() - aboutDialog.rect().center());
  //  aboutDialog.setWindowFlags(Qt::Popup);
    aboutDialog.exec();

}


