#include <iostream>
#include <WS2tcpip.h>
#include <string.h>
#include <string>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) 

using namespace std;

DWORD WINAPI ThreadFunc(LPVOID client);

struct part
{
	string factory;
	float price;
	string date;
};

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

	part a = { "factory1\0", 10.2, "01.12.2021" };
	part b = { "factory1\0", 10.8, "01.09.2021" };
	part c = { "factory2\0", 5.8, "07.09.2020" };
	part n = { "factory7\0", 5.8, "07.09.2010" };
	part d = { "factory4\0", 6.1, "10.07.2020" };
	part t = { "factory5\0", 6.1, "10.07.2021" };
	part u = { "factory5\0", 50, "01.09.1939" };

	FILE* f;
	f = fopen("data.txt", "w");
	fwrite(&a, sizeof(part), 1, f);
	fwrite(&b, sizeof(part), 1, f);
	fwrite(&c, sizeof(part), 1, f);
	fwrite(&n, sizeof(part), 1, f);
	fwrite(&d, sizeof(part), 1, f);
	fwrite(&t, sizeof(part), 1, f);
	fwrite(&u, sizeof(part), 1, f);
	fclose(f);

	cout << "SUCCESS: Server created\n";

	listen(listening, SOMAXCONN);

	SOCKET client; 

	while (client = accept(listening, NULL, NULL))
	{
		DWORD thID;
		CreateThread(NULL, NULL, ThreadFunc, &client, NULL, &thID);
	}

	WSACleanup();
}

DWORD WINAPI ThreadFunc(LPVOID client)
{
	part q;
	SOCKET s = ((SOCKET*)client)[0];
	cout << "NEW CLIENT CONNECTED\n";

	char buf[1000];
	bool result = false;
	ZeroMemory(buf, 1000);

	FILE* f;
	f = fopen("data.txt", "r");

	while (recv(s, buf, sizeof(buf), 0) != 0)
	{
		result = false;
		while (fread(&q, sizeof(part), 1, f))
		{
			if (!strcmp(q.date.c_str(), buf))
			{
				strcpy(buf, (q.factory+" ").c_str());
				string d = to_string(q.price);
				strcat(buf, d.c_str());
				buf[q.factory.length() + d.length()] = '\0';
				send(s,buf, sizeof(buf), 0);
				ZeroMemory(buf, 1000);
				result = true;
			}
		}
		if (!result)
			send(s, "Vrong value", sizeof("Vrong value"), 0);
	}
	fclose(f);
	closesocket(s);
	return 0;
}