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

	SOCKET soc = socket(AF_INET, SOCK_DGRAM, 0);
	if (soc == INVALID_SOCKET)
	{
		cout << "ERROR: Can't create socket\n";
		WSACleanup();
		return 0;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(34505);
	hint.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(soc, (sockaddr*)&hint, sizeof(hint)) < 0)
	{
		cout << "ERROR: Bind failed\n";
		WSACleanup();
		closesocket(soc);
		return 0;
	}

	cout << "SUCCESS: Server created\n";

	char buf[1000];
	ZeroMemory(buf, 1000);

	int l;
	l = sizeof(hint);

	while (recvfrom(soc, buf, 1000, 0, (sockaddr*)&hint, &l) != 0)
	{
		sendto(soc, changeStr(buf), 1000, 0, (sockaddr*)&hint, l);
		ZeroMemory(buf, 1000);
	}


	WSACleanup();
}

char* changeStr(char* str)
{
	char buf;
	int length = strlen(str);
	int counter = 0;
	for (int i = 0; i < length; i++)
	{
		if (str[i] == '@')
		{
			counter++;
			if (counter == 2)
			{
				counter = 0;
				str[i] = '#';
			}
		}
	}
	str[length] = '\n';
	return str;
}