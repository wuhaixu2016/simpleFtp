#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <vector>
#include "filethread.h"
#include "msgprocess.h"
#include "fileprocess.h"

class QTcpSocket;
class QTcpServer;
class msgProcess;
class fileProcess;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private slots:
    void initClient();
    void closeClient();
    void listFolder();
    void returnPath();
    void EnterPath(int, int);
    void type();
    void syst();
    void getFile(int, int);
    void rmd();
    void mkd();
    void mv();
    void upload();
    void download();
    void changeMode();
    // for port
    void getConnect();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    string getPath();
    void pasvMode();
    void portMode();
    string getList(string path);
    void addTask(string file, string mode, string type);
    void addCmd(string str);
    void connectInit();
    void tableInit();
    void newListen();

private:
    Ui::MainWindow *ui;
    QTcpSocket *m_socket;
    QTcpSocket *m_pasv;
    QTcpServer *m_server;
    QTcpSocket *m_port;
    msgProcess *m_msg;
    fileProcess *m_file;
    int select;
    int mode; // 0 is pasv; 1 is port;
    int port;
    int transMode; // 0 is down, 1 is up;
    string underWorking;
    string ip;
};

#endif // MAINWINDOW_H
