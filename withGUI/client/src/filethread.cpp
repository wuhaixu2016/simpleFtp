#include "filethread.h"


fileThread::fileThread()
{

}

void fileThread::run(){
    switch (this->mode) {
    case 0:
        handlerRetr();
        break;
    case 1:
        handlerStor();
        break;
    default:
        break;
    }
}

void fileThread::handlerRetr(){
    this->m_pasv = new QTcpSocket;
    msgProcess *m_msg = new msgProcess;
    string p_str = "PASV\r\n";
    m_socket->write(QByteArray::fromStdString(p_str));
    if(this->m_socket->waitForReadyRead(3000)){
        p_str = m_socket->readAll();
        p_str = m_msg->removeEnd(p_str);
    }
    // get ip & port
    string ip = m_msg->getIp(p_str);
    int m_port = m_msg->getPort(p_str);
    this->m_pasv->connectToHost(QString::fromStdString(ip),m_port,QTcpSocket::ReadWrite);
    if(!this->m_pasv->waitForConnected()){
        p_str = "client connect error";
    }
    m_socket->write(QByteArray::fromStdString(str));
    if(this->m_socket->waitForReadyRead(3000)){
        m_socket->readAll();
    }
    // to do write
    QFile file(QString::fromStdString(s));
    QFileInfo f(QString::fromStdString(s));
    if(f.exists()){
        if(!file.open(QFile::WriteOnly|QFile::Text|QFile::Append)){
            return;
        }
    }
    else{
        if(!file.open(QFile::WriteOnly|QFile::Text)){
            return;
        }
    }
    QTextStream out(&file);
    str = "";
    while(true){
        str = "";
        if(this->m_pasv->waitForReadyRead(3000)){
            str = m_pasv->readAll();
            str = m_msg->removeEnd(str);
        }
        if(str.length() == 0){
            break;
        }
        out << QString::fromStdString(str);
        file.flush();
    }
    file.close();
    m_pasv->disconnectFromHost();
    if(this->m_socket->waitForReadyRead(3000)){
        m_socket->readAll();
    }
}

void fileThread::handlerStor(){
    this->m_pasv = new QTcpSocket;
    msgProcess *m_msg = new msgProcess;
    string p_str = "PASV\r\n";
    m_socket->write(QByteArray::fromStdString(p_str));
    if(this->m_socket->waitForReadyRead(3000)){
        p_str = m_socket->readAll();
    }
    // get ip & port
    string ip = m_msg->getIp(p_str);
    int m_port = m_msg->getPort(p_str);
    this->m_pasv->connectToHost(QString::fromStdString(ip),m_port,QTcpSocket::ReadWrite);
    if(!this->m_pasv->waitForConnected()){
        p_str = "client connect error";
    }
    m_socket->write(QByteArray::fromStdString(str));
    // get info
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
    }
    // read txt
    // to do find txt
    QFile file(QString::fromStdString(s));
    if(!file.open(QFile::ReadOnly|QFile::Text)){
        str = "fail to open file";
    }
    QTextStream in(&file);
    string content = "";
    QString myText = in.readAll();
    content = myText.toStdString();
    m_pasv->write(QByteArray::fromStdString(content));
    m_pasv->flush();
    m_pasv->disconnectFromHost();
    // get finish
    if(this->m_socket->waitForReadyRead(3000)){
        str = m_socket->readAll();
        str = m_msg->removeEnd(str);
    }
    emit updateList();
}
