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
//for tray icon and app handling via tray
#include <QSystemTrayIcon>
#include <QMessageBox>

//setting ui
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    setMaximumWidth(540);
//    setMaximumHeight(520);
//    setMinimumWidth(540);
//    setMinimumHeight(520);
    //hiding the fake combo used to get log
    ui->interfaceCombo2->hide();
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center()); // moving to center of desktop
    //create tray icon
    createActions();
    createTrayIcon();
    trayIcon->show();

    //populating interfaceCombobox
    refreshinterfaceCombo();
    //enable or disable buttons descision
    enabledisableButtons();
    //disable the downlink and uplink frames , we are enabling them when user enable limits by checkboxs
    ui->uplinkFrame->setEnabled(false); ui->downlinkFrame->setEnabled(false);
    //check current status
    on_check();
    //show terminal and seting checked
    ui->terminalFrame->show(); ui->showterminal->setChecked(true);
}

//on app exit
MainWindow::~MainWindow()
{
    delete ui;
}

//quit app with action
void MainWindow::on_actionQuit_triggered()
{
    stopThrottler();
    stopthrottler->waitForFinished();//wait otherwise stopThrottler will not open gksudo

    QString quitout;
    quitout= stopthrottler->readAll();
    if(quitout.count()==0){
        qDebug()<<"Please Enter sudo password to Stop Throttling" <<ui->interfaceCombo->currentText()<< "and Quit App.";
    }
    else{
        qDebug()<<"Throttling on Selected Interface Stopped ! , while other interfaces may be throttled by Uthrottle in background.";
//        throttler->kill();
//        stopthrottler->kill();
//        status->kill();
        qApp->quit();
    }
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
    ui->interfaceCombo2->setModel(model2);  // dummy combbox to check logs
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

//to change the data according to selected item/preset in treeWidget
void MainWindow::on_treeWidget_clicked(/*const QModelIndex &index*/)
{ //we can use index in each below but fuck it
    ui->interfaceTitle->setText(inputstring());

    if(ui->interfaceTitle->text()=="25K"){
       ui->downloadlimit->setReadOnly(true);
       ui->uploadlimit->setReadOnly(true);
       ui->downloadlimit->setText("25");
       ui->uploadlimit->setText("25");
       ui->limitDownloadCheckBox->setChecked(true);
       ui->limitUploadCheckBox->setChecked(true);
        }
    else if(ui->interfaceTitle->text()=="56K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("56");
        ui->uploadlimit->setText("56");
        ui->limitDownloadCheckBox->setChecked(true);
        ui->limitUploadCheckBox->setChecked(true);
    }
    else if(ui->interfaceTitle->text()=="80K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("80");
        ui->uploadlimit->setText("80");
        ui->limitDownloadCheckBox->setChecked(true);
        ui->limitUploadCheckBox->setChecked(true);
    }
    else if(ui->interfaceTitle->text()=="100K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("100");
        ui->uploadlimit->setText("100");
        ui->limitDownloadCheckBox->setChecked(true);
        ui->limitUploadCheckBox->setChecked(true);
    }
    else if(ui->interfaceTitle->text()=="120K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("120");
        ui->uploadlimit->setText("120");
        ui->limitDownloadCheckBox->setChecked(true);
        ui->limitUploadCheckBox->setChecked(true);
    }
    else if(ui->interfaceTitle->text()=="150K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("150");
        ui->uploadlimit->setText("150");
        ui->limitDownloadCheckBox->setChecked(true);
        ui->limitUploadCheckBox->setChecked(true);
    }
    else if(ui->interfaceTitle->text()=="200K"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("200");
        ui->uploadlimit->setText("200");
        ui->limitDownloadCheckBox->setChecked(true);
        ui->limitUploadCheckBox->setChecked(true);
    }
    else if(ui->interfaceTitle->text()=="Edge"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("250");
        ui->uploadlimit->setText("250");
        ui->limitDownloadCheckBox->setChecked(true);
        ui->limitUploadCheckBox->setChecked(true);
    }
    else if(ui->interfaceTitle->text()=="3G"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("750");
        ui->uploadlimit->setText("750");
        ui->limitDownloadCheckBox->setChecked(true);
        ui->limitUploadCheckBox->setChecked(true);
    }
    else if(ui->interfaceTitle->text()=="LTE / 4G"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("1024");
        ui->uploadlimit->setText("1024");
        ui->limitDownloadCheckBox->setChecked(true);
        ui->limitUploadCheckBox->setChecked(true);
    }
    else if(ui->interfaceTitle->text()=="DSL"){
        ui->downloadlimit->setReadOnly(true);
        ui->uploadlimit->setReadOnly(true);
        ui->downloadlimit->setText("2024");
        ui->uploadlimit->setText("2024");
        ui->limitDownloadCheckBox->setChecked(true);
        ui->limitUploadCheckBox->setChecked(true);
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
        ui->limitDownloadCheckBox->setChecked(true);
        ui->limitUploadCheckBox->setChecked(true);
    }
    //enable or disable buttons descision
    enabledisableButtons();

}

//START this is deep shit
//START this is deep shit
void MainWindow::on_checkbox_action_triggered_from_downloadCheckbox(/*const QModelIndex &index*/)
{ //we can use index in each below but fuck it
    ui->interfaceTitle->setText(inputstring());

    if(ui->interfaceTitle->text()=="25K"){
       ui->downloadlimit->setReadOnly(true);
       ui->downloadlimit->setText("25");
        }
    else if(ui->interfaceTitle->text()=="56K"){
        ui->downloadlimit->setReadOnly(true);
        ui->downloadlimit->setText("56");
    }
    else if(ui->interfaceTitle->text()=="80K"){
        ui->downloadlimit->setReadOnly(true);
        ui->downloadlimit->setText("80");
    }
    else if(ui->interfaceTitle->text()=="100K"){
        ui->downloadlimit->setReadOnly(true);
        ui->downloadlimit->setText("100");
    }
    else if(ui->interfaceTitle->text()=="120K"){
        ui->downloadlimit->setReadOnly(true);
        ui->downloadlimit->setText("120");
    }
    else if(ui->interfaceTitle->text()=="150K"){
        ui->downloadlimit->setReadOnly(true);
        ui->downloadlimit->setText("150");
    }
    else if(ui->interfaceTitle->text()=="200K"){
        ui->downloadlimit->setReadOnly(true);
        ui->downloadlimit->setText("200");
    }
    else if(ui->interfaceTitle->text()=="Edge"){
        ui->downloadlimit->setReadOnly(true);
        ui->downloadlimit->setText("250");
    }
    else if(ui->interfaceTitle->text()=="3G"){
        ui->downloadlimit->setReadOnly(true);
        ui->downloadlimit->setText("750");
    }
    else if(ui->interfaceTitle->text()=="LTE / 4G"){
        ui->downloadlimit->setReadOnly(true);
        ui->downloadlimit->setText("1024");
    }
    else if(ui->interfaceTitle->text()=="DSL"){
        ui->downloadlimit->setReadOnly(true);
        ui->downloadlimit->setText("2024");
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
}
void MainWindow::on_checkbox_action_triggered_from_uplinkCheckbox(/*const QModelIndex &index*/)
{ //we can use index in each below but fuck it
    ui->interfaceTitle->setText(inputstring());

    if(ui->interfaceTitle->text()=="25K"){
       ui->uploadlimit->setReadOnly(true);
       ui->uploadlimit->setText("25");
        }
    else if(ui->interfaceTitle->text()=="56K"){
       ui->uploadlimit->setReadOnly(true);
       ui->uploadlimit->setText("56");
    }
    else if(ui->interfaceTitle->text()=="80K"){
        ui->uploadlimit->setReadOnly(true);
        ui->uploadlimit->setText("80");
    }
    else if(ui->interfaceTitle->text()=="100K"){
        ui->uploadlimit->setReadOnly(true);
        ui->uploadlimit->setText("100");
    }
    else if(ui->interfaceTitle->text()=="120K"){
        ui->uploadlimit->setReadOnly(true);
        ui->uploadlimit->setText("120");
    }
    else if(ui->interfaceTitle->text()=="150K"){
        ui->uploadlimit->setReadOnly(true);
        ui->uploadlimit->setText("150");
    }
    else if(ui->interfaceTitle->text()=="200K"){
        ui->uploadlimit->setReadOnly(true);
        ui->uploadlimit->setText("200");
    }
    else if(ui->interfaceTitle->text()=="Edge"){
        ui->uploadlimit->setReadOnly(true);
        ui->uploadlimit->setText("250");
    }
    else if(ui->interfaceTitle->text()=="3G"){
        ui->uploadlimit->setReadOnly(true);
        ui->uploadlimit->setText("750");
    }
    else if(ui->interfaceTitle->text()=="LTE / 4G"){
        ui->uploadlimit->setReadOnly(true);
        ui->uploadlimit->setText("1024");
    }
    else if(ui->interfaceTitle->text()=="DSL"){
        ui->uploadlimit->setReadOnly(true);
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
}
//END this is deep shit
//END this is deep shit

//to enable/disble buttons
void MainWindow::enabledisableButtons(){

    //when no preset is selected and no checkbox is checked
    if(((ui->interfaceTitle->text().count()==0) && ((ui->limitDownloadCheckBox->checkState()== Qt::Unchecked)||(ui->limitUploadCheckBox->checkState()== Qt::Unchecked)))&&((ui->downloadlimit->text().count()<1)||(ui->uploadlimit->text().count()<1))){
        //disable all buttons
        ui->startThrottleBtn->setEnabled(false);

    }
    //when preset is selected and one of checkbox is checked
    else if(((ui->interfaceTitle->text().count()>0) &&((ui->limitDownloadCheckBox->checkState()==Qt::Checked)||(ui->limitUploadCheckBox->checkState()==Qt::Checked)))&&((ui->downloadlimit->text().count()>0)||(ui->uploadlimit->text().count()>0))) {
        //enable buttons
        ui->startThrottleBtn->setEnabled(true);

    }
    //when both the checkbox are unchecked
    else if((ui->limitDownloadCheckBox->checkState()==Qt::Unchecked)&&(ui->limitUploadCheckBox->checkState()==Qt::Unchecked)){
        //disable all buttons
        ui->startThrottleBtn->setEnabled(false);

    }


}

//when checkbox states changed-----------------------------------/* */
void MainWindow::on_limitDownloadCheckBox_stateChanged(int arg1) /* */
{                                                                /* */
    if(arg1==2){ui->downlinkFrame->setEnabled(true);             /* */
        on_checkbox_action_triggered_from_downloadCheckbox();    /* */
    }                                                            /* */
    else if(arg1==0){                                            /* */
        ui->downloadlimit->setText("999999999");                 /* */
        ui->downlinkFrame->setEnabled(false);                    /* */
    }                                                            /* */
    enabledisableButtons();                                      /* */
                                                                 /* */
}                                                                /* */
                                                                 /* */
void MainWindow::on_limitUploadCheckBox_stateChanged(int arg1)   /* */
{                                                                /* */
    if(arg1==2){ui->uplinkFrame->setEnabled(true);               /* */
        on_checkbox_action_triggered_from_uplinkCheckbox();      /* */
    }                                                            /* */
    else if(arg1==0){                                            /* */
        ui->uploadlimit->setText("999999999");                   /* */
        ui->uplinkFrame->setEnabled(false);}                     /* */
    enabledisableButtons();                                      /* */
                                                                 /* */
}                                                                /* */
//when checkbox states changed-----------------------------------/* */


// Start/Stop clicked
void MainWindow::on_startThrottleBtn_clicked()
{
 //    checks if none of the value passed are empty to throttler , it breaks the execution of engine if the value passed is blank and pops a message in terminal1 to input the blank values or uncheck the checkbox.
    if((ui->limitDownloadCheckBox->isChecked() || ui->limitUploadCheckBox->isChecked()) && (ui->downloadlimit->text().count()<1 || ui->uploadlimit->text().count()<1)){
            ui->terminal1->clear();

QString damn ;
            if(ui->downloadlimit->text().count()<1){
                damn ="Download limit section" ;}
            else if(ui->uploadlimit->text().count()<1){damn="Upload limit section";}
            ui->terminal1->appendPlainText("Please set limit or uncheck the checkbox to start throttling .\nThrottling not started (Required values not passed by "+ damn + ").");

        }
    else
    //start app engine
    startThrottler();

}
void MainWindow::on_stopThrottleBtn_clicked()
{
    //stop app engine
    stopThrottler();


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
   // ui->mainFrame->setEnabled(false);
    ui->treeWidget->setEnabled(false);
    ui->tabWidget->setEnabled(false);

  //  ui->startThrottleBtn->setEnabled(false);
}
void MainWindow::enableGUI(){
    ui->mainFrame->setEnabled(true);
    ui->treeWidget->setEnabled(true);
    ui->tabWidget->setEnabled(true);
   // ui->startThrottleBtn->setEnabled(true);
}


//core processes
void MainWindow::startThrottler(){
    QString mainprog = "gksudo";
    QStringList arguments;
    arguments << "wondershaper" << ui->interfaceCombo->currentText() << ui->downloadlimit->text() << ui->uploadlimit->text();
    throttler = new QProcess(this);
    throttler->start(mainprog, arguments);
//    QObject::connect(throttler,SIGNAL(readyReadStandardOutput()),this,SLOT(printOutput()));
    QObject::connect(throttler,SIGNAL(finished(int)),this,SLOT(on_check()));
//    ui->stopThrottleBtn->setEnabled(true);
//    ui->startThrottleBtn->setEnabled(false);
}

void MainWindow::stopThrottler(){
    QString mainprog = "gksudo";
    QStringList arguments;
    arguments << "wondershaper" << "clear" << ui->interfaceCombo->currentText();
    stopthrottler = new QProcess(this);
    stopthrottler->start(mainprog, arguments);
    QObject::connect(stopthrottler,SIGNAL(finished(int)),this,SLOT(on_check()));

    qDebug()<<"Stopping"<<ui->interfaceCombo->currentText();

}

//check button clicked
void MainWindow::on_check()
{
QString mainprog = "wondershaper";
QStringList arguments;
arguments << ui->interfaceCombo->currentText();
status = new QProcess(this);
status->start(mainprog, arguments);
QObject::connect(status ,SIGNAL(readyReadStandardOutput()),this,SLOT(printOutputInTerminal1()));
enabledisableButtons();
}


//print status in terminal1
void MainWindow::printOutputInTerminal1(){
    ui->terminal1->clear();
    QString output;
    output = status->readAll();
    ui->terminal1->appendPlainText(output);
    //qDebug()<<output ;
    if(output.contains("avgidle")){
        ui->interfaceStatusLable->setText("Throttling currnet interface");
        if(ui->interfaceStatusLable->text()=="Throttling currnet interface"){
            ui->stopThrottleBtn->setEnabled(true);
            ui->startThrottleBtn->setEnabled(false);
            ui->statusIcon->setPixmap(QPixmap(":/icons/icon2.png"));
            //disable the modifiers for other interfaces only disable for interface which is being throttled
            disableGUI();
        }
    }
    else{
        ui->interfaceStatusLable->setText("Not Throttling currnet interface");
        ui->stopThrottleBtn->setEnabled(false);
        ui->statusIcon->setPixmap(QPixmap(":/icons/icon1.png"));
        //enable the modifiers
        enableGUI();

    }
}

//printoutput() when sta
//void MainWindow::printOutput(){
//    QString line(stopthrottler->readAllStandardOutput() );
//    ui->statusBar->showMessage(line);
//   //on_check();
//   //  qDebug()<<throttler;
//}



//Start tray shits here
void MainWindow::setVisible(bool visible){
    minimizeAction->setEnabled(visible);
//  maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("UThrottle going to systray"),
                                 tr("UThrottle will keep running in the "
                                    "system tray"));
        hide();
        event->ignore();
    }
}

void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize to tray"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

//    maximizeAction = new QAction(tr("Maximize"), this);
//    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    runinbackground = new QAction(tr("Run in background"),this);
    connect(runinbackground,SIGNAL(triggered()), this ,SLOT(on_actionRuninBackground_triggered()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(on_actionQuit_triggered()));
}

void MainWindow::createTrayIcon()
{   //make menu
    trayIconMenu = new QMenu(this);
    //add actions to menu
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(runinbackground);
//  trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
//  initialize tray
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icons/tray.svg")); //only svg working
}
//End tray shits


//check the status of throttling when interface changed (live update)
void MainWindow::on_interfaceCombo_currentIndexChanged(int index)
{
    on_check();
}
//show hide terminal
void MainWindow::on_showterminal_clicked(bool checked)
{
if(checked){ui->terminalFrame->show();}
else if(!checked){ui->terminalFrame->hide();}
}
//runinbackground
void MainWindow::on_actionRuninBackground_triggered()
{
    qDebug()<<"Uthrottle don't take a single bit of RAM when running in background. Happy Throttling !";
    qApp->quit();
}


//TODO get the list of interfaces which throttler is throttling
//done but buggy right now
void MainWindow::check_throttling_interface(){

    QString mainprog = "wondershaper";
    QStringList arguments;
    arguments << ui->interfaceCombo2->currentText();
    status = new QProcess(this);
    status->start(mainprog, arguments);
    //QObject::connect(status ,SIGNAL(readyReadStandardOutput()),this,SLOT(printOutputInTerminal1()));
    QObject::connect(status ,SIGNAL(readyReadStandardOutput()),this,SLOT(printthestatus()));
    QObject::connect(status ,SIGNAL(finished(int)),this,SLOT(finished_check_change_index()));


}
//change the interfaces
void MainWindow::printthestatus(){

    QString quitout;
    quitout= status->readAll();

    if(quitout.contains("avgidle")){ ui->terminal1->appendPlainText("Throttling "+ui->interfaceCombo2->currentText()); }

   // finished_check_change_index();
}

void MainWindow::finished_check_change_index(){
    int total_interface;
    total_interface = ui->interfaceCombo2->count();
    int currentIndx;
    currentIndx = ui->interfaceCombo2->currentIndex();



    while (currentIndx < total_interface && currentIndx != -1) {
        ui->interfaceCombo2->setCurrentIndex(++currentIndx);
        check_throttling_interface();
       // printthestatus();
        qDebug()<<total_interface << currentIndx;
 break;
    }



}

void MainWindow::on_pushButton_clicked()
{
    ui->terminal1->clear();
    ui->interfaceCombo2->setCurrentIndex(0);
    check_throttling_interface();

}




