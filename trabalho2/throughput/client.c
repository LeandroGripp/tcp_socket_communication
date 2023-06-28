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
#define MAXLINE 32 * 1000 + 1 // 32KB + string terminator

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

	for (int message_size = 1; message_size < 33; message_size++)
	{
		// long long totalBits = message_size * 1024 * 100000 * 8;

		printf("Message size: %d KB\n", message_size);

		memset(buffer, 'A', message_size * 1000);
		buffer[message_size * 1000] = '\0';

		double times_spent[2];
		// double throughputs[2];
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
			// throughputs[i] = (double)100000 * 1024 * message_size / times_spent[i] / 1000000;
		}
		double throughput1, throughput2, throughput3;

		throughput1 = (double)100000 * 1024 * message_size / times_spent[0] / 1000000;
		printf("\t[1] Time spent: %.3fs | Throughput: %.3f Mbps\n", times_spent[0], throughput1);

		throughput2 = (double)100000 * 1024 * message_size / times_spent[1] / 1000000;
		printf("\t[2] Time spent: %.3fs | Throughput: %.3f Mbps\n", times_spent[1], throughput2);

		throughput3 = (double)100000 * 1024 * message_size / times_spent[2] / 1000000;
		printf("\t[3] Time spent: %.3fs | Throughput: %.3f Mbps\n", times_spent[2], throughput3);

		double avg_time_spent = (times_spent[0] + times_spent[1] + times_spent[2]) / 3;
		double avg_throughput = (throughput1 + throughput2 + throughput3) / 3;

		printf("\t\tAverage time spent: %.3fs | Average throughput: %.3f Mbps\n", avg_time_spent, avg_throughput);
	}

	close(sockfd);
	return 0;
}