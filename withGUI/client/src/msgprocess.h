#ifndef MSGPROCESS_H
#define MSGPROCESS_H
#include <string>
#include <QString>
using namespace std;

class msgProcess
{
public:
    msgProcess();
    string removeEnd(string msg);
    string getIp(string msg);
    int getPort(string msg);
    string replace(string msg, char s, char t);
};

#endif // MSGPROCESS_H
