#include "transMode.h"

int main(int argc, char **argv) {

	char ip[200] = "127.0.0.1"; 
	int myPort = 21;
	for (int i = 0; i < argc; i++) {
		if (strcmp("-ip", argv[i]) == 0) {
			strcpy(ip, argv[i + 1]);
		}
		else if (strcmp("-port", argv[i]) == 0) {
			myPort = atoi(argv[i + 1]);
		}
	}
	int sockfd, sendFd, ftpFd;
	int port[2] = { 0 };
	int ipAdress[4] = { 0 };
	struct sockaddr_in addr,pasvAddr,portAddr;
	int status = normal;
	char sentence[bufsize] = { '\0' };
	// create connect
	sockfd = normalMode(&sockfd, &addr, myPort, ip);
	getSentence(sockfd, sentence);
	printf("%s\n", sentence);
	fflush(stdout);

	// message loop
	while (1) {
		fgets(sentence, bufsize, stdin);
		int len = strlen(sentence);
		sentence[len-1] = '\r';
		sentence[len] = '\n';
		sentence[len + 1] = '\0';
		writeSentence(sockfd, sentence, len);
		int myCommand = getCommand(sentence, len);

		// USER, PASS, RETR, STOR, QUIT, SYST, TYPE, PORT, PASV, MKD, CWD, PWD, LIST, RMD, RNFR, RNTO
		if (myCommand != RETR && myCommand != STOR && myCommand != LIST) {
			if (myCommand == PORT) {
				status = activeMode;
				char tmpStr[bufsize] = { '\0' };
				char tmpCommand[10] = { '\0' };
				sscanf(sentence, "%s %s", tmpCommand, tmpStr);
				getIpFromStr(tmpStr, port, ipAdress);
			}
			getSentence(sockfd, sentence);
			printf("from server: %s\n", sentence);
			fflush(stdout);
			if (myCommand == PASV) {
				status = passiveMode;
				char tmpStr[bufsize] = { '\0' };
				char tmpCommand[10] = { '\0' };
				sscanf(sentence, "%s %s %s (%s", tmpCommand, tmpCommand, tmpCommand, tmpStr);
				getIpFromStr(tmpStr, port, ipAdress);
			}
		}
		else if(myCommand == LIST) {
			int MYPORT = port[0] * 256 + port[1];
			char MYIP[bufsize] = { '\0' };
			sprintf(MYIP, "%d.%d.%d.%d", ipAdress[0], ipAdress[1], ipAdress[2], ipAdress[3]);
			if (status == passiveMode) {
				// GET
				sendFd = pasvMode(&ftpFd, &pasvAddr, MYPORT, MYIP);// to do port & ip
			}
			else if (status == activeMode) {
				// INPUT
				sendFd = portMode(&ftpFd, &portAddr, MYPORT, MYIP);// to do port & ip
			}
			getSentence(sendFd, sentence);
			printf("from server: %s\n", sentence);
			fflush(stdout);
			close(sendFd);
			if (status == activeMode) {
				close(ftpFd);
			}
			status = normal;
		}
		else if (myCommand == RETR) {
			int MYPORT = port[0] * 256 + port[1];
			char MYIP[bufsize] = { '\0' };
			sprintf(MYIP, "%d.%d.%d.%d", ipAdress[0], ipAdress[1], ipAdress[2], ipAdress[3]);
			char tmp[bufsize] = { '\0' };
			for (int i = 5; i < len; i++)
			{
				if (sentence[i] == '\0' || sentence[i] == '\n' || sentence[i] == '\r') {
					break;
				}
				tmp[i - 5] = sentence[i];
			}
			if (status == passiveMode) {
				sendFd = pasvMode(&ftpFd, &pasvAddr, MYPORT, MYIP);
			}
			else if (status == activeMode) {
				sendFd = portMode(&ftpFd, &portAddr, MYPORT, MYIP);
			}
			getSentence(sockfd, sentence);
			printf("from server: %s\n", sentence);
			fflush(stdout);
			// write
			writeFile(sendFd, tmp);
			close(sendFd);
			if (status == activeMode) {
				close(ftpFd);
			}
			getSentence(sockfd, sentence);
			printf("from server: %s\n", sentence);
			fflush(stdout);
			status = normal;
		}
		else if (myCommand == STOR) {
			int MYPORT = port[0] * 256 + port[1];
			char MYIP[bufsize] = { '\0' };
			sprintf(MYIP, "%d.%d.%d.%d", ipAdress[0], ipAdress[1], ipAdress[2], ipAdress[3]);
			char tmp[bufsize] = { '\0' };
			for (int i = 5; i < len; i++)
			{
				if (sentence[i] == '\0' || sentence[i] == '\n'|| sentence[i] == '\r') {
					break;
				}
				tmp[i - 5] = sentence[i];
			}
			if (status == passiveMode) {
				sendFd = pasvMode(&ftpFd, &pasvAddr, MYPORT, MYIP);
			}
			else if (status == activeMode) {
				sendFd = portMode(&ftpFd, &portAddr, MYPORT, MYIP);
			}
			getSentence(sockfd, sentence);
			printf("from server: %s\n", sentence);
			fflush(stdout);
			// finish connect
			int flag = open(tmp, O_RDONLY, S_IRUSR | S_IWUSR | S_IROTH);
			if (flag == -1) {
				printf("open %s false\n",tmp);
				fflush(stdout);
			}
			char file[bufsize] = { '\0' };
			while (read(flag, file, bufsize - 1) > 0) {
				writeSentence(sendFd, file, strlen(file));
			}
			close(flag);
			close(sendFd);
			if (status == activeMode) {
				close(ftpFd);
			}
			getSentence(sockfd, sentence);
			printf("from server: %s\n", sentence);
			fflush(stdout);
			status = normal;
		}
	}
	close(sockfd);
	return 0;
}
