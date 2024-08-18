
#define  _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <iostream>
#include<WinSock2.h>
#include<process.h>
#include"connectionHandler.h"
#include"http.h"

#define TEST 0
#if !TEST
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
		printf("le = %d\n", err);
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
		_beginthreadex(NULL, 0, threadMain, (void*)acceptSock, 0, &threadId);
		printf("begin thread tid=%d\n", threadId);
	}
	ret = WSACleanup();
	if (ret != 0) {
		printf("WSACleanup fail,le = %d\n", WSAGetLastError());
		return -1;
	}

	return 0;
}
#else
int main() {
	const char* httpRequestString = "GET / HTTP/1.1\r\nHost: localhost:27015\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nsec-ch-ua: \"Chromium\";v=\"122\", \"Not(A:Brand\";v=\"24\", \"Microsoft Edge\";v=\"122\"\r\nContent-Length: 5\r\n\r\nhello";
	struct HTTPRequestPacket request;
	int ret = initHTTPRequestPacket(&request);
	if (ret != 0) {
		printf("initHTTPRequestPacket fail,ret=%d\n", ret);
		return -1;
	}
	ret = parseBufToRequestPacket(httpRequestString, strlen(httpRequestString),&request);

	return 0;
}
#endif