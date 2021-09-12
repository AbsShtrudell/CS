#include <iostream>
#include <WS2tcpip.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) 

using namespace std;

char* changeStr(char* str);

int main()
{
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	if (WSAStartup(ver, &wsData) != 0)
	{
		cout << "ERROR: Can't initialize winsock\n";
		return 0;
	}

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cout << "ERROR: Can't create socket\n";
		WSACleanup();
		return 0;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(34505);
	hint.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(listening, (sockaddr*)&hint, sizeof(hint)) < 0)
	{
		cout << "ERROR: Bind failed\n";
		WSACleanup();
		closesocket(listening);
		return 0;
	}

	cout << "SUCCESS: Server created\n";

	listen(listening, SOMAXCONN);

	SOCKET client = accept(listening, NULL, NULL);
	cout << "NEW CLIENT CONNECTED\n";

	char buf[1000];
	ZeroMemory(buf, 1000);

	while (recv(client, buf, sizeof(buf), 0) != 0)
	{
		send(client, changeStr(buf), sizeof(buf), 0);
		ZeroMemory(buf, 1000);
	}


	WSACleanup();
}

char* changeStr(char* str)
{
	char buf;
	int length = strlen(str);
	for (int i = 0; i < length - 1; i++)
		for (int j = 0; j < length - i - 1; j++)
			if (str[j] > str[j + 1])
			{
				buf = str[j];
				str[j] = str[j + 1];
				str[j + 1] = buf;
			}
	str[length] = '\n';
	return str;
}