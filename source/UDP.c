#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "UDP.h"

//--------------------------------------------------------------------------------------------------------------------------

#define MAX_MESSAGE_SIZE 40960

//--------------------------------------------------------------------------------------------------------------------------

int UDP_server(int port)
{
	struct sockaddr_in server_addr = {};
	struct sockaddr_in client_addr = {};
	socklen_t client_addr_len = sizeof(client_addr);
	char message[MAX_MESSAGE_SIZE] = {'\0'};

	int server_fd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	printf("Server was successfully launched on port %d.\n\n", port);

	while (1)
	{
		int max_fd = server_fd;
		if (STDIN_FILENO > max_fd)
		{
			max_fd = STDIN_FILENO;
		}

		fd_set read_fds = {};
		FD_ZERO(&read_fds);
		FD_SET(server_fd,    &read_fds);
		FD_SET(STDIN_FILENO, &read_fds);

		select(max_fd + 1, &read_fds, NULL, NULL, NULL);

		if (FD_ISSET(server_fd, &read_fds))
		{
			int bytes_received = recvfrom(server_fd, message, MAX_MESSAGE_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);

			message[bytes_received] = '\0';
			printf("Client: %s\n", message);

			if (strcmp(message, "exit") == 0)
			{
				printf("\nСlient successfully disconnected.\n\n");
				continue;
			}

			sendto(server_fd, message, bytes_received, 0, (struct sockaddr *)&client_addr, client_addr_len);
		}

		if (FD_ISSET(STDIN_FILENO, &read_fds))
		{
			fgets(message, MAX_MESSAGE_SIZE, stdin);
			message[strcspn(message, "\n")] = '\0';

			if (strcmp(message, "server_exit") == 0)
			{
				printf("Server was successfully shut down.\n");
				close(server_fd);
				return 0;
			}

			sendto(server_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr, client_addr_len);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------

int UDP_client(const char* address, int port)
{
	struct sockaddr_in server_addr = {};
	socklen_t server_addr_len = sizeof(server_addr);
	char message[MAX_MESSAGE_SIZE] = {'\0'};

	int client_fd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0)
	{
		perror("ERROR: Invalid address.\n");
		close(client_fd);
		return -1;
	}

	printf("You have been successfully connected to the server.\n\n");

	while (1)
	{
		int max_fd = client_fd;
		if (STDIN_FILENO > max_fd)
		{
			max_fd = STDIN_FILENO;
		}

		fd_set read_fds = {};
		FD_ZERO(&read_fds);
		FD_SET(client_fd,    &read_fds);
		FD_SET(STDIN_FILENO, &read_fds);

		select(max_fd + 1, &read_fds, NULL, NULL, NULL);

		if (FD_ISSET(client_fd, &read_fds))
		{
			int bytes_received = recvfrom(client_fd, message, MAX_MESSAGE_SIZE, 0, (struct sockaddr *)&server_addr, &server_addr_len);
			message[bytes_received] = '\0';
			printf("Server: %s\n", message);
		}

		if (FD_ISSET(STDIN_FILENO, &read_fds))
		{
			fgets(message, MAX_MESSAGE_SIZE, stdin);
			message[strcspn(message, "\n")] = '\0';

			if (strcmp(message, "exit") == 0) {
				printf("You have been successfully disconnected from the server.\n");
				close(client_fd);
				return 0;
				break;
			}

			sendto(client_fd, message, strlen(message), 0, (struct sockaddr *)&server_addr, server_addr_len);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
