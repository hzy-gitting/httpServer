
#define  _WINSOCK_DEPRECATED_NO_WARNINGS 
#include<stdio.h>
#include<WinSock2.h>
#include<process.h>
#include"httpResponseThreadMain.h"

int main()
{
	WSADATA wsaData;
	int ret;
	if (ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		printf("WSAStartup fail ,le = %d\n", ret);
		return -1;
	}
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		int err = WSAGetLastError();
		printf("create socket fail,le = %d\n", err);
		WSACleanup();
		return -1;
	}
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(27015);
	ret = bind(sock, (sockaddr*)&service, sizeof(service));
	if (ret == SOCKET_ERROR) {
		printf("bind fail ,le = %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	ret = listen(sock, SOMAXCONN);
	if (ret == SOCKET_ERROR) {
		printf("listen fail,le=%d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	while (1) {
		printf("waiting for incoming connection...\n");
		SOCKET acceptSock = accept(sock, NULL, NULL);
		if (acceptSock == INVALID_SOCKET) {
			printf("Accept fail,le=%d\n", WSAGetLastError());
			return -1;
		}
		unsigned int threadId;
		_beginthreadex(NULL, 0, httpResponseThreadMain, (void*)acceptSock, 0, &threadId);
		printf("begin thread tid=%d\n", threadId);
	}
	ret = WSACleanup();
	if (ret != 0) {
		printf("WSACleanup fail,le = %d\n", WSAGetLastError());
		return -1;
	}

	return 0;
}
