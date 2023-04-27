#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 54322
#define MAX_LINE 256

int main(int argc, char *argv[])
{
	FILE *fp;
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host_ip;
	char buf[MAX_LINE];
	int s;
	int s_addr;
	int len;
	if (argc == 2)
	{
		host_ip = argv[1];
		s_addr = inet_addr(host_ip);
		if (s_addr == INADDR_NONE)
		{
			fprintf(stderr, "simplex-talk: invalid host: %s\n", host_ip);
			exit(1);
		}
	}
	else
	{
		fprintf(stderr, "usage: simplex-talk host_ip\n");
		exit(1);
	}
	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	// bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_addr.s_addr = s_addr;
	sin.sin_port = htons(SERVER_PORT);
	/* active open */
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("simplex-talk: socket");
		exit(1);
	}
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		perror("simplex-talk: connect");
		close(s);
		exit(1);
	}
	/* main loop: get and send lines of text */
	while (fgets(buf, sizeof(buf), stdin))
	{
		buf[MAX_LINE - 1] = '\0';
		len = strlen(buf) + 1;
		send(s, buf, len, 0);
		recv(s, buf, sizeof(buf), 0);
		fputs(buf, stdout);
	}
}