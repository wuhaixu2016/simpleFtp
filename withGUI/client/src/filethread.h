#ifndef FILETHREAD_H
#define FILETHREAD_H

#include "msgprocess.h"
#include "fileprocess.h"
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QHostInfo>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

class fileThread: public QThread
{
    Q_OBJECT

signals:
    void updateList();

public:
    fileThread();
    void run();

    void handlerRetr();
    void handlerStor();

    int mode; // 0 is retr; 1 is stor
    QTcpSocket* m_socket;
    QTcpSocket* m_pasv;
    string s; // filename
    string str;
};

#endif // FILETHREAD_H
