#ifndef FILEPROCESS_H
#define FILEPROCESS_H
#include <string>
#include <vector>
using namespace std;
typedef struct file
{
    string fileName;
    string size;
    string type;
    string time;
}file;

class fileProcess
{
public:
    fileProcess();
    vector<file> getFileInfo(string info);
};

#endif // FILEPROCESS_H
