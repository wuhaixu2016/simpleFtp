#include "handler.h"

void getPort(int* port) {
	srand((unsigned)time(NULL));
	int randomPort = rand() % (65535 - 20000) + 20000;
	port[0] = randomPort / 256;
	port[1] = randomPort % 256;
}

void getIp(int* ipAdress) {
	char ip[bufsize] = { '\0' };
	char *temp = NULL;
	int inet_sock;
	struct ifreq ifr;
	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	memset(ifr.ifr_name, 0, sizeof(ifr.ifr_name));
	memcpy(ifr.ifr_name, "eth0", strlen("eth0"));
	if (0 != ioctl(inet_sock, SIOCGIFADDR, &ifr))
	{
		return ;
	}
	temp = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr);
	memcpy(ip, temp, strlen(temp));
	close(inet_sock);
	int k = 0;
	int tmp = 0;
	for (int i = 0; i < strlen(ip); i++) {
		if (ip[i] != ',' && ip[i] != '\n' && ip[i] != '\0' && ip[i] == '\r') {
			tmp = tmp * 10 + ip[i] - '0';
		}
		else {
			ipAdress[k] = tmp;
			tmp = 0;
			k += 1;
		}
	}
	if (k == 3) {
		ipAdress[3] = tmp;
	}
}

void getAdress(char*adress, int* port, int * ipAdress) {
	sprintf(adress, "(%d,%d,%d,%d,%d,%d)", ipAdress[0], ipAdress[1], ipAdress[2], ipAdress[3],port[0],port[1]);
}

void getIpFromStr(char* adress, int* port, int* ipAdress) {
	int k = 0;
	int tmp = 0;
	for (int i = 0; i < strlen(adress); i++) {
		if (adress[i] != ',' && adress[i] != '\n' && adress[i] != '\0' && adress[i] != '\r'){
			tmp = tmp * 10 + adress[i] - '0';
		}	
		else {
			if (k == 4 || k==5) {
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
	while (1) {
		int n = read(connfd, sentence + p, bufsize - 1 - p);
		if (n < 0) {
			close(connfd);
			return -1;
		}
		else if (n == 0) {
			break;
		}
		else {
			p += n;
			if (sentence[p - 1] == '\n') {
				break;
			}
		}
	}
	// add '\0'
	sentence[p - 1] = '\0';
	int len = p - 1;
	return len;
}

int writeSentence(int connfd, const char* sentence, int len) {
	//send str to socket
	int p = 0;
	while (p < len) {
		int n = write(connfd, sentence + p, len - p);
		if (n < 0) {
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
		if (sentence[i] == ' ' || i == len-1) {
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

// USER, PASS, RETR, STOR, QUIT, SYST, TYPE, PORT, PASV, MKD, CWD, PWD, LIST, RMD, RNFR, RNTO
int handleCommand(Connection* connection, int command, char* sentence, int len) {
	if (connection->status == 0) {
		if (command != USER) {
			handlePERMISSION(connection);
			return 0;
		}
	}
	if (connection->status == 1) {
		if (command != PASS && command != USER) {
			handlePERMISSION(connection);
			return 0;
		}
	}

	switch (command)
	{
	case USER:
		handleUSER(connection, sentence);
		return 0;
	case PASS:
		handlePASS(connection, sentence);
		return 0;
	case RETR:
		handleRETR(connection, sentence);
		break;
	case STOR:
		handleSTOR(connection, sentence);
		break;
	case QUIT:
		handleQUIT(connection);
		return -1;
		break;
	case SYST:
		handleSYST(connection);
		break;
	case TYPE:
		handleTYPE(connection, sentence);
		break;
	case PORT:
		handlePORT(connection, sentence);
		break;
	case PASV:
		handlePASV(connection);
		break;
	case MKD:
		handleMKD(connection, sentence);
		break;
	case CWD:
		handleCWD(connection, sentence);
		break;
	case PWD:
		handlePWD(connection);
		break;
	case LIST:
		handleLIST(connection, sentence);
		break;
	case RMD:
		handleRMD(connection, sentence);
		break;
	case RNFR:
		handleRNFR(connection, sentence);
		break;
	case RNTO:
		handleRNTO(connection, sentence);
		break;
	default:
		handleERROR(connection);
		break;
	}
	return 0;
}

void handleUSER(Connection* connection, char* sentence) {
	char tmp[] = "USER anonymous";
	for (int i = 0; i < strlen(tmp); i++) {
		if (tmp[i] != sentence[i]) {
			handlePERMISSION(connection);
			return;
		}
	}
	connection->status = 1;
	writeSentence(connection->connfd, loginIn, strlen(loginIn));
	return ;
}

void handlePASS(Connection* connection, char* sentence) {
	connection->status = 2;
	writeSentence(connection->connfd, success, strlen(success));
}

void handleRETR(Connection* connection, char* sentence) {
	if (connection->ftpMode == normal) {
		writeSentence(connection->connfd, noMode, strlen(noMode));
		return;
	}
	else if (connection->ftpMode == activeMode) {
		if (connect(connection->sendFd, (struct sockaddr*)&connection->activeAddr, sizeof(connection->activeAddr)) < 0) {
			writeSentence(connection->connfd, noMode, strlen(noMode));
			return;
		}
	}
	else {
		if ((connection->sendFd = accept(connection->ftpFd, NULL, NULL)) == -1) {
			writeSentence(connection->connfd, noMode, strlen(noMode));
			return;
		}
	}
	// get name
	char tmp[bufsize] = { '\0' };
	for (int i = 5; i < bufsize; i++) {
		if (sentence[i] == '\n' || sentence[i] == '\0' || (i== strlen(sentence)-1) || sentence[i] == '\r')
			break;
		else {
			tmp[i - 5] = sentence[i];
		}
	}
	int flag = open(tmp, O_RDONLY, S_IRUSR | S_IWUSR |S_IROTH);
	if (flag == -1) {
		writeSentence(connection->connfd, unaccept, strlen(unaccept));
		return;
	}
	struct stat buf;
	stat(tmp, &buf);
	char resp[bufsize] = { '\0' };
	if (connection->ftpMode == activeMode) {
		sprintf(resp, "%s %s (%lu bytes)\r\n", pasvOpen, tmp, (unsigned long)(buf.st_size));
	}
	if (connection->ftpMode == passiveMode) {
		sprintf(resp, "%s %s (%lu bytes)\r\n", pasvOpen, tmp, (unsigned long)(buf.st_size));
	}
	writeSentence(connection->connfd, resp, strlen(resp));

	char file[bufsize] = { '\0' };
	int len;
	while ((len = read(flag, file, bufsize - 1)) > 0) {
		// write socket
		int p = 0;
		while (p < len) {
			int n = write(connection->sendFd, file + p, len - p);
			if (n < 0) {
				return;
			}
			else {
				p += n;
			}
		}
	}
	close(flag);
	close(connection->sendFd);
	if (connection->ftpMode == passiveMode) {
		close(connection->ftpFd);
	}
	writeSentence(connection->connfd, complete, strlen(complete));
	connection->ftpMode = normal;
}

void handleSTOR(Connection* connection, char* sentence) {
	if (connection->ftpMode == normal) {
		writeSentence(connection->connfd, noMode, strlen(noMode));
		return;
	}
	else if (connection->ftpMode == activeMode) {
		if (connect(connection->sendFd, (struct sockaddr*)&connection->activeAddr, sizeof(connection->activeAddr)) < 0) {
			writeSentence(connection->connfd, noMode, strlen(noMode));
			return;
		}
	}
	else {
		if ((connection->sendFd = accept(connection->ftpFd, NULL, NULL)) == -1) {
			writeSentence(connection->connfd, noMode, strlen(noMode));
			return;
		}
	}
	// get name
	char tmp[bufsize] = { '\0' };
	for (int i = 5; i < bufsize; i++) {
		if (sentence[i] == '\n' || sentence[i] == '\0' || (i == strlen(sentence)-1) || sentence[i] == '\r')
			break;
		else {
			tmp[i - 5] = sentence[i];
		}
	}
	// create file
	int flag = open(tmp, O_RDONLY | O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH);
	if (flag == -1) {
		writeSentence(connection->connfd, unaccept, strlen(unaccept));
		return;
	}
	struct stat buf;
	stat(tmp, &buf);
	char resp[bufsize] = { '\0' };
	if (connection->ftpMode == activeMode) {
		sprintf(resp, "%s %s (%lu bytes)\r\n", portOpen, tmp, (unsigned long)(buf.st_size));
	}
	if (connection->ftpMode == passiveMode) {
		sprintf(resp, "%s %s (%lu bytes)\r\n", pasvOpen, tmp, (unsigned long)(buf.st_size));
	}
	writeSentence(connection->connfd, resp, strlen(resp));
	// write file
	while (1) {
		int n = read(connection->sendFd, sentence, bufsize-1);
		if (n < 0) {
			break;
		}
		else if (n == 0) {
			break;
		}
		if (write(flag, sentence, n) == 0) {
			break;
		}
	}
	//save
	close(flag);
	close(connection->sendFd);
	if (connection->ftpMode == passiveMode) {
		close(connection->ftpFd);
	}
	writeSentence(connection->connfd, complete, strlen(complete));
	connection->ftpMode = normal;
}

void handleQUIT(Connection* connection) {
	writeSentence(connection->connfd, finish, strlen(finish));
}

void handleSYST(Connection* connection) {
	writeSentence(connection->connfd, systResponse, strlen(systResponse));
}

void handleTYPE(Connection* connection, char* sentence) {
	char tmp[] = "TYPE I";
	for (int i = 0; i < strlen(tmp); i++) {
		if (tmp[i] != sentence[i]) {
			handleERROR(connection);
		}
	}
	writeSentence(connection->connfd, typeSucc, strlen(typeSucc));
}

void handlePORT(Connection* connection, char* sentence) {
	// check and calculate input
	int ipAdress[4] = { 0 };
	int port[2] = { 0 };
	
	char tmp[bufsize] = { '\0' };
	for (int i = 5; i < bufsize; i++) {
		if (sentence[i] == '\0' || (i == strlen(sentence)-1) || sentence[i] == '\r' || sentence[i] == '\n')
			break;
		else {
			tmp[i - 5] = sentence[i];
		}
	}
	getIpFromStr(tmp, port, ipAdress);
	sprintf(tmp, "%d.%d.%d.%d", ipAdress[0], ipAdress[1], ipAdress[2], ipAdress[3]);

	// create socket
	if ((connection->sendFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		writeSentence(connection->connfd, portFail, strlen(portFail));
	}

	memset(&connection->activeAddr, 0, sizeof(connection->activeAddr));
	connection->activeAddr.sin_port = htons(port[0]*256+port[1]);
	connection->activeAddr.sin_family = AF_INET;

	if (inet_pton(AF_INET,tmp, &connection->activeAddr.sin_addr) <= 0) {			
		writeSentence(connection->connfd, portFail, strlen(portFail));
	}

	writeSentence(connection->connfd, portSucc, strlen(portSucc));
	// save
	connection->ftpMode = activeMode;

}

void handlePASV(Connection* connection) {
	int ipAdress[4] = { 0 };
	int port[2] = { 0 };
	getIp(ipAdress);
	getPort(port);
	char adress[bufsize] = { '\0' };
	strcpy(adress, pasvSucc);
	char ip[200] = { '\0' };
	getAdress(ip, port, ipAdress);
	strcat(adress, ip);
	strcat(adress, "\r\n");
	writeSentence(connection->connfd, adress, strlen(adress));
	
	// create socket
	if ((connection->ftpFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		return ;
	}

	// set ip and port
	memset(&connection->pasvAddr, 0, sizeof(connection->pasvAddr));
	connection->pasvAddr.sin_family = AF_INET;
	connection->pasvAddr.sin_port = htons(port[0] * 256 + port[1]);
	connection->pasvAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// listen"0.0.0.0"

	// bind ip and port to socket
	if (bind(connection->ftpFd, (struct sockaddr*)&connection->pasvAddr, sizeof(connection->pasvAddr)) == -1) {
	}

	// listen
	if (listen(connection->ftpFd, maxLink) == -1) {
	}
	// save
	connection->ftpMode = passiveMode;
}

void handleMKD(Connection* connection, char* sentence) {
	char tmp[bufsize] = { '\0' };
	for (int i = 4; i < bufsize; i++) {
		if (sentence[i] == '\n' || sentence[i] == '\0' || sentence[i] == '\r')
			break;
		else {
			tmp[i - 4] = sentence[i];
		}
	}

	int flag = mkdir(tmp, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (flag != 0) {
		writeSentence(connection->connfd, unaccept, strlen(unaccept));
	}
	else {
		writeSentence(connection->connfd, mkdSucc, strlen(mkdSucc));
	}
	return;
}

void handleCWD(Connection* connection, char* sentence) {
	char tmp[bufsize] = { '\0' };
	for (int i = 4; i < bufsize; i++) {
		if(sentence[i] == '\n' || sentence[i] == '\0'|| sentence[i] == '\r')
			break;
		else {
			tmp[i - 4] = sentence[i];
		}
	}
	int flag = chdir(tmp);
	if (flag != 0) {
		writeSentence(connection->connfd, unaccept, strlen(unaccept));
	}
	else {
		writeSentence(connection->connfd, cwdSucc, strlen(cwdSucc));
	}
	return;
}

void handlePWD(Connection* connection) {
	char tmp[bufsize] = { '\0' };
	getcwd(tmp, bufsize);
	tmp[strlen(tmp)] = '\r';
	tmp[strlen(tmp)] = '\n';
	if(strlen(tmp) != 0)
		writeSentence(connection->connfd, tmp, strlen(tmp));
	else
		writeSentence(connection->connfd, unaccept, strlen(unaccept));
}

void handleLIST(Connection* connection, char* sentence) {
	if (connection->ftpMode == normal) {
		writeSentence(connection->connfd, noMode, strlen(noMode));
		return;
	}
	else if (connection->ftpMode == activeMode) {
		if (connect(connection->sendFd, (struct sockaddr*)&connection->activeAddr, sizeof(connection->activeAddr)) < 0) {
			writeSentence(connection->connfd, noMode, strlen(noMode));
			return;
		}
	}
	else {
		if ((connection->sendFd = accept(connection->ftpFd, NULL, NULL)) == -1) {
			writeSentence(connection->connfd, noMode, strlen(noMode));
			return;
		}
	}
	char file[bufsize] = { '\0' };
	for (int i = 5; i < bufsize; i++) {
		if (sentence[i] == '\n' || sentence[i] == '\0' || sentence[i] == '\r')
			break;
		else {
			file[i - 5] = sentence[i];
		}
	}
	char command[bufsize] = { '\0' };
	sprintf(command, "ls -l %s 2>/dev/null", file);
	FILE* stream;
	stream = popen(command, "r");
	char tmp[bufsize] = { '\0' };
	char res[bufsize] = { '\0' };
	strcpy(res, "");
	while (fgets(tmp, bufsize - 1, stream)) {
		strcat(res, tmp);
	}
	writeSentence(connection->sendFd, res, strlen(res));
	close(connection->sendFd);
	if (connection->ftpMode == passiveMode) {
		close(connection->ftpFd);
	}
	connection->ftpMode = normal;
}

void handleRMD(Connection* connection, char* sentence) {
	char tmp[bufsize] = { '\0' };
	char fileRm[bufsize] = { '\0' };
	for (int i = 4; i < bufsize; i++) {
		if (sentence[i] == '\n' || sentence[i] == '\0' || sentence[i] == '\r')
			break;
		else {
			tmp[i - 4] = sentence[i];
		}
	}
	if (strcmp(tmp, "/") == 0) {
		writeSentence(connection->connfd, unaccept, strlen(unaccept));
		return;
	}
	sprintf(fileRm, "rm -rf %s", tmp);
	if (system(fileRm) == 0) {
		writeSentence(connection->connfd, rmdSucc, strlen(rmdSucc));
	}
	else {
		writeSentence(connection->connfd, unaccept, strlen(unaccept));
	}
}

void handleRNFR(Connection* connection, char* sentence) {
	char tmp[bufsize] = { '\0' };
	for (int i = 4; i < bufsize; i++) {
		if (sentence[i] == '\n' || sentence[i] == '\0'|| sentence[i] == '\r')
			break;
		else {
			tmp[i - 4] = sentence[i];
		}
	}
	sprintf(connection->oldPath, "%s", tmp);
	writeSentence(connection->connfd, rnfrSucc, strlen(rnfrSucc));
}

void handleRNTO(Connection* connection, char* sentence) {
	char tmp[bufsize] = { '\0' };
	for (int i = 4; i < bufsize; i++) {
		if (sentence[i] == '\n' || sentence[i] == '\0'|| sentence[i] == '\r')
			break;
		else {
			tmp[i - 4] = sentence[i];
		}
	}
	char mvCommand[bufsize] = { '\0' };
	sprintf(mvCommand, "mv %s %s", connection->oldPath, tmp);
	if (system(mvCommand) == 0) {
		writeSentence(connection->connfd, rntoSucc, strlen(rntoSucc));
	}
	else {
		writeSentence(connection->connfd, unaccept, strlen(unaccept));
	}
}

void handleERROR(Connection* connection) {
	writeSentence(connection->connfd, unaccept, strlen(unaccept));
}

void handlePERMISSION(Connection* connection) {
	writeSentence(connection->connfd, permissionDeny, strlen(permissionDeny));
}

void *multiThreadConnection(Connection* connection) {
	int error = writeSentence(connection->connfd, greeting, strlen(greeting));
	if (error) {
	}
	char sentence[bufsize];
	while (1) {
		int len = getSentence(connection->connfd, &sentence[0]);
		if (len < 0) {
			break;
		}
		int command = getCommand(&sentence[0], len);
		int flag = handleCommand(connection, command, &sentence[0], len); // -1Ê±ÍË³ö
		if (flag == -1) {
			break;
		}
	}
	close(connection->connfd);
	free(connection);
	return NULL;
}