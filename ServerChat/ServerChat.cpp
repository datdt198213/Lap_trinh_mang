#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<WinSock2.h>
#include<utility>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

int numberClient = 0;
char** id_array;

std::pair<SOCKET, char*> clients[64];

DWORD WINAPI ClientThread(LPVOID lpParam);

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
	listen(listener, 2);

	while (1)
	{
		SOCKET client = accept(listener, NULL, NULL);
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}

	closesocket(listener);
	WSACleanup();
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
	SOCKET client = *(SOCKET*)lpParam;
	int ret;
	char buf[256];

	if (client == INVALID_SOCKET)
	{
		ret = GetLastError();
	}
	else
	{
		printf("New client connection: %d \n", client);
		clients[numberClient].first = client;
		clients[numberClient].second = NULL;
		numberClient++;
		char hello_msg[50] = "Enter client_id:xxxxxx to chat: ";		// create hello sentences
		send(client, hello_msg, strlen(hello_msg), 0);
	}

	while (1)
	{
		ret = recv(client, buf, sizeof(buf) - 1, 0);
		if (ret == SOCKET_ERROR)
		{
			ret = WSAGetLastError();
			for (int i = 0; i < numberClient; i++)
			{
				if (client == clients[i].first)
				{
					printf("Error with %d, error code %d, Close connection", client, ret);
					closesocket(client);
					numberClient--;
					for (int j = i; j < numberClient; j++) clients[j] = clients[j + 1];
				}
			}
		}
		else
		{
			buf[ret] = 0;

			for (int i = 0; i < numberClient; i++)
			{
				if (client == clients[i].first)
				{
					if (clients[i].second == NULL)
					{
						char str_check[50] = "client_id: ";
						if (strncmp(buf, str_check, strlen(str_check)) == 0)
						{
							char* name = new char[100];
							int name_length = strlen(buf) - strlen(str_check) - 1; // -1 de bo di ki tu xuong dong trong luc nhap du lieu vao
							memcpy(name, buf + strlen(str_check), name_length);
							name[name_length] = 0;
							clients[i].second = name;
						}
					}
					else
					{
						char mess[1000];
						strcpy(mess, clients[i].second);	
						strcat(mess, ": ");
						strcat(mess, buf);

						for (int j = 0; j < numberClient; j++)
							if (j != i) 	send(clients[j].first, mess, strlen(mess), 0);
					}
				}
			}
		}
	}
	closesocket(client);
}