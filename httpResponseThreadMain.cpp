
#include<WinSock2.h>
#include<stdio.h>
#include "httpResponseThreadMain.h"
#include"HTTPRequest.h"
#include"HTTPResponse.h"

void processRequest(struct HTTPRequestPacket *request,struct HTTPResponsePacket *response) {
	addResponseHeader(response, "Server", "CServer");
	response->body = (char*)malloc(6);
	response->bodyLength = 6;
	memcpy(response->body, "hello!", 6);
}

unsigned __stdcall httpResponseThreadMain(void* arg) {
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
	char* buf = (char*)malloc(1024 * 4);
	if (buf == NULL) {
		printf("malloc fail\n");
		return -1;
	}
	int len = 0;
	serializeHTTPResponsePacket(&response, buf, &len);
	ret = send(sock, buf, len, 0);
	if (ret == SOCKET_ERROR) {
		printf("send fail\n");
	}
	closesocket(sock);
	destroyHTTPResponsePacket(&response);
	destroyHTTPRequestPacket(&request);
	free(buf);
	return 1;
}
