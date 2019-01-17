#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utli.h"

typedef struct Connection
{
	int connfd;
	int status;

	struct sockaddr_in pasvAddr;
	struct sockaddr_in activeAddr;
	int ftpFd;
	int sendFd;
	int ftpMode;

	char oldPath[100];
} Connection;

void getPort(int* port);

void getIp(int* ipAdress);

void getAdress(char*adress, int* port, int * ipAdress);

void getIpFromStr(char* adress, int* port, int* ipAdress);

int getSentence(int connfd, char* sentence);

int writeSentence(int connfd, const char* sentence, int len);

int getCommand(char* sentence, int len);

int handleCommand(Connection* connection, int command, char* sentence, int len);

// USER, PASS, RETR, STOR, QUIT, SYST, TYPE, PORT, PASV, MKD, CWD, PWD, LIST, RMD, RNFR, RNTO
void handleUSER(Connection* connection, char* sentence);
void handlePASS(Connection* connection, char* sentence);
void handleRETR(Connection* connection, char* sentence);
void handleSTOR(Connection* connection, char* sentence);
void handleQUIT(Connection* connection);
void handleSYST(Connection* connection);
void handleTYPE(Connection* connection, char* sentence);
void handlePORT(Connection* connection, char* sentence);
void handlePASV(Connection* connection);
void handleMKD(Connection* connection, char* sentence);
void handleCWD(Connection* connection, char* sentence);
void handlePWD(Connection* connection);
void handleLIST(Connection* connection, char* sentence);
void handleRMD(Connection* connection, char* sentence);
void handleRNFR(Connection* connection, char* sentence);
void handleRNTO(Connection* connection, char* sentence);
void handleERROR(Connection* connection);
void handlePERMISSION(Connection* connection);

void *multiThreadConnection(Connection* connection);