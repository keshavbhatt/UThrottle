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
    //enable or disable buttons descision
    enabledisableButtons();
    //disable the downlink and uplink frames , we are enabling them when user enable limits by checkboxs
    ui->uplinkFrame->setEnabled(false); ui->downlinkFrame->setEnabled(false);
    //disable stop button at startup
    ui->stopThrottleBtn->setEnabled(false);
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
    if(ui->interfaceTitle->text()=="25K"){
       ui->downloadlimit->setReadOnly(true);
       ui->uploadlimit->setReadOnly(true);
       ui->downloadlimit->setText("25");
       ui->uploadlimit->setText("25");
        }
    else if(ui->interfaceTitle->text()=="56K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("56");
        ui->uploadlimit->setText("56");
    }
    else if(ui->interfaceTitle->text()=="80K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("80");
        ui->uploadlimit->setText("80");
    }
    else if(ui->interfaceTitle->text()=="100K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("100");
        ui->uploadlimit->setText("100");
    }
    else if(ui->interfaceTitle->text()=="120K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("120");
        ui->uploadlimit->setText("120");
    }
    else if(ui->interfaceTitle->text()=="150K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("150");
        ui->uploadlimit->setText("150");
    }
    else if(ui->interfaceTitle->text()=="200K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("200");
        ui->uploadlimit->setText("200");
    }
    else if(ui->interfaceTitle->text()=="Edge"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("250");
        ui->uploadlimit->setText("250");
    }
    else if(ui->interfaceTitle->text()=="3G"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("750");
        ui->uploadlimit->setText("750");
    }
    else if(ui->interfaceTitle->text()=="LTE / 4G"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("1024");
        ui->uploadlimit->setText("1024");
    }
    else if(ui->interfaceTitle->text()=="DSL"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("2024");
        ui->uploadlimit->setText("2024");
    }
    else if(ui->interfaceTitle->text()=="Custom"){
        //seting int validiator
        ui->downloadlimit->setValidator( new QIntValidator(0, 1000000, this) );
        ui->uploadlimit->setValidator( new QIntValidator(0, 1000000, this) );
        //readonly false allow user input custom values
        ui->downloadlimit->setReadOnly(false);
        ui->uploadlimit->setReadOnly(false);
        //setting value to blank
        ui->downloadlimit->setText("");
        ui->uploadlimit->setText("");
    }
    //enable or disable buttons descision
    enabledisableButtons();

}

//to enable/disble buttons
void MainWindow::enabledisableButtons(){

    //when no preset is selected and no checkbox is checked
    if(((ui->interfaceTitle->text().count()==0) && ((ui->limitDownloadCheckBox->checkState()== Qt::Unchecked)||(ui->limitUploadCheckBox->checkState()== Qt::Unchecked)))&&((ui->downloadlimit->text().count()<1)||(ui->uploadlimit->text().count()<1))){
        //disable all buttons
        ui->startThrottleBtn->setEnabled(false);
      //  ui->stopThrottleBtn->setEnabled(false);
    }
    //when preset is selected and one of checkbox is checked
    else if(((ui->interfaceTitle->text().count()>0) &&((ui->limitDownloadCheckBox->checkState()==Qt::Checked)||(ui->limitUploadCheckBox->checkState()==Qt::Checked)))&&((ui->downloadlimit->text().count()>0)||(ui->uploadlimit->text().count()>0))) {
        //enable buttons
        ui->startThrottleBtn->setEnabled(true); // qDebug()<<"da,m";
      //  ui->stopThrottleBtn->setEnabled(true);

    }
    //when both the checkbox are unchecked
    else if((ui->limitDownloadCheckBox->checkState()==Qt::Unchecked)&&(ui->limitUploadCheckBox->checkState()==Qt::Unchecked)){
        //disable all buttons
        ui->startThrottleBtn->setEnabled(false);
      //  ui->stopThrottleBtn->setEnabled(false);
    }

}

//when checkbox states changed-----------------------------------/* */
void MainWindow::on_limitDownloadCheckBox_stateChanged(int arg1) /* */
{                                                                /* */
    if(arg1==2){ui->downlinkFrame->setEnabled(true);}            /* */
    else if(arg1==0){ui->downlinkFrame->setEnabled(false);}      /* */
    enabledisableButtons();                                      /* */
                                                                 /* */
}                                                                /* */
                                                                 /* */
void MainWindow::on_limitUploadCheckBox_stateChanged(int arg1)   /* */
{                                                                /* */
    if(arg1==2){ui->uplinkFrame->setEnabled(true);}              /* */
    else if(arg1==0){ui->uplinkFrame->setEnabled(false);}        /* */
    enabledisableButtons();                                      /* */
                                                                 /* */
}                                                                /* */
//when checkbox states changed-----------------------------------/* */


// Start/Stop clicked
void MainWindow::on_startThrottleBtn_clicked()
{
    //start app engine
    startThrottler();
    //disable the modifiers
    disableGUI();
}
void MainWindow::on_stopThrottleBtn_clicked()
{
    //stop app engine
    enableGUI();
}

//on custom preset download/Uplimit text chnaged--------------------/* */
void MainWindow::on_downloadlimit_textChanged(const QString &arg1)  /* */
{                                                                   /* */
       enabledisableButtons();                                      /* */
       if(arg1.count()<1){                                          /* */
           ui->startThrottleBtn->setEnabled(false);                 /* */
       }                                                            /* */
}                                                                   /* */
void MainWindow::on_uploadlimit_textChanged(const QString &arg1)    /* */
{                                                                   /* */
    enabledisableButtons();                                         /* */
    if(arg1.count()<1){                                             /* */
        ui->startThrottleBtn->setEnabled(false);                    /* */
    }                                                               /* */
}                                                                   /* */
//------------------------------------------------------------------/* */


void MainWindow::disableGUI(){
    ui->mainFrame->setEnabled(false);
    ui->treeWidget->setEnabled(false);
    ui->tabWidget->setEnabled(false);
    ui->startThrottleBtn->setEnabled(false);
}
void MainWindow::enableGUI(){
    ui->mainFrame->setEnabled(true);
    ui->treeWidget->setEnabled(true);
    ui->tabWidget->setEnabled(true);
    ui->startThrottleBtn->setEnabled(true);
}


//core processes
void MainWindow::startThrottler(){

}
