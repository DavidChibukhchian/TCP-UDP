#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("ERROR: Wrong number of arguments. Run %s [TCP_server|TCP_client|UDP_server|UDP_client]\n", argv[0]);
		return -1;
	}
	/*
	if (strcmp(argv[1],      "TCP_server") == 0)
	{
		TCP_server();
	}
	else if (strcmp(argv[1], "TCP_client") == 0)
	{
		TCP_client();
	}
	else if (strcmp(argv[1], "UDP_server") == 0)
	{
		UDP_server();
	}
	else if (strcmp(argv[1], "UDP_client") == 0)
	{
		UDP_client();
	}
	else
	{
		printf("ERROR: Wrong mode. Run %s [TCP_server|TCP_client|UDP_server|UDP_client]\n", argv[0]);
		return -2;
	}
	*/

	return 0;
}
