#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT 54321
#define MAX_LINE 32 * 1000 // 32KB

int main()
{
  struct sockaddr_in sin, client;
  char buf[MAX_LINE];
  int buf_len;
  int s;
  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(SERVER_PORT);
  // addr_len = sizeof(sin);
  /* setup passive open */
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("simplex-talk: socket");
    exit(1);
  }
  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0)
  {
    perror("simplex-talk: bind");
    exit(1);
  }
  /* wait for datagram, then send received text back */
  while ((buf_len = recvfrom(
              s, buf, sizeof(buf), MSG_WAITALL, (struct sockaddr *)&client,
              (socklen_t *)sizeof(client))))
  {
    fputs(buf, stdout);

    sprintf(buf, "echo");
    buf[MAX_LINE - 1] = '\0';
    buf_len = strlen(buf) + 1;

    printf("about to send \n");

    sendto(s, buf, buf_len, 0, (struct sockaddr *)&client, sizeof(client));

    printf("sent \n");
  }
}