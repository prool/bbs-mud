// BBS CLIENT, Prool

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <ncurses.h>
#include <iconv.h>
#include <locale.h> // for UTF-8 under ncurses

#include "common.h"

#define STRLEN 255
#define LOG "./client.log"

// codetables for console and log
#define T_KOI			0
#define T_UTF			1
#define T_WIN			2
#define T_ALT			3
#define T_LAT			4

// globals
int max_x, max_y;  
int pid;
char pids[10];
int codetable;

// prototypes
void S(char *str);
void newline();

// functions
void utf8_to_koi(char *str_i, char *str_o)
{
	iconv_t cd;
	size_t len_i, len_o = BUFFERSIZE*2;
	size_t i;

	if ((cd = iconv_open("KOI8-R", "UTF-8")) == (iconv_t) - 1)
	{
		printf("utf8_to_koi: iconv_open error\n");
		return;
	}
	len_i = strlen(str_i);
	if ((i=iconv(cd, &str_i, &len_i, &str_o, &len_o)) == (size_t) - 1)
	{
		printf("utf8_to_koi: iconv error\n");
		// return;
	}
	if (iconv_close(cd) == -1)
	{
		printf("utf8_to_koi: iconv_close error\n");
		return;
	}
}

void koi_to_utf8(char *str_i, char *str_o)
{
	iconv_t cd;
	size_t len_i, len_o = BUFFERSIZE*2;
	size_t i;

	if ((cd = iconv_open("UTF-8", "KOI8-R")) == (iconv_t) - 1)
	{
		printf("koi_to_utf8: iconv_open error\n");
		return;
	}
	len_i = strlen(str_i);
	if ((i=iconv(cd, &str_i, &len_i, &str_o, &len_o)) == (size_t) - 1)
	{
		printf("koi_to_utf8: iconv error\n");
		// return;
	}
	if (iconv_close(cd) == -1)
	{
		printf("koi_to_utf8: iconv_close error\n");
		return;
	}
}

void utf8_to_win(char *str_i, char *str_o)
{
	iconv_t cd;
	size_t len_i, len_o = BUFFERSIZE*2;
	size_t i;

	if ((cd = iconv_open("CP1251", "UTF-8")) == (iconv_t) - 1)
	{
		printf("utf8_to_win: iconv_open error\n");
		return;
	}
	len_i = strlen(str_i);
	if ((i=iconv(cd, &str_i, &len_i, &str_o, &len_o)) == (size_t) - 1)
	{
		printf("utf8_to_win: iconv error\n");
		// return;
	}
	if (iconv_close(cd) == -1)
	{
		printf("utf8_to_win: iconv_close error\n");
		return;
	}
}

void win_to_utf8(char *str_i, char *str_o)
{
	iconv_t cd;
	size_t len_i, len_o = BUFFERSIZE*2;
	size_t i;

	if ((cd = iconv_open("UTF-8", "CP1251")) == (iconv_t) - 1)
	{
		printf("win_to_utf8: iconv_open error\n");
		return;
	}
	len_i = strlen(str_i);
	if ((i=iconv(cd, &str_i, &len_i, &str_o, &len_o)) == (size_t) - 1)
	{
		printf("win_to_utf8: iconv error\n");
		// return;
	}
	if (iconv_close(cd) == -1)
	{
		printf("win_to_utf8: iconv_close error\n");
		return;
	}
}

void fromwin (char *str)
{char buf [BUFFERSIZE*2];
win_to_utf8(str,buf);
strcpy(str,buf);
}

void outhex(char *str)
{
while(*str)
	{
	printf("'%X' ", *str++&0xFFU);
	}
printf("\n");
}

void version(void)
{
printw("Prool BBS client compilation %s %s\n", __DATE__, __TIME__);
}

void command(char *cmd, char *p1, char *p2)
{char path[STRLEN]; FILE *fcmd;
time_t mytime;

mytime = time(0);

sprintf(path,"%s%i.%i",CMD_DIR,pid,mytime);
// S(path); // debug
#define S_ERR "fopen error. check directories 'cmd' and 'messages'!\n\n\n\n"
if((fcmd=fopen(path,"w"))==NULL) {S(S_ERR); printf(S_ERR); exit(2);}
if(fprintf(fcmd, "%s\n%s\n%s\n", cmd, p1, p2)==EOF) S("fputs error\n");
if(fclose(fcmd)) S("fclose error\n");
refresh();
}

void gets_(char *str, int len)
{char c; char *p; int char_count;
p=str;
char_count=0;
	while(1)
	{
	c=getch();
	if (c!=ERR)
		if (c=='\n')
			{
			*p=0;
			break;
			}
		else
			{
			*p++=c;
			if(char_count++>=len) break;
			}
	}
}

void help(void)
{
S("Prool BBS client help");
S("");
S("help - help");
S("exit, quit - exit");
S("koi, utf - set codetable");
S("ping - ping");
S("ver - version");
S("who - who");
S("gossip - gossip (болтать). Например: gossip Всем привет");
S("Или сокращенно так:");
S("\" Всем привет");
S("Вас услышат во всем игровом Мире");
S("tell - сказать игроку (приватно), например tell prool hello, prool");
S("Или так:");
S("сказать prool привет Прулю!");
S("say - говорить внутри комнаты. Например, say hello, guys!");
S("Или сокращенно:");
S("' hello, guys!");
S("(Вас услышат только в той комнате, где Вы находитесь)");
S("look - look (или см)");
S("Команды перемещения: с ю з в или north east west south");
S("");
S("Примечание:");
S("Так как мад только разрабатывается, то особого сервиса ПОКА нет!");
S("Команды надо вводить только полностью, так как написано в хелпе");
S("и в том же регистре букв. Между командой и параметром должен");
S("быть только один пробел. Угадывания команд по первым буквам пока нет.");
S("Редактирования строки пока нет. Воды и атмосферы тоже нет,");
S("даже роботов пока нет");
S("");
S("(Prool site: http://mud.kharkov.org/)");
refresh();
}

void getstring(char *str, int maxlen)
{char c; int i;

for (i=0; i<maxlen; i++,str++)
	{
	*str=getch();
	if (*str=='\n')
		{
		*str=0;
		return;
		}
	}
*str=0;
}

void newline()
{int x, y;

getyx(stdscr, y, x);
if (y==max_y-1)
	{
	move(0,0);
	insdelln (-1);
	move(y,0);
	}
}

void S(char *str)
{char utfbuf[BUFFERSIZE*2];
int i;

newline();
if (codetable==1) {for (i=0;i<BUFFERSIZE*2;i++) utfbuf[i]=0; koi_to_utf8(str,utfbuf); printw("%s\n",utfbuf);}
else printw("%s\n",str);
}

// ======================= MAIN ===================================

int main(int argc, char **argv, char **env)
{
char buf[BUFFERSIZE];
char path[BUFFERSIZE];
char name[BUFFERSIZE];
char p0[BUFFERSIZE];
char p1[BUFFERSIZE];
char c, *p, *pp, *params, *p2;
int i, x, y, char_count;
DIR *dd;
struct dirent *nextfile;
FILE *fcmd, *flog;
int trigger_exit=0;
chtype ch;

int pechal=0;

codetable=T_UTF;

#if 0
printf("koi: тест русских КОИ восемь Эр\n");
printf("utf: п╒п╣я│я┌ - пёп╒п╓ п╡п╬я│п╣п╪я▄");
printf("\n\n");
printf("press any key\n\n");
getchar();
#endif

// prool: config file processing

FILE *fconfig;
char string[STRLEN];

fconfig=fopen("bbs.cfg","r");
if (fconfig)
	{
	//printf("bbs.cfg open\n");
	while (!feof(fconfig))
		{char *pp;
		string[0]=0;
		fgets(string,STRLEN,fconfig);
		pp=strchr(string,'\n');
		if (pp) *pp=0;
		// printf("`%s'\n", string); // debug print
		if (!strcmp(string,"test")) printf("TEST OK!\n");
		else if (!strcmp(string,"client_codetable_utf")) codetable=T_UTF;
		else if (!strcmp(string,"client_codetable_koi")) codetable=T_KOI;
		}
	fclose(fconfig);
	}
else
	{
	printf("bbs.cfg not open\n");
	}

setlocale(LC_CTYPE, ""); // for UTF-8 under ncurses
initscr();

max_x=getmaxx(stdscr);
max_y=getmaxy(stdscr);

getyx(stdscr, y, x);

for (i=0;i<max_y;i++) printw("\n");
refresh();

if (!has_colors()) printw("Цвета не поддерживаются!\n");
start_color();

// 1 цвет в палитре - белые символы на чёрном фоне
init_pair(1, COLOR_WHITE, COLOR_BLACK);

// 2 цвет в палитре - зелёные символы на фоне
init_pair(2, COLOR_GREEN, COLOR_BLACK);
init_pair(3, COLOR_RED, COLOR_BLACK);

newline();
pid=getpid();
// printw("pid=%i", pid=getpid());
sprintf(pids,"%i",pid);
printw("pid=%s\n", pids);
newline();
//printw("argc=%i", argc);
//printw("argv=%s", argv[0]);
//printw("env=%s", env[0]);

ch = 'w' | COLOR_PAIR(1); // символ с цветом 1 из палитры

// attron(COLOR_PAIR(2));
// attron(A_REVERSE);

version();
S("Use help for help!");
S("Используйте команду помощь для получения помощи");
// attron(A_UNDERLINE);
S("(site: http://mud.kharkov.org)");
#if 0 // test of UTF-8
S("koi: тест русских\n\n");
S("utf: я┌п╣я│я│я│я│ я┌п╣я│я┌");
S("\n\n");
#endif
// attroff(A_UNDERLINE);

newline();
attron(COLOR_PAIR(1));
printw("Enter Your name: ");
refresh();
gets_(name, BUFFERSIZE);
// S(name);
refresh();

if (name[0]==0) strcpy(name, "Enter");

// регистрация
command("LOGON", name, 0);

halfdelay(1);
// noecho();

// тест, запущен ли сервер
usleep(TICK+1);
		if ((dd=opendir(CMD_DIR))!=NULL)
		while((nextfile=readdir(dd))!=NULL)
			{char path[BUFFERSIZE];
			char str[STRLEN];
			int j;
			FILE *fm;

			if (nextfile->d_name[0]=='.') continue;
			strcpy(path,MESSAGE_DIR);
			strcat(path,nextfile->d_name);
			j=atoi(path+strlen(MESSAGE_DIR));
			if (j==pid)
				{
				attron(COLOR_PAIR(3));
				newline();
				printw ("Сервер, похоже, не запущен. Печаль!\n");
				pechal=1;
				attron(COLOR_PAIR(1));
				}
			}
		closedir(dd);

// определение своего IP
	{i=0;
	while(env[i])
		{
		strcpy(buf,env[i]);
		if(pp=strchr(buf,'='))*pp=0;
		if (!strcmp(buf,"SSH_CLIENT"))
			{
			strcpy(buf,pp+1);
			S(buf);
			break;
			}
		i++;
		}
	}

command("IP", buf, 0);

if (pp=strchr(buf,' ')) *pp=0;


if((flog=fopen(LOG,"a"))==NULL) S("fopen log error\n");
if(fprintf(flog, "%s %s login %s %s\n", ptime(), name, nslookup(buf), buf)==EOF)
	S("fputs log error\n");
if(fclose(flog)) S("fclose log error\n");

while(1)
{
p=buf;
char_count=0;
for (i=0;i<BUFFERSIZE;i++) buf[i]=0;

newline();
printw ("> %s\n", buf);
// S("> ");
// S(buf);
	while(1)
	{
	c=getch();
	if (c!=ERR)
		if (c=='\n')
			{
			*p=0;
			break;
			}
		else
			{
			*p++=c;
			if(char_count++>=BUFFERSIZE) break;
			}
	else
		{
		if ((dd=opendir(MESSAGE_DIR))!=NULL)
		while((nextfile=readdir(dd))!=NULL)
			{char path[BUFFERSIZE];
			char str[STRLEN];
			int j;
			FILE *fm;

			if (nextfile->d_name[0]=='.') continue;
			strcpy(path,MESSAGE_DIR);
			strcat(path,nextfile->d_name);
			// newline();
			// printw("%s\n", path);
			// newline();
			j=atoi(path+strlen(MESSAGE_DIR));
			// printw("j=%i\n", j);
			// newline();
			if (j==pid)
				{
				attron(COLOR_PAIR(2));
				fm=fopen(path,"r");
				while (!feof(fm))
					{
					str[0]=0;
					fgets(str, STRLEN, fm);
					if (!strcmp(str,"!!EXIT") && trigger_exit)
						{
						fclose(fm);
						unlink(path);
						S("Exit. Press any key\n");
						newline();
						refresh();
						nocbreak();
						getch();
						endwin();
						return 2;
						}
					S(str); // $$$
					newline();
					}
				attron(COLOR_PAIR(1));
				fclose(fm);
				unlink(path);
				printw ("> %s\n", buf);
				}
			}
		closedir(dd);
		}
	}
p1[0]=0;p2=0;
strcpy(p0,buf);
if((pp=strchr(p0,' '))!=NULL)
	{
	*pp=0; 
	params=pp+1;
	strcpy(p1,params);
	if (p1[0])
		if (pp=strchr(p1,' '))
		{
		*pp=0;
		p2=pp+1;
		}
	}
else {params=0; p2=0;}

if(buf[0]==0) ;
else if ((!strcmp(p0,"quit")) || (!strcmp(p0,"exit")) || (!strcmp(p0,"конец"))
|| (!strcmp(p0,"выход")))
	{command ("EXIT", 0, 0); trigger_exit=1; if (pechal) {endwin(); return 3;} }
else if ((!strcmp(p0,"help")) || (!strcmp(p0,"помощь"))) {help(); }
else if (!strcmp(p0,"koi")) {codetable=0; S("Set KOI-8R codetable");}
else if (!strcmp(p0,"utf")) {codetable=1; S("Set UTF-8 codetable");}
else if (!strcmp(p0,"ping")) {command("PING", 0, 0); }
else if ((!strcmp(p0,"who")) || (!strcmp(p0,"кто"))) {command("WHO", 0, 0); }
else if ((!strcmp(p0,"look")) || (!strcmp(p0,"см"))) {command("LOOK", 0, 0); }
else if ((!strcmp(p0,"north")) || (!strcmp(p0,"с"))) {command("north", 0, 0); }
else if ((!strcmp(p0,"south")) || (!strcmp(p0,"ю"))) {command("south", 0, 0); }
else if ((!strcmp(p0,"west")) || (!strcmp(p0,"з"))) {command("west", 0, 0); }
else if ((!strcmp(p0,"east")) || (!strcmp(p0,"в"))) {command("east", 0, 0); }
else if ((!strcmp(p0,"up")) || (!strcmp(p0,"вв"))) {command("up", 0, 0); }
else if ((!strcmp(p0,"down")) || (!strcmp(p0,"вн"))) {command("down", 0, 0); }
else if ((!strcmp(p0,"gossip"))||(!strcmp(p0,"\""))) {command("GOSSIP", params, 0); }
else if ((!strcmp(p0,"say"))||(!strcmp(p0,"'"))) {command("SAY", params, 0); }
else if ((!strcmp(p0,"tell"))||(!strcmp(p0,"сказать"))) {command("TELL", p1, p2); }
else if (!strcmp(p0,"env"))
	{i=0;
	while(env[i])
		{
		printw("env[%i]=%s",i,env[i]);
		newline();
		i++;
		}
	}
else if (!strcmp(p0,"ver"))
	{
	version();
	newline();
	command("VER", 0, 0);
	}
else
	{
	newline(); printw("unknown command `%s'",buf); refresh();
	}
}

endwin();
return 1;
}
