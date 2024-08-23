#pragma once
#include<WinSock2.h>
struct HTTPRequestPacket {
	char* method;
	char* uri;
	char* version;
	struct HTTPHeader* headers;
	char* body;
	int bodyLength;
};

enum {
	PS_METHOD,
	PS_URI,
	PS_VERSION,
	PS_HEADER_KEY,
	PS_HEADER_VALUE,
	PS_BODY
};
struct ParseContext {
	int parseState;
	int methodLen;
	int uriLen;
	int versionLen;
	struct HTTPHeader* currentHeader;
	int keyLen;
	int valueLen;
	int parsingSpace;
	int bodyLen;
};


#define PSCONTINUE (0)
#define PSERROR (-1)
#define PSEND (1)

int initHTTPRequestPacket(HTTPRequestPacket* request);

void destroyHTTPRequestPacket(HTTPRequestPacket* request);

int recvHTTPRequestPacket(SOCKET sock, HTTPRequestPacket* request);
