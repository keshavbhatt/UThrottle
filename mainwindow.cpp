#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
//for app gui
#include <QApplication>
#include <QDesktopWidget>
//for process handling and debuggings
#include <QDebug>
#include <QProcess>
//to get the list populate from text file
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QDesktopServices>
#include <QComboBox>


//setting ui
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center()); // moving to center of desktop
    //populating interfaceCombobox
    refreshinterfaceCombo();
}

//on app exit
MainWindow::~MainWindow()
{
    delete ui;
}

//quit app with action
void MainWindow::on_actionQuit_triggered()
{
   qApp->quit();
}

//about action/dialog
void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutDialog;
    //Set location of player in center of display
    aboutDialog.move(QApplication::desktop()->screen()->rect().center() - aboutDialog.rect().center());
    //  aboutDialog.setWindowFlags(Qt::Popup);
    aboutDialog.exec();

}

void MainWindow::refreshinterfaceCombo()  // we will grep output via bash script and then will write the _interfacef.txt and load it to the interfaceCombo
{
    // Get interface info with    "interfacelist" << a bash script with  this content >>   netstat -i | awk '! /(Kernel|Iface)/ {print $1}'
    QString program = "/usr/bin/interfacelist";
    interfaceInfo = new QProcess(this);
    interfaceInfo->start(program);
    interfaceInfo->waitForBytesWritten();
    interfaceInfo->waitForFinished();
    //  QObject::connect(interfaceInfo, SIGNAL(finished(int)), this, SLOT(populateCombo()));
    qDebug() << "running" << program;
    //qDebug() << interfaceInfo->readAllStandardOutput() ;  // please comment this if uncommented data will not get written into the file below

    //now we will write the output to a file to later read and populate the interfaceCombo
    //we will save the interface list to a file and save it to storagelocation via desktop services
    QString setting_path=QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QFile file(setting_path+"_interfaces.txt");
         if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
             return;
         QTextStream out(&file);
         out << interfaceInfo->readAllStandardOutput();
        // qDebug()<<setting_path;

    file.close();    // file needed to close first since we opening it again in our next function.
    populateCombo(); // calling slot to populate the interfaceCombo process finished() slot not working i dont know why.
}

void MainWindow::populateCombo() // we will poulate interfaceCombo when this slot is called
{
    QString setting_path=QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QStandardItemModel *model2 = new QStandardItemModel();
    QFile file(setting_path+"_interfaces.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;  // Open File Using A Stream Reader Object
    QTextStream in(&file);
    while (!in.atEnd()) {     // Read Each Line Until End Of File
      QString line = in.readLine();
      QList<QStandardItem *> items;
      QStringList fields = line.split(">");  // Read Line (Trimming Whitespace)
      QStringList fields3 = fields.filter("");  // Read Line (Trimming Whitespace)
      foreach (QString text, fields3) {
        items.append(new QStandardItem((QString)text));
      }
      if (items.length() > 0) {  // Add It To model If It Is Not Empty
        model2->appendRow(items);
      }
    }
    ui->interfaceCombo->setModel(model2); //setting model of qcombobox
    file.close();
}


//refresh the interfaces list // will rewrite the _interfacef.txt and reload the interfaceCombo
void MainWindow::on_refreshInterfaceBtn_clicked()
{
refreshinterfaceCombo();
}

//to return the string name of item choosed in treewidget
QString MainWindow::inputstring() {
    QModelIndexList templatelist =
    ui->treeWidget->selectionModel()->selectedIndexes();
    QString  stringlist;
    foreach (const QModelIndex &index, templatelist) {
    stringlist.append(index.data(Qt::DisplayRole).toString());
   }
    return stringlist;
}

//to change the data according to selected item in treeWidget
void MainWindow::on_treeWidget_clicked(const QModelIndex &index)
{
    ui->interfaceTitle->setText(inputstring());
    if(ui->interfaceTitle->text()=="56K"){
        ui->downloadlimit->setText("56");
        ui->uploadlimit->setText("56");
    }
    else if(ui->interfaceTitle->text()=="Edge"){
        ui->downloadlimit->setText("250");
        ui->uploadlimit->setText("250");
    }
    else if(ui->interfaceTitle->text()=="3G"){
        ui->downloadlimit->setText("750");
        ui->uploadlimit->setText("750");
    }
    else if(ui->interfaceTitle->text()=="LTE / 4G"){
        ui->downloadlimit->setText("1024");
        ui->uploadlimit->setText("1024");
    }
    else if(ui->interfaceTitle->text()=="DSL"){
        ui->downloadlimit->setText("2024");
        ui->uploadlimit->setText("2024");
    }
}
