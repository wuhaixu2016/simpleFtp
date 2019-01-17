#include "handler.h"


void getPort(int* port) {
	srand((unsigned)time(NULL));
	int randomPort = rand() % (65535 - 20000) + 20000;
	port[0] = randomPort / 256;
	port[1] = randomPort % 256;
}

void getIp(int* ipAdress) {
	char ip[20] = {'\0'};
    char hname[128];
    struct hostent *hent;
    int i;
    gethostname(hname, sizeof(hname));
    //hent = gethostent();
    hent = gethostbyname(hname);
    for(i = 0; hent->h_addr_list[i]; i++) {
    	sprintf(ip,"%s",inet_ntoa(*(struct in_addr*)(hent->h_addr_list[i])));
    }

	ipAdress[0] = 127;
	ipAdress[1] = 0;
	ipAdress[2] = 0;
	ipAdress[3] = 1;
	int k = 0;
	int tmp = 0;
	for(int i = 0; i<strlen(ip); i++){
		if(ip[i] == '.'){
			ipAdress[k] = tmp;
			tmp = 0;
			k += 1;
		}
		else{
			tmp = tmp*10 + ip[i] -'0';
		}
	}
	ipAdress[k] = tmp;
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
	case REST:
		handleREST(connection, sentence);
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

void* threadrest(conPack* pack){
	// get name
	Connection* connection = pack->connection;
	char* sentence = pack->sentence;
	char tmp[bufsize] = { '\0' };
	for (int i = 5; i < bufsize; i++) {
		if (sentence[i] == '\n' || sentence[i] == '\0' || (i== strlen(sentence)-1) || sentence[i] == '\r')
			break;
		else {
			tmp[i - 5] = sentence[i];
		}
	}
	// get
    int filesize = 0;
    char filename[100] = {'\0'};
    int j = 0;
    int tag = 0;
    for(int i = 0; i<strlen(tmp); i++)
    {
        if(tmp[i] == ' '){
            tag = 1;
            continue;
        }
        if(tag == 0){
            filesize = filesize*10 + tmp[i] - '0';
        }
        else{
            filename[j] = tmp[i];
            j++;
        }
    }
	int flag = open(filename, O_RDONLY, S_IRWXG | S_IRWXO | S_IRWXU);
	if (flag == -1) {
		writeSentence(connection->connfd, unaccept, strlen(unaccept));
		return NULL;
	}
	if(filesize > 0){
		lseek(flag, filesize, SEEK_SET);
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
			int n = write(pack->sendFd, file + p, len - p);
			if (n < 0) {
				return NULL;
			}
			else {
				p += n;
			}
		}
	}
	close(flag);
	close(pack->sendFd);
	if (connection->ftpMode == passiveMode) {
		close(connection->ftpFd);
	}
	writeSentence(connection->connfd, complete, strlen(complete));
	connection->ftpMode = normal;
	return NULL;
}


void handleREST(Connection* connection, char* sentence){
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
	printf("1\n");
	fflush(stdout);
	conPack* t = (conPack*)malloc(sizeof(conPack));;
	t->connection = connection;
	t->sentence = sentence;
	t->sendFd = connection->sendFd;
	pthread_t newthread;
	pthread_create(&newthread, NULL, (void*)threadrest, t);
}

void* threadretr(conPack* pack){
	// get name
	Connection* connection = pack->connection;
	char* sentence = pack->sentence;
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
		return NULL;
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
			int n = write(pack->sendFd, file + p, len - p);
			if (n < 0) {
				return NULL;
			}
			else {
				p += n;
			}
		}
	}
	close(flag);
	close(pack->sendFd);
	if (connection->ftpMode == passiveMode) {
		close(connection->ftpFd);
	}
	writeSentence(connection->connfd, complete, strlen(complete));
	connection->ftpMode = normal;
	return NULL;
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
	conPack* t = (conPack*)malloc(sizeof(conPack));;
	t->connection = connection;
	t->sentence = sentence;
	t->sendFd = connection->sendFd;
	pthread_t newthread;
	pthread_create(&newthread, NULL, (void*)threadretr, t);
}

void* threadstor(conPack* pack){
	Connection* connection = pack->connection;
	char* sentence = pack->sentence;
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
		return NULL;
	}
	struct stat buf;
	stat(tmp, &buf);
	char resp[bufsize] = { '\0' };
	if (connection->ftpMode == activeMode) {
		sprintf(resp, "%s %s\r\n", portOpen, tmp);
	}
	if (connection->ftpMode == passiveMode) {
		sprintf(resp, "%s %s\r\n", pasvOpen, tmp);
	}
	writeSentence(connection->connfd, resp, strlen(resp));
	// write file
	int n = 1;
	while (n>0) {
		n = read(pack->sendFd, sentence, bufsize-1);
		if (n < 0) {
			break;
		}
		else if (n == 0) {
			break;
		}
		int len = strlen(sentence);
		for(int i = 0; i<len; i++){
			if(sentence[i] == '\r'){
				sentence[i] = ' ';
			}
		}
		if (write(flag, sentence, strlen(sentence)) == 0) {
			break;
		}
		if(n < bufsize-1){
			break;
		}
	}
	//save
	close(flag);
	close(pack->sendFd);
	if (connection->ftpMode == passiveMode) {
		close(connection->ftpFd);
	}
	writeSentence(connection->connfd, complete, strlen(complete));
	connection->ftpMode = normal;
	return NULL;
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
	conPack* t = (conPack*)malloc(sizeof(conPack));;
	t->connection = connection;
	t->sentence = sentence;
	t->sendFd = connection->sendFd;
	pthread_t newthread;
	pthread_create(&newthread, NULL, (void*)threadstor, t);
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
	char trans[] = "list\r\n";
	writeSentence(connection->connfd, trans, strlen(trans));
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
	writeSentence(connection->connfd, trans, strlen(trans));
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