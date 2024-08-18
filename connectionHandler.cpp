#include"http.h"
#include<WinSock2.h>
#include<stdio.h>
#include "connectionHandler.h"


void processRequest(struct HTTPRequestPacket *request,struct HTTPResponsePacket *response) {
	addResponseHeader(response, "Server", "CServer");
	response->body = (char*)malloc(6);
	response->bodyLength = 6;
	memcpy(response->body, "hello!", 6);
}

unsigned __stdcall threadMain(void* arg) {
	int ret;
	SOCKET sock = (SOCKET)arg;
	struct HTTPRequestPacket request;
	struct HTTPResponsePacket response;
	ret = initHTTPRequestPacket(&request);
	if (ret != 0) {
		printf("initHTTPRequestPacket fail,ret=%d\n", ret);
		return -1;
	}
	ret = recvHTTPRequestPacket(sock, &request);
	if (ret != 0) {
		printf("recvHTTPRequestPacket fail,ret=%d", ret);
		return -1;
	}
	ret = initHTTPResponsePacket(&response);
	if (ret != 0) {
		printf("initHTTPResponsePacket fail,ret = %d\n", ret);
		return -1;
	}
	
	processRequest(&request, &response);
	ret = send(sock, "this is server.", 16, 0);
	if (ret == SOCKET_ERROR) {
		printf("send fail\n");
	}
	closesocket(sock);
	destroyHTTPRequestPacket(&request);
	return 1;
}
