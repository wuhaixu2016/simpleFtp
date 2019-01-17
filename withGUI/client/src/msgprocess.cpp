#include "msgprocess.h"

msgProcess::msgProcess()
{

}

string msgProcess::removeEnd(string msg){
    for(int i = msg.length()-1; i>=0; i--){
        if(msg[i] != '\r' && msg[i] != '\n'){
            msg[i+1] = '\0';
            break;
        }
    }
    return msg;
}

string msgProcess::getIp(string msg){
    string ip = "";
    int flag = 0;
    for(int i = 0; i<msg.length(); i++){
        if(msg[i] == '('){
            flag = 1;
            continue;
        }
        if(flag > 0){
            if(msg[i] == ','){
                flag += 1;
                if(flag == 5){
                    break;
                }
                ip += ".";
            }
            else if(msg[i] == ')'){
                break;
            }
            else{
                ip += msg[i];
            }
        }
    }
    return ip;
}

int msgProcess::getPort(string msg){
    string first = "";
    string second = "";
    int flag = 0;
    for(int i = 0; i<msg.length(); i++){
        if(msg[i] == '('){
            flag = 1;
            continue;
        }
        if(flag > 0){
            if(msg[i] == ','){
                flag += 1;
            }
            else if(msg[i] == ')'){
                break;
            }
            else if(flag == 5){
                first += msg[i];
            }
            else if(flag == 6){
                second += msg[i];
            }
        }
    }
    int port = atoi(first.c_str())*256 + atoi(second.c_str());
    return port;
}

string msgProcess::replace(string msg, char s, char t){
    for(int i = 0; i<msg.length(); i++){
        if(msg[i] == s){
            msg[i] = t;
        }
    }
    return msg;
}
