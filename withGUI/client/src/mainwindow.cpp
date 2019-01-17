#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostInfo>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->m_msg = new msgProcess();
    this->m_file = new fileProcess();
    this->m_pasv = new QTcpSocket;
    this->m_socket = new QTcpSocket;
    this->m_port = new QTcpSocket;
    this->m_server = new QTcpServer;
    this->mode = 0;
    this->port = 4096;
    this->transMode = 0;
    this->underWorking = "";
    // get host ip
    QHostInfo info = QHostInfo::fromName(QHostInfo::localHostName());
    foreach (QHostAddress adress, info.addresses()) {
        if(adress.protocol() == QAbstractSocket::IPv4Protocol){
            this->ip = adress.toString().toStdString();
        }
    }
    connectInit();
    tableInit();
}

void MainWindow::connectInit(){
    connect(ui->connect, SIGNAL(clicked()), this, SLOT(initClient()));
    connect(ui->disconnect, SIGNAL(clicked()), this, SLOT(closeClient()));
    connect(ui->last_folder, SIGNAL(clicked()), this, SLOT(returnPath()));
    connect(ui->type, SIGNAL(clicked()), this, SLOT(type()));
    connect(ui->syst, SIGNAL(clicked()), this, SLOT(syst()));
    connect(ui->table_file, SIGNAL(cellDoubleClicked(int,int)),this,SLOT(EnterPath(int,int)));
    connect(ui->table_file, SIGNAL(cellClicked(int,int)),this,SLOT(getFile(int,int)));
    connect(ui->rmd, SIGNAL(clicked()), this, SLOT(rmd()));
    connect(ui->mkd, SIGNAL(clicked()), this, SLOT(mkd()));
    connect(ui->mv, SIGNAL(clicked()), this, SLOT(mv()));
    connect(ui->upload, SIGNAL(clicked()), this, SLOT(upload()));
    connect(ui->download, SIGNAL(clicked()), this, SLOT(download()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeMode()));
    // tcp server
    connect(this->m_server, SIGNAL(newConnection()), SLOT(getConnect()));
}

void MainWindow::tableInit(){
    // set table
    ui->table_file->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_file->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table_file->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(245,245,245);};");
    ui->table_tasks->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(245,245,245);};");
    ui->table_tasks->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_file->setFont(QFont("Helvetica"));
    ui->table_tasks->setFont(QFont("Helvetica"));
}

MainWindow::~MainWindow() {
    delete ui;
    closeClient();
}

void MainWindow::initClient(){
    const string ip = ui->lineEdit_ip->text().toStdString();
    const string port = ui->lineEdit_port->text().toStdString();
    int m_port = atoi(port.c_str());
    const string user = ui->lineEdit_user->text().toStdString();
    const string pass = ui->lineEdit_pass->text().toStdString();
    if(ip.length() == 0 || port.length() == 0 || user.length() == 0 || pass.length() == 0){
        string str = "please finish init info";
        addCmd(str);
        return;
    }
    //客户端建立
    string str = "";
    this->m_socket->connectToHost(QString::fromStdString(ip),m_port,QTcpSocket::ReadWrite);
    if(!this->m_socket->waitForConnected()) {
        str = "client connect error";
    }
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    else{
        str = "read fail";
    }
    addCmd(str);

    // login in
    str = "USER "+user+"\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);

    str = "PASS "+pass+"\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);

    // init
    listFolder();
}

void MainWindow::closeClient(){
    string str = "QUIT\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    str += "-------------disconnect-------------";
    addCmd(str);
    ui->cmdText->moveCursor(QTextCursor::End);
}

void MainWindow::listFolder(){
    string path = getPath();
    pasvMode();
    string list = getList(path);
    vector<file> fileInfo = m_file->getFileInfo(list);
    ui->table_file->setRowCount(fileInfo.size());
    // show tables
    for(int i = 0; i<fileInfo.size(); i++){
        ui->table_file->setItem(i,0,new QTableWidgetItem(QString::fromStdString(fileInfo[i].fileName)));
        ui->table_file->setItem(i,1,new QTableWidgetItem(QString::fromStdString(fileInfo[i].size)));
        ui->table_file->setItem(i,2,new QTableWidgetItem(QString::fromStdString(fileInfo[i].type)));
        ui->table_file->setItem(i,3,new QTableWidgetItem(QString::fromStdString(fileInfo[i].time)));
    }
}

string MainWindow::getPath(){
    string str = "PWD\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    ui->cmdText->moveCursor(QTextCursor::End);
    return str;
}

void MainWindow::pasvMode(){
    string str = "PASV\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    // get ip & port
    string ip = m_msg->getIp(str);
    int m_port = m_msg->getPort(str);
    this->m_pasv->connectToHost(QString::fromStdString(ip),m_port,QTcpSocket::ReadWrite);
    if(!this->m_pasv->waitForConnected()) {
        str = "client connect error";
    }
    ui->cmdText->moveCursor(QTextCursor::End);
}

void MainWindow::portMode(){
    // listen
    newListen();
    // send msg
    string m_ip = m_msg->replace(this->ip, '.', ',');
    m_ip += ",";
    int first = this->port/256;
    int second = this->port%256;
    m_ip = m_ip + to_string(first) + "," + to_string(second);
    string str = "PORT " + m_ip + "\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    ui->cmdText->moveCursor(QTextCursor::End);
}

string MainWindow::getList(string path){
    string list = "";
    string str = "LIST "+path+"\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    if(this->m_pasv->waitForReadyRead(3000)){
        list = m_pasv->readAll();
        list = m_msg->removeEnd(list);
    }
    m_pasv->disconnectFromHost();
    addCmd(list);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    ui->cmdText->moveCursor(QTextCursor::End);
    return list;
}

void MainWindow::returnPath(){
    string str = "CWD ..\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    // to do check is root
    listFolder();
}

// x is row number
void MainWindow::EnterPath(int x, int){
    QTableWidgetItem * item = new QTableWidgetItem;
    item = ui->table_file->item(x,0);
    string s = item->text().toStdString();

    string str = "CWD " + s + "\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    // to do check is root
    listFolder();
}

void MainWindow::type(){
    string str = "TYPE I\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    ui->cmdText->moveCursor(QTextCursor::End);
}

void MainWindow::syst(){
    string str = "SYST I\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    ui->cmdText->moveCursor(QTextCursor::End);
}

void MainWindow::getFile(int x, int){
    this->select = x;
}

void MainWindow::rmd(){
    QTableWidgetItem * item = new QTableWidgetItem;
    item = ui->table_file->item(this->select,0);
    string s = item->text().toStdString();
    if(s[s.length()-1] == '\0'){
        s = s.substr(0,s.length()-1);
    }
    string str = "RMD " + s + "\r\n";
    m_socket->write(QByteArray::fromStdString(str));
    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    listFolder();
}

void MainWindow::mkd(){
    string s = ui->fileName->text().toStdString();
    string str = "MKD " + s + "\r\n";
    m_socket->write(QByteArray::fromStdString(str));

    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    listFolder();
}

void MainWindow::mv(){
    // rnfr
    QTableWidgetItem * item = new QTableWidgetItem;
    item = ui->table_file->item(this->select,0);
    string s = item->text().toStdString();
    if(s[s.length()-1] == '\0'){
        s = s.substr(0,s.length()-1);
    }
    string str = "RNFR " + s + "\r\n";
    m_socket->write(QByteArray::fromStdString(str));

    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);

    // rnto
    s = ui->fileName->text().toStdString();
    str = "RNTO " + s + "\r\n";
    m_socket->write(QByteArray::fromStdString(str));

    addCmd(str);
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    addCmd(str);
    listFolder();
}

void MainWindow::upload(){
    this->transMode = 1;
    string s = ui->load_file->text().toStdString();
    this->underWorking = s;
    string str = "STOR " + s + "\r\n";
    if(this->mode == 0){
        fileThread* t = new fileThread;
        t->mode = 1;
        t->m_socket = this->m_socket;
        t->s = s;
        t->str = str;
        connect(t, SIGNAL(updateList()), SLOT(listFolder()));
        t->start();
        // list
        addTask(s, "pasv", "upload");
    }
    else{
        portMode();
        m_socket->write(QByteArray::fromStdString(str));
        addCmd(str);
        if(this->m_socket->waitForReadyRead(3000)){
            str = m_socket->readAll();
            str = m_msg->removeEnd(str);
        }
        addCmd(str);
    }
}

void MainWindow::download(){
    this->transMode = 0;
    QTableWidgetItem * item = new QTableWidgetItem;
    item = ui->table_file->item(this->select,0);
    string s = item->text().toStdString();
    if(s[s.length()-1] == '\0'){
        s = s.substr(0,s.length()-1);
    }
    this->underWorking = s;
    // check if exit else rest
    // check rest
    string str;
    QFileInfo f(QString::fromStdString(s));
    if(f.exists()){
        QFile file(QString::fromStdString(s));
        if(!file.open(QFile::ReadOnly|QFile::Text)){
            str = "fail to open file";
        }
        QTextStream in(&file);
        string content = "";
        QString myText = in.readAll();
        content = myText.toStdString();
        file.close();
        str = "REST ";
        str += to_string(content.length());
        str += " ";
        str += s;
        str += "\r\n";
    }
    else{
        str = "RETR " + s + "\r\n";
    }
    addCmd(str);
    if(this->mode == 0){
        fileThread* t = new fileThread;
        t->mode = 0;
        t->m_socket = this->m_socket;
        t->s = s;
        t->str = str;
        t->start();
        ui->cmdText->moveCursor(QTextCursor::End);
        // add task table
        addTask(s, "pasv", "download");
    }
    else{
        portMode();
        m_socket->write(QByteArray::fromStdString(str));
        addCmd(str);
        if(this->m_socket->waitForReadyRead(3000)){
            str = m_socket->readAll();
            str = m_msg->removeEnd(str);
        }
        addCmd(str);
        if(this->m_socket->waitForReadyRead(3000)){
            str = m_socket->readAll();
            str = m_msg->removeEnd(str);
        }
        addCmd(str);
    }
}

void MainWindow::addTask(string file, string mode, string type){
    int row = ui->table_tasks->rowCount();
    ui->table_tasks->setRowCount(row+1);
    ui->table_tasks->setItem(row,0,new QTableWidgetItem(QString::fromStdString(file)));
    ui->table_tasks->setItem(row,1,new QTableWidgetItem(QString::fromStdString(mode)));
    ui->table_tasks->setItem(row,2,new QTableWidgetItem(QString::fromStdString(type)));
}

void MainWindow::addCmd(string str){
    QString tmp = ui->cmdText->toPlainText();
    tmp.append(QString::fromStdString(str));
    ui->cmdText->setPlainText(tmp);
}

void MainWindow::newListen(){
    if(!this->m_server->listen(QHostAddress::Any, this->port)){
        this->m_server->close();
        string str = "fail to listen";
        addCmd(str);
    }
}

void MainWindow::changeMode(){
    this->mode = ui->comboBox->currentIndex();
}

void MainWindow::getConnect(){
    string str = "";
    this->m_port = this->m_server->nextPendingConnection();
    if(this->transMode == 0){
        QFile file(QString::fromStdString(this->underWorking));
        if(!file.open(QFile::WriteOnly|QFile::Text)){
            str = "fail to create file";
            addCmd(str);
            return;
        }
        QTextStream out(&file);
        if(this->m_port->waitForReadyRead(3000)){
            str = m_port->readAll();
            str = m_msg->removeEnd(str);
        }
        out << QString::fromStdString(str);
        file.flush();
        file.close();
        this->m_server->close();
        ui->cmdText->moveCursor(QTextCursor::End);
        // add task table
        addTask(this->underWorking, "port", "download");
    }
    else{
        QFile file(QString::fromStdString(this->underWorking));
        if(!file.open(QFile::ReadOnly|QFile::Text)){
            str = "fail to open file";
            addCmd(str);
        }
        QTextStream in(&file);
        QString myText = in.readAll();
        string content = myText.toStdString();
        m_port->write(QByteArray::fromStdString(content+'\0'));
        m_port->flush();
        if(this->m_socket->waitForReadyRead(3000)){
            str = m_socket->readAll();
            str = m_msg->removeEnd(str);
        }
        addCmd(str);
        this->m_server->close();
        listFolder();
        // list
        addTask(this->underWorking, "port", "upload");
        ui->cmdText->moveCursor(QTextCursor::End);
    }
}
