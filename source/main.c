#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TCP.h"
#include "UDP.h"

//--------------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	int err = 0;

	if (!((argc == 3) || (argc == 4)))
	{
		printf("ERROR: Wrong number of arguments.\n");
		printf("Run %s <mode> <port>\n", argv[0]);
		printf("modes:\n");
		printf("\tTCP_server\n");
		printf("\tTCP_client <address>\n");
		printf("\tUDP_server\n");
		printf("\tUDP_client <address>\n");

		return -1;
	}

	int port = -1;
	const char* address = NULL;
	if (argc == 3)
	{
		port = atoi(argv[2]);
	}
	else
	{
		port = atoi(argv[3]);
		address = argv[2];
		
	}
	
	if (strcmp(argv[1],      "TCP_server") == 0)
	{
		err = TCP_server(port);
	}
	else if (strcmp(argv[1], "TCP_client") == 0)
	{
		err = TCP_client(address, port);
	}
	else if (strcmp(argv[1], "UDP_server") == 0)
	{
		err = UDP_server(port);
	}
	else if (strcmp(argv[1], "UDP_client") == 0)
	{
		err = UDP_client(address, port);
	}
	else
	{
		printf("ERROR: Wrong mode.\n");
		printf("Run %s <mode> <port>\n", argv[0]);
		printf("modes:\n");
		printf("\tTCP_server\n");
		printf("\tTCP_client <address>\n");
		printf("\tUDP_server\n");
		printf("\tUDP_client <address>\n");
		
		return -2;
	}

	if (err) return err;

	return 0;
}

//--------------------------------------------------------------------------------------------------------------------------
