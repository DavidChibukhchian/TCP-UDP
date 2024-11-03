#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "TCP.h"

//--------------------------------------------------------------------------------------------------------------------------

#define MAX_MESSAGE_SIZE 40960

//--------------------------------------------------------------------------------------------------------------------------

int TCP_server(int port)
{
	struct sockaddr_in server_addr = {};
	struct sockaddr_in client_addr = {};
	socklen_t client_addr_len = sizeof(client_addr);
	char message[MAX_MESSAGE_SIZE] = {'\0'};

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	listen(server_fd, 1);
	printf("Server was successfully launched on port %d.\n\n", port);

	int client_fd = -1;
	while (1)
	{
		if (client_fd == -1)
		{
			printf("Waiting for a client connection...\n");
			client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
			printf("Сlient successfully connected.\n\n");
		}

		while (client_fd != -1)
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
				int bytes_received = recv(client_fd, message, MAX_MESSAGE_SIZE, 0);
				if (bytes_received <= 0)
				{
					printf("\nСlient successfully disconnected.\n");
					close(client_fd);
					client_fd = -1;
					break;
				}

				message[bytes_received] = '\0';
				printf("Client: %s\n", message);
			}

			if (FD_ISSET(STDIN_FILENO, &read_fds))
			{
				fgets(message, MAX_MESSAGE_SIZE, stdin);
				message[strcspn(message, "\n")] = '\0';

				if (strcmp(message, "server_exit") == 0)
				{
					printf("Server was successfully shut down.\n");
					close(client_fd);
					close(server_fd);

					return 0;
				}

				send(client_fd, message, strlen(message), 0);
			}
		}
	}
}


//--------------------------------------------------------------------------------------------------------------------------

int TCP_client(const char* address, int port)
{
	struct sockaddr_in server_addr = {};
	char message[MAX_MESSAGE_SIZE] = {'\0'};

	int client_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0)
	{
		printf("ERROR: Invalid address\n");
		close(client_fd);
		return -1;
	}

	if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("ERROR: Connection error\n");
		close(client_fd);
		return -2;
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
			int bytes_received = recv(client_fd, message, MAX_MESSAGE_SIZE, 0);
			if (bytes_received <= 0)
			{
				printf("\nServer has ended the connection.\n");
				close(client_fd);
				return 0;
			}

			message[bytes_received] = '\0';
			printf("Server: %s\n", message);
		}

		if (FD_ISSET(STDIN_FILENO, &read_fds))
		{
			fgets(message, MAX_MESSAGE_SIZE, stdin);
			message[strcspn(message, "\n")] = '\0';

			if (strcmp(message, "exit") == 0)
			{
				printf("You have been successfully disconnected from the server.\n");
				close(client_fd);
				return 0;
			}

			send(client_fd, message, strlen(message), 0);
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------
