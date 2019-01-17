#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <fcntl.h>

#include "utli.h"

void getIpFromStr(char* adress, int* port, int* ipAdress);

int getSentence(int connfd, char* sentence);

int writeSentence(int sockfd, const char* sentence, int len);

int getCommand(char* sentence, int len);

void writeFile(int sockfd, char* tmp);

int portMode(int* sockfd, struct sockaddr_in* addr, int port, char* ip);

// same as normalMode
int pasvMode(int* sockfd, struct sockaddr_in* addr, int port, char* ip);

int normalMode(int* sockfd, struct sockaddr_in* addr, int port, char* ip);