#include "common.h"

char *ptime(void) // Возвращаемое значение: ссылка на текстовую строку с текущим временем
	{
	char *tmstr;
	time_t mytime;

	mytime = time(0);

	tmstr = (char *) asctime(localtime(&mytime));
	*(tmstr + strlen(tmstr) - 1) = '\0';

	return tmstr;

	}


char *nslookup(const char *ip)
// example: input "217.12.192.65", returned output "www.itl.ua"
{
struct in_addr ip0;
struct hostent *hp;

if (!inet_aton(ip, &ip0))
	/* printf("can't parse IP address %s", ip) */;

if ((hp = gethostbyaddr(&ip0, sizeof ip0, AF_INET)) == NULL)
	/* printf("no name associated with %s", ip) */;

// printf("name associated with %s is %s\n", ip, hp->h_name);

return (hp!=NULL)?hp->h_name:"*";
}
