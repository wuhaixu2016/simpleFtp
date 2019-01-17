#include "fileprocess.h"

fileProcess::fileProcess()
{

}

// multi interval is same as sigle
vector<string> split(string text, char interval){
    vector<string> list;
    string tmp = "";
    for(int i = 0; i<text.length(); i++){
        if(text[i] == interval && text[i+1] != interval){
            if(i == text.length()-1){
                break;
            }
            list.push_back(tmp);
            tmp = "";
        }
        else{
            tmp = tmp + text[i];
        }
    }
    list.push_back(tmp);
    return list;
}

vector<file> fileProcess::getFileInfo(string info){
    vector<file> str;
    vector<string> tmp = split(info, '\n');
    for(int i = 0; i<tmp.size(); i++){\
        if(i == 0){
            continue;
        }
        vector<string> tmpInfo = split(tmp[i],' ');
        file tmpfile;
        tmpfile.fileName = tmpInfo[8];
        tmpfile.size = tmpInfo[4];
        tmpfile.time = tmpInfo[5] + " " + tmpInfo[6]+ " " +tmpInfo[7];
        tmpfile.type = tmpInfo[0];
        str.push_back(tmpfile);
    }
    return str;
}
