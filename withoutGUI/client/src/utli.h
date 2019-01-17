#define bufsize 8192

typedef enum commandList {
	USER, PASS, RETR, STOR, QUIT, SYST, TYPE, PORT, PASV, MKD, CWD, PWD, LIST, RMD, RNFR, RNTO
}commandList;

typedef enum mode {
	normal, activeMode, passiveMode
}mode;

static const char commandName[][16] = {
	"USER", "PASS", "RETR", "STOR", "QUIT", "SYST", "TYPE", "PORT", "PASV", "MKD", "CWD", "PWD", "LIST", "RMD", "RNFR", "RNTO"
};