// BBS SERVER, Prool

#include "common.h"
#include "server.h"

#define STRLEN 255
#define MAX_CHARS 10
#define MAX_X 10
#define MAX_Y 10
#define INIT_X 0
#define INIT_Y 0
#define MAX_CHARS_IN_ROOM 2

// structures

struct char_data
	{
	int pid;
	char name[MAX_NAME_LEN];
	int x,y;
	}
	chars[MAX_CHARS];

struct room_data
	{
	char *descr;
	int chars[MAX_CHARS_IN_ROOM];
	}
	rooms[MAX_X][MAX_Y];

// local functions definition

void send_pid(int pid, char *str);
void server_cmd(int pid, char *str, char *p1, char *p2);

// local functions bodys

char *kuda_ushel(int direct)
{
	switch(direct)
	{
	case NORTH: return "на север";
	case EAST: return "на восток";
	case SOUTH: return "на юг";
	case WEST: return "на запад";
	case UP: return "вверх";             
	case DOWN: return "вниз";
	default: return "к чорту";
	}
}

char *otkuda_prishel(int direct)
{
	switch(direct)
	{
	case NORTH: return "с юга";
	case EAST: return "с запада";
	case SOUTH: return "с севера";
	case WEST: return "с востока";
	case UP: return "снизу";             
	case DOWN: return "сверху";
	default: return "из Сумрака";
	}
}

void send_room(int x, int y, char *msg)
{int i, pid;
if ((x>=MAX_X)||(y>=MAX_Y)) return;
for (i=0; i<MAX_CHARS_IN_ROOM; i++)
	{
	if (pid=rooms[x][y].chars[i]) send_pid(pid, msg);
	}
}

void pid_leave_room(int pid)
{int i, j, x, y;
i=vnum_by_pid(pid);
if (i==-1) return;
// освобождение места в комнате
x=chars[i].x;
y=chars[i].y;
for (j=0;j<MAX_CHARS_IN_ROOM;j++)
	{
	if (rooms[x][y].chars[j]==pid)
		{
		rooms[x][y].chars[j]=0;
		// printf("pid_leave_room OK\n");
		}
	}
}

void init_rooms()
{int x,y,i;
for (x=0;x<MAX_X;x++)
for (y=0;y<MAX_Y;y++)
	{
	if ((x==INIT_X)&&(y==INIT_Y))
		rooms[x][y].descr=INIT_ROOM_DESCR;
	else rooms[x][y].descr=ROOM_DESCR;
	for (i=0;i<MAX_CHARS_IN_ROOM;i++)
		rooms[x][y].chars[i]=0;
	}
}

void send_pid(int pid, char *str)
{
time_t mytime;
char path[STRLEN];
FILE *f;

if (pid==0) return;

mytime = time(0);
//printf("time =%li\n", mytime);

// формирование имени файла
sprintf(path,"%s%i.%i",MESSAGE_DIR,pid,mytime);
//printf("path=%s\n", path);

if ((f=fopen(path,"a"))!=NULL)
	{
	fputs(str,f);
	fclose(f);
	}
else
	printf("send_pid() file error\n");

}

void send_all(char *str)
{int i;
for (i=0;i<MAX_CHARS;i++)
	if (chars[i].pid)
		send_pid(chars[i].pid,str);
}

int gamers_count()
{int i; int j;
j=0;
for (i=0;i<MAX_CHARS;i++)
	if (chars[i].pid)
		j++;
return j;
}

int vnum_by_pid(int pid)
{int i;
if (pid==0) return -1;
for (i=0;i<MAX_CHARS;i++)
	if (chars[i].pid==pid)
		return (i);
return -1;
}

char *name_by_pid(int pid)
{int i;
for (i=0;i<MAX_CHARS;i++)
	if (chars[i].pid==pid)
		return (chars[i].name);
return ("Anonymous");
}

int pid_by_name(char *name)
{int i;
if (name==0) return 0;
if (name[0]==0) return 0;
for (i=0;i<MAX_CHARS;i++)
	if (!strcmp(chars[i].name,name))
		return chars[i].pid;
return 0;
}

int moving (int pid, int direct)
	{int j, vnum, oldx, oldy, newx, newy;
	char buf[STRLEN];

	if ((vnum=vnum_by_pid(pid))==-1) return 3;
#define NO_WAY "Вы не можете туда пройти, там конец Мира\n"
	oldx=chars[vnum].x; oldy=chars[vnum].y;
	newx=oldx; newy=oldy;

	switch(direct)
	{
	case NORTH: if (oldx==0) {send_pid(pid,NO_WAY); return 1;} else newx--; break;
	case EAST:  if (oldy>=(MAX_Y-1)) {send_pid(pid,NO_WAY); return 1;} else newy++; break;
	case SOUTH: if (oldx>=(MAX_X-1)) {send_pid(pid,NO_WAY); return 1;} else newx++; break;
	case WEST:  if (oldy==0) {send_pid(pid,NO_WAY); return 1;} else newy--; break;
	case UP:             
	case DOWN:  send_pid(pid,"Вы не можете туда пройти, Мир пока плоский\n"); return 5;
	default: return 4;           
	}

	if ((j=room_char_free_slot(newx, newy))==-1)
		{
		send_pid(pid,"Вы не можете туда пройти, там занято\n");
		return 2;
		}
	// удаляем из старой комн.
	pid_leave_room(pid);
	// сообщения: в старую "ушел"
	sprintf(buf,"Игрок %s ушел %s\n", name_by_pid(pid), kuda_ushel(direct));
	send_room(oldx,oldy,buf);
	// сообщения: в новую "пришел"
	sprintf(buf,"Игрок %s пришел %s\n", name_by_pid(pid), otkuda_prishel(direct));
	send_room(newx,newy,buf);
	// помещаем в новую комн.
        chars[vnum].x=newx;
        chars[vnum].y=newy;
        rooms[newx][newy].chars[j]=pid;
	// сообщение игроку "вы пошли на восток" и look новой комнаты
	sprintf(buf, "Вы пошли %s\n", kuda_ushel(direct));
	send_pid(pid, buf);
	server_cmd(pid, "LOOK", "", ""); // а это уже наглость, именумая рекурсией
	return 0;
	}

void server_cmd(int pid, char *str, char *p1, char *p2)
{char *pp; int i, j, x, y, vnum, newx, newy, oldx, oldy;
char buf[STRLEN];

if((pp=strchr(str,'\n'))!=NULL) *pp=0;
if((pp=strchr(p1,'\n'))!=NULL) *pp=0;
if((pp=strchr(p2,'\n'))!=NULL) *pp=0;

if (!strcmp(str,"LOGON"))
	{
	printf("%s logon %s ", ptime(), p1);
	for (i=1; i<MAX_CHARS;i++)
		{
		if (chars[i].pid==0)
			{
			chars[i].pid=pid;
			strncpy(chars[i].name,p1,MAX_NAME_LEN);
			if ((j=room_char_free_slot(INIT_X, INIT_Y))!=-1)
				{
				chars[i].x=INIT_X;
				chars[i].y=INIT_Y;
				rooms[INIT_X][INIT_Y].chars[j]=pid;
				printf(" OK i=%i\n",i);
				sprintf(buf,
				"Сообщение сервера: %s logon OK. Сейчас на сервере игроков в онлайне %i\n",
				chars[i].name, gamers_count());
				send_pid(pid,buf);
				sprintf(buf,"Игрок %s вошел в игру\n", chars[i].name);
				send_all(buf);
				return;
				}
			printf(" ROOM OVERFLOW\n");
			send_pid(pid,"Начальная комната переполнена игроками! Зайдите позже, пожалуйста, администрация сервера будет извещена о переполнении и примет меры в ближайшее время\n");
			return;
			}
		}
	printf(" OVERFLOW\n");
	send_pid(pid,"Сервер переполнен игроками! Зайдите позже, пожалуйста, администрация сервера будет извещена о переполнении и примет меры в ближайшее время\n");
	}
else if (!strcmp(str,"east")) moving(pid, EAST);
else if (!strcmp(str,"west")) moving(pid, WEST);
else if (!strcmp(str,"north")) moving(pid, NORTH);
else if (!strcmp(str,"south")) moving(pid, SOUTH);
else if (!strcmp(str,"up")) moving(pid, UP);
else if (!strcmp(str,"down")) moving(pid, DOWN);
else if (!strcmp(str,"PING"))
	{
	send_pid(pid,"Pong!\n");
	}
else if (!strcmp(str,"IP"))
	{
	if(pp=strchr(p1,' ')) *pp=0;
	printf("IP %s %s\n", p1, nslookup(p1));
	}
else if (!strcmp(str,"TELL"))
	{
	printf("TELL `%s' `%s'\n", p1, p2);
	i=pid_by_name(p1);
	printf("pid=%i\n", i);
	if (i==0)
		{
		sprintf(buf,"Вы попытались сказать '%s' отсутствующему игроку %s\n",p2,p1);
		send_pid(pid, buf);
		}
	else
		{
		sprintf(buf,"Игрок %s сказал Вам: '%s'\n",name_by_pid(pid),p2);
		send_pid(i, buf);
		sprintf(buf,"Вы сказали '%s' игроку %s\n",p2,name_by_pid(i));
		send_pid(pid, buf);
		}
	}
else if (!strcmp(str,"GOSSIP"))
	{
	sprintf(buf,"Игрок %s сказал всем: '%s'\n",name_by_pid(pid),p1);
	send_all(buf);
	}
else if (!strcmp(str,"SAY"))
	{
	if((vnum=vnum_by_pid(pid))!=-1)
		{
		sprintf(buf,"Игрок %s сказал: '%s'\n",name_by_pid(pid),p1);
		send_room(chars[vnum].x,chars[vnum].y,buf);
		}
	}
else if (!strcmp(str,"WHO"))
	{
	send_pid(pid, "Who online\n");
	for (i=0;i<MAX_CHARS;i++)
		if (chars[i].pid)
			{
			send_pid(pid,chars[i].name);
			send_pid(pid,"\n");
			}
	}
else if (!strcmp(str,"VER"))
	{
#define S_VER "Server compilation time %s %s\n"
	sprintf(buf,S_VER, __DATE__, __TIME__);
	send_pid(pid,buf);
	}
else if (!strcmp(str,"LOOK"))
	{int x,y;
	if((j=vnum_by_pid(pid))==-1)
		{
		sprintf(buf,"Вы призрак какой-то и находитесь нигде");
		send_pid(pid,buf);
		}
	else
		{
		x=chars[j].x;
		y=chars[j].y;
		sprintf(buf,"Room [%i,%i]\n\n%s",x, y, rooms[x][y].descr);
		send_pid(pid,buf);
		// вывод списка находящихся в комнате игроков
		for (j=0;j<MAX_CHARS_IN_ROOM;j++)
			{
			if (rooms[x][y].chars[j])
				if (rooms[x][y].chars[j]!=pid)
					{
					sprintf(buf,"\nИгрок %s находится тут",name_by_pid(rooms[x][y].chars[j]));
					send_pid(pid,buf);
					}
			}
		}
	}
else if (!strcmp(str,"EXIT"))
	{
	send_pid(pid,"Вы вышли\n!!EXIT");
	for (i=0;i<MAX_CHARS;i++) 
		{
		if (chars[i].pid==pid)
			{
			printf("%s User %s logoff\n", ptime(), chars[i].name);
			sprintf(buf,"Игрок %s вышел из игры\n", chars[i].name);
			pid_leave_room(pid);
			chars[i].pid=0;
			send_all(buf);
			}
		}
	}
else
	printf("unknown command pid=%i cmd=`%s' p1=`%s' p2=`%s'\n", pid, str, p1, p2);
}

int room_char_free_slot(int x, int y)
{int i;
if ((x<0)||(x>MAX_X)||(y<0)||(y>MAX_Y)) return -1;
for (i=0;i<MAX_CHARS_IN_ROOM;i++)
	{
	if (rooms[x][y].chars[i]==0) return i;
	}
return -1;
}

// ======================= SERVER MAIN() ===================================

int main()
{char path[BUFFERSIZE];
char str[STRLEN];
char p1[STRLEN];
char p2[STRLEN];
FILE *f, *fcmd;
DIR *dd;
struct dirent *nextfile;
int pid;
int i;
int tick_counter;

printf("\nBBS SERVER started\n");
printf("\n%s BBS SERVER started \n",ptime());
printf("Author: Prool. proolix@gmail.com http://prool.kharkov.org http://mud.kharkov.org\n");
printf(S_VER, __DATE__, __TIME__);
printf("For enter to BBSMUD use 'bbs' program. For stop server use control C or kill -1\n\n");
#if 1
printf("koi: тест русских\n\n");
printf("utf: я┌п╣я│я│я│я│ я┌п╣я│я┌");
printf("\n\n");
#endif


// инициалиация структуры игроков

for (i=0;i<MAX_CHARS;i++)
	{
	chars[i].pid=0;
	}

// инициализация комнат
init_rooms();

// очистка каталогов cmd и messages

mkdir("cmd",0777);
mkdir("messages",0777);
		if ((dd=opendir(MESSAGE_DIR))!=NULL)
		while((nextfile=readdir(dd))!=NULL)
			{char path[BUFFERSIZE];
			if (nextfile->d_name[0]=='.') continue;
			strcpy(path,MESSAGE_DIR);
			strcat(path,nextfile->d_name);
			// printf("%s\n", path);
			if (unlink(path)) printf("unlink error\n");
			}
		closedir(dd);

		if ((dd=opendir(CMD_DIR))!=NULL)
		while((nextfile=readdir(dd))!=NULL)
			{char path[BUFFERSIZE];
			if (nextfile->d_name[0]=='.') continue;
			strcpy(path,CMD_DIR);
			strcat(path,nextfile->d_name);
			// printf("%s\n", path);
			if (unlink(path)) printf("unlink error\n");
			}
		closedir(dd);

tick_counter=0;

while(1)
	{
		if ((dd=opendir(CMD_DIR))!=NULL)
		while((nextfile=readdir(dd))!=NULL)
			{char path[BUFFERSIZE];
			if (nextfile->d_name[0]=='.') continue;
			strcpy(path,CMD_DIR);
			strcat(path,nextfile->d_name);
			// printf("%s\n", path);

			if ((fcmd=fopen(path,"r"))!=NULL)
				{
				str[0]=0; p1[0]=0; p2[0]=0;
				if (fgets(str,STRLEN,fcmd))
					if (fgets(p1,STRLEN,fcmd))
						fgets(p2,STRLEN,fcmd);
				pid=atoi(path+strlen(CMD_DIR));
				server_cmd(pid, str, p1, p2);
				}
			if (unlink(path)) printf("unlink error\n");
			}
		closedir(dd);

	// printf("Tick %i microsec\n\n",TICK);
	usleep(TICK);
	tick_counter++;
	if (tick_counter%120==0)
		{
		send_all("Прошла минута\n");
		}
	}
}
