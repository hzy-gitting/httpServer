#pragma once
#include<WinSock2.h>
struct HTTPHeader {
	char* key;
	char* value;
	struct HTTPHeader* next;
};

struct HTTPRequestPacket {
	char* method;
	char* uri;
	char* version;
	struct HTTPHeader* headers;
	char* body;
	int bodyLength;
};

struct HTTPResponsePacket {
	char* version;
	char* statusCode;
	char* reasonPhrase;
	struct HTTPHeader* headers;
	char* body;
	int bodyLength;
};

int recvHTTPRequestPacket(SOCKET sock, HTTPRequestPacket* request);

HTTPHeader* mallocHTTPHeaderNode();

HTTPHeader* createHTTPHeader(const char* key, const char* value);

void insertHTTPHeader(HTTPHeader** ppHead, HTTPHeader* s);

void addResponseHeader(HTTPResponsePacket* response, const char* key, const char* value);

int initHTTPRequestPacket(HTTPRequestPacket* request);

void destroyHTTPRequestPacket(HTTPRequestPacket* request);

int initHTTPResponsePacket(HTTPResponsePacket* response);

void destroyHTTPResponsePacket(HTTPResponsePacket* response);
