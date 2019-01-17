#define bufsize 8192
#define maxLink 10

typedef enum command {
	USER, PASS, RETR, STOR, QUIT, SYST, TYPE, PORT, PASV, MKD, CWD, PWD, LIST, RMD, RNFR, RNTO, REST
}command;

typedef enum mode {
	normal, activeMode, passiveMode
}mode;

static const char commandName[][16] = {
	"USER", "PASS", "RETR", "STOR", "QUIT", "SYST", "TYPE", "PORT", "PASV", "MKD", "CWD", "PWD", "LIST", "RMD", "RNFR", "RNTO", "REST"
};


static const char greeting[] = "220 FTP server ready\r\n";
static const char loginIn[] = "331 please give email\r\n";
static const char success[] = "230 Welcome\r\n";
static const char systResponse[] = "215 UNIX Type: L8\r\n";
static const char typeSucc[] = "200 Type set to I.\r\n";
static const char portSucc[] = "200 PORT command successful.\r\n";
static const char portFail[] = "500 PORT command Fail.\r\n";
static const char portOpen[] = "50 Opening BINARY mode data connection for";
static const char pasvSucc[] = "227 Entering Passive Mode ";
static const char pasvOpen[] = "150 Opening BINARYmode data connection for";
static const char complete[] = "226 Transfer complete.\r\n";
static const char mkdSucc[] = "250 mkdir successful.\r\n";
static const char rmdSucc[] = "250 rmdir successful.\r\n";
static const char rnfrSucc[] = "250 rnfr successful.\r\n";
static const char rntoSucc[] = "250 rnto successful.\r\n";
static const char cwdSucc[] = "250 chdir successful.\r\n";
static const char finish[] = "221 Goodbye.\r\n";
static const char unaccept[] = "500 unaccept syntax\r\n";
static const char noMode[] = "500 please define the mode first.\r\n";
static const char permissionDeny[] = "530 permission denied\r\n";
