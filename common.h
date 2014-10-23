// Prool BBS
// common header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>

#define MESSAGE_DIR "messages/"
#define CMD_DIR "cmd/"
#define BUFFERSIZE 255

#define MAX_NAME_LEN 15

#define TICK 500000 // microsec
