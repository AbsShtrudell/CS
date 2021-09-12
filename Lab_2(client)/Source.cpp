#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <conio.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
	string ipAddress;
	int port = 34505;
	char input;

	cout << "Enter ip address ";
	getline(cin, ipAddress);

	cout << "Do you want to change port? [Y/N]\n";
	input = _getch();
	switch (input)
	{
	case 'y':case'Y':
		cout << "Enter port ";
		cin >> port;
	}

	system("CLS");

	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cout << "ERROR: Can't initialize winsock\n";
		_getch();
		return 0;
	}

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cout << "ERROR: Can't create socket\n";
		WSACleanup();
		_getch();
		return 0;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr.S_un.S_addr);

	char buf[1000];
	ZeroMemory(buf, 1000);

	int l;
	l = sizeof(hint);

	while (true)
	{
		cout << "> ";
		cin.getline(buf, sizeof(buf));
		int sendResult = sendto(sock, buf, sizeof(buf), 0, (sockaddr*)&hint, l);
		if (sendResult == SOCKET_ERROR)
			cout << "ERROR: Failed to send messege\n";

		ZeroMemory(buf, 1000);

		int bytesRecived = recvfrom(sock, buf, 1000, 0, (sockaddr*)&hint, &l);
		if (bytesRecived > 0)
		{
			cout << "SERVER> ";
			for (int i = 0; buf[i] != '\n'; i++)
			{
				cout << buf[i];
			}
			cout << endl;
		}

		ZeroMemory(buf, 1000);
	}

	closesocket(sock);
	WSACleanup();

}