
#include<WinSock2.h>
#include<stdio.h>
#include<process.h>
#include "httpResponseThreadMain.h"
#include "processIncomingConnectionThreadMain.h"

unsigned __stdcall processIncomingConnectionThreadMain(void* arg)
{
	SOCKET sock = (SOCKET)arg;
	while (1) {
		printf("waiting for incoming connection...\n");
		SOCKET acceptSock = accept(sock, NULL, NULL);
		if (acceptSock == INVALID_SOCKET) {
			printf("Accept fail,le=%d\n", WSAGetLastError());
			return 1;
		}
		unsigned int threadId;
		_beginthreadex(NULL, 0, httpResponseThreadMain, (void*)acceptSock, 0, &threadId);
		printf("begin thread tid=%d\n", threadId);
	}
	return 0;
}
