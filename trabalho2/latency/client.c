#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define MAXLINE 1000 + 1 // 1KB + string terminator

// Driver code
int main(int argc, char *argv[])
{
	char *host_ip;
	int s_addr;
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr;

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

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	bzero((char *)&servaddr, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = s_addr;
	servaddr.sin_port = htons(PORT);

	int n;
	socklen_t len;

	int messageSizes[] = {1, 1, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
	int messageSize;

	for (int size_idx = 1; size_idx < 12; size_idx++)
	{
		messageSize = messageSizes[size_idx];
		printf("Message size: %d bytes\n", messageSize);

		memset(buffer, 'A', messageSize);
		buffer[messageSize] = '\0';

		double times_spent[2];
		for (int i = 0; i < 3; i++)
		{
			clock_t start = clock();
			for (int j = 0; j < 100000; j++)
			{
				sendto(sockfd, (const char *)buffer, strlen(buffer),
					   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
					   sizeof(servaddr));

				n = recvfrom(sockfd, (char *)buffer, MAXLINE,
							 MSG_WAITALL, (struct sockaddr *)&servaddr,
							 &len);
				buffer[n] = '\0';
			}
			clock_t end = clock();
			times_spent[i] = (double)(end - start) / CLOCKS_PER_SEC;
			printf("\t[%d] Time spent: %fs\n", i + 1, times_spent[i]);
		}

		double avg_time_spent = (times_spent[0] + times_spent[1] + times_spent[2]) / 3;

		printf("\t\tAverage time spent: %fs\n", avg_time_spent);
	}

	close(sockfd);
	return 0;
}