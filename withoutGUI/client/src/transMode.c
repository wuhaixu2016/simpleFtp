#include "transMode.h"
#define maxConnect 10

void getIpFromStr(char* adress, int* port, int* ipAdress) {
	int k = 0;
	int tmp = 0;
	for (int i = 0; i < strlen(adress); i++) {
		if (adress[i] != ',' && adress[i] != '\n' && adress[i] != '\0' && adress[i] != ')') {
			tmp = tmp * 10 + adress[i] - '0';
		}
		else {
			if (k == 4 || k == 5) {
				port[k - 4] = tmp;
			}
			else {
				ipAdress[k] = tmp;
			}
			tmp = 0;
			k += 1;
		}
	}
	// k = 5
	if (k == 5) {
		port[k - 4] = tmp;
	}
}

int getSentence(int connfd, char* sentence) {
	// get str from socket
	int p = 0;
	int n = read(connfd, sentence + p, 8191 - p);
	if (n < 0) {
		printf("Error read(): %s(%d)\n", strerror(errno), errno);
		fflush(stdout);
		return 1;
	}
	else if (n == 0) {
	}
	else {
		p += n;
		if (sentence[p - 1] == '\n') {
		}
	}
	sentence[p - 1] = '\0';
	int len = p - 1;
	return len;
}

int writeSentence(int sockfd, const char* sentence, int len) {
	//send str to socket
	int p = 0;
	while (p < len) {
		int n = write(sockfd, sentence + p, len +1 - p);
		if (n < 0) {
			printf("error write(): %s(%d)\n", strerror(errno), errno);
			fflush(stdout);
			return 1;
		}
		else {
			p += n;
		}
	}
	return 0;
}

int getCommand(char* sentence, int len) {
	int strlength = 0;
	for (int i = 0; i < len; i++) {
		if (sentence[i] == ' ' || sentence[i] == '\n' || i == len - 1) {
			strlength = i;
			break;
		}
	}
	if (strlength == 0) {
		return -1;
	}
	else {
		char * command = (char *)malloc((strlength + 1) * sizeof(char));
		command[strlength] = '\0';
		for (int i = 0; i < strlength; i++) {
			command[i] = sentence[i];
		}
		int num = sizeof(commandName) / sizeof(char*);
		for (int i = 0; i < num; i++) {
			if (strcmp(commandName[i], command) == 0) {
				free(command);
				return i;
			}
		}
		free(command);
	}
	return -1;
}

void writeFile(int sockfd, char* tmp) {
	char sentence[bufsize] = { '\0' };
	int flag = open(tmp, O_RDONLY | O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH);
	while (1) {
		int n = read(sockfd, sentence, bufsize - 1);
		if (n < 0) {
			break;
		}
		else if (n == 0) {
			break;
		}
		int tag = write(flag, sentence, n);
		if (tag == 0) {
			break;
		}
	}
	close(flag);
}

int portMode(int* sockfd, struct sockaddr_in* addr, int port, char* ip) {
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		fflush(stdout);
		return -1;
	}
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = htonl(INADDR_ANY);	
	if (bind(*sockfd, (struct sockaddr*)addr, sizeof(*addr)) == -1) {
		printf("please change port\n");
		fflush(stdout);
		return -1;
	}
	if (listen(*sockfd, maxConnect) == -1) {
		printf("please change port\n");
		fflush(stdout);
		return -1;
	}
	int sendFd = -1;
	if ((sendFd = accept(*sockfd, NULL, NULL)) == -1) {
		printf("please change port\n");
		fflush(stdout);
		return -1;
	}
	return sendFd;
}

int pasvMode(int* sockfd, struct sockaddr_in* addr, int port, char* ip) {
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		fflush(stdout);
		return -1;
	}
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
	addr->sin_port = htons(port);
	if (inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {			
		printf("Error inet_pton(): %s(%d)\n", strerror(errno), errno);
		fflush(stdout);
		return -1;
	}
	if (connect(*sockfd, (struct sockaddr*)addr, sizeof(*addr)) < 0) {
		printf("Error connect(): %s(%d)\n", strerror(errno), errno);
		fflush(stdout);
		return -1;
	}
	return *sockfd;
}

int normalMode(int* sockfd, struct sockaddr_in* addr, int port, char* ip) {
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		fflush(stdout);
		return -1;
	}
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
	addr->sin_port = htons(port);
	if (inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
		printf("Error inet_pton(): %s(%d)\n", strerror(errno), errno);
		fflush(stdout);
		return -1;
	}
	if (connect(*sockfd, (struct sockaddr*)addr, sizeof(*addr)) < 0) {
		printf("Error connect(): %s(%d)\n", strerror(errno), errno);
		fflush(stdout);
		return -1;
	}
	return *sockfd;
}