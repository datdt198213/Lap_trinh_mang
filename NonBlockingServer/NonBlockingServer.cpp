#include<stdio.h>
#include<WinSock2.h>

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN sockaddr_in;
	sockaddr_in.sin_family = AF_INET;
	sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr_in.sin_port = htons(8000);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (sockaddr*)&sockaddr_in, sizeof(sockaddr_in));
	listen(listener, 5);

	SOCKET clients[64];
	int numberClients;

	int ret;
	char buf[256];
	while (1)
	{
		SOCKET client = accept(listener, NULL,NULL);

		if (client == INVALID_SOCKET)
		{
			ret = WSAGetLastError();
			if (ret == WSAEWOULDBLOCK)
			{
				printf("%d", ret);
			}
		}
		else
		{
			clients[numberClients] = client;
			numberClients++;
		}
	}
}