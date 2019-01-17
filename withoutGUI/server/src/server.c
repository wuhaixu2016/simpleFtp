#include "handler.h"
#include <pthread.h> 

// malloc new thread for each connect
int main(int argc, char **argv) {
	char rootPath[200] = "/tmp";
	int myPort = 21;
	for (int i = 0; i < argc; i++) {
		if (strcmp("-root", argv[i]) == 0) {
			strcpy(rootPath, argv[i + 1]);
		}
		else if (strcmp("-port", argv[i]) == 0) {
			myPort = atoi(argv[i + 1]);
		}
	}
	chdir(rootPath);
	int listenfd, connfd;		// first for listening; second for transfer
	struct sockaddr_in addr;
	Connection* newConnect = NULL;

	// create socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		return 1;
	}

	// set ip and port
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(myPort);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);	// listen"0.0.0.0"

	// bind ip and port to socket
	if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		return 1;
	}

	// begin listen
	if (listen(listenfd, maxLink) == -1) {
		return 1;
	}
	pthread_t newThreadId[100];
	int count = 0;
	// listening
	while (1) {
		// wait for connect-blocked
		if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
			continue;
		}
		newConnect = (Connection*)malloc(sizeof(Connection)); // to release
		newConnect->connfd = connfd;
		newConnect->status = 0;
		newConnect->ftpMode = normal;
		int flag = pthread_create(&newThreadId[count], NULL, (void*)multiThreadConnection, newConnect);
		if (flag != 0) {
		}
		count++;
		count %= 100;
	}
	close(listenfd);
}

