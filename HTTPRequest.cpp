#include"httpHeader.h"
#include "HTTPRequest.h"
#include<stdio.h>

#define MAX_METHOD_LEN (10)
#define MAX_URI_LEN (1024 * 4)
#define MAX_VERSION_LEN (10)
int initHTTPRequestPacket(struct HTTPRequestPacket* request) {
	request->method = (char*)malloc(MAX_METHOD_LEN);
	if (!request->method) {
		return -1;
	}
	request->uri = (char*)malloc(MAX_URI_LEN);
	if (!request->uri) {
		free(request->method);
		return -1;
	}
	request->version = (char*)malloc(MAX_VERSION_LEN);
	if (!request->version) {
		free(request->method);
		free(request->uri);
		return -1;
	}
	request->headers = NULL;
	request->body = NULL;
	request->bodyLength = 0;
	return 0;
}


void destroyHTTPRequestPacket(struct HTTPRequestPacket* request) {
	free(request->method);
	free(request->uri);
	free(request->version);
	freeHTTPHeaders(request->headers);
	if (request->body) {
		free(request->body);
	}
}


static void initParseContext(struct ParseContext* parseContext) {
	parseContext->parseState = PS_METHOD;
	parseContext->methodLen = 0;
	parseContext->uriLen = 0;
	parseContext->versionLen = 0;
	parseContext->currentHeader = NULL;
	parseContext->keyLen = 0;
	parseContext->valueLen = 0;
	parseContext->parsingSpace = 1;
	parseContext->bodyLen = 0;
}
static int parseMethodOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
{
	if (ch == ' ') {
		request->method[parseCtx->methodLen] = '\0';
		parseCtx->parseState = PS_URI;
	}
	else {
		request->method[parseCtx->methodLen++] = ch;
	}
	return PSCONTINUE;
}
static int parseURIOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
{
	if (ch == ' ') {
		request->uri[parseCtx->uriLen] = '\0';
		parseCtx->parseState = PS_VERSION;
	}
	else {
		request->uri[parseCtx->uriLen++] = ch;
	}
	return PSCONTINUE;
}
static int parseVersionOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
{
	if (ch == '\r') {
		request->version[parseCtx->versionLen] = '\0';
	}
	else if (ch == '\n') {
		parseCtx->parseState = PS_HEADER_KEY;
	}
	else {
		request->version[parseCtx->versionLen++] = ch;
	}
	return PSCONTINUE;
}

static int getContentLength(struct HTTPRequestPacket* request)
{
	struct HTTPHeader* pHeader = request->headers;

	while (pHeader) {
		if (strcmp(pHeader->key, "Content-Length") == 0) {
			break;
		}
		pHeader = pHeader->next;
	}
	if (pHeader == NULL) {
		return 0;
	}
	return atoi(pHeader->value);
}

static int parseHeaderKeyOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
{
	if (ch == '\r') {

	}
	else if (ch == '\n') {
		int contentLength = getContentLength(request);
		if (contentLength == 0) {
			return PSEND;
		}
		request->body = (char*)malloc(contentLength);
		if (request->body == NULL) {
			return PSERROR;
		}
		request->bodyLength = contentLength;
		parseCtx->parseState = PS_BODY;
	}
	else if (ch != ':') {
		if (parseCtx->currentHeader == NULL) {
			parseCtx->currentHeader = mallocHTTPHeaderNode();
			if (!parseCtx->currentHeader) {
				return PSERROR;
			}
		}
		parseCtx->currentHeader->key[parseCtx->keyLen++] = ch;
	}
	else {
		parseCtx->currentHeader->key[parseCtx->keyLen] = '\0';
		parseCtx->parseState = PS_HEADER_VALUE;
	}
	return PSCONTINUE;
}
static int parseHeaderValueOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
{
	if (ch != '\r' && ch != '\n') {
		if (ch != ' ' || !parseCtx->parsingSpace) {
			parseCtx->currentHeader->value[parseCtx->valueLen++] = ch;
			parseCtx->parsingSpace = 0;
		}
	}
	else if (ch == '\n') {
		parseCtx->currentHeader->value[parseCtx->valueLen] = '\0';
		insertHTTPHeader(&(request->headers), parseCtx->currentHeader);
		parseCtx->parseState = PS_HEADER_KEY;
		parseCtx->currentHeader = NULL;
		parseCtx->keyLen = 0;
		parseCtx->valueLen = 0;
		parseCtx->parsingSpace = 1;
	}
	return PSCONTINUE;
}

static int parseBodyOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
{
	request->body[parseCtx->bodyLen++] = ch;
	if (parseCtx->bodyLen == request->bodyLength) {
		return PSEND;
	}
	return PSCONTINUE;
}


static int (*findParseOperationByState(int state))(char, struct HTTPRequestPacket*, struct ParseContext* parseCtx)
{
	switch (state)
	{
	case PS_METHOD:
		return parseMethodOperation;
	case PS_URI:
		return parseURIOperation;
	case PS_VERSION:
		return parseVersionOperation;
	case PS_HEADER_KEY:
		return parseHeaderKeyOperation;
	case PS_HEADER_VALUE:
		return parseHeaderValueOperation;
	case PS_BODY:
		return parseBodyOperation;
	}

	return NULL;
}

static int parseBufToRequestPacket(const char* buf, int len, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
{
	int ret = PSCONTINUE;

	for (int i = 0; i < len && ret == PSCONTINUE; i++) {
		ret = findParseOperationByState(parseCtx->parseState)(buf[i], request, parseCtx);
	}

	return ret;
}



int recvHTTPRequestPacket(SOCKET sock, struct HTTPRequestPacket* request)
{
	char buf[1024];
	int bytesRead = 0;
	int ret;
	struct ParseContext parseCtx;

	initParseContext(&parseCtx);
	do {
		bytesRead = recv(sock, buf, 1024, 0);
		if (bytesRead > 0) {
			ret = parseBufToRequestPacket(buf, bytesRead, request, &parseCtx);
			if (ret == PSERROR) {
				return PSERROR;
			}
			if (ret == PSEND) {
				return 0;
			}
		}
		else if (bytesRead == 0) {
			printf("connection closed when parsing.\n");
			return -2;
		}
		else {
			printf("recv fail,le=%d\n", WSAGetLastError());
			return -3;
		}
	} while (bytesRead > 0);

	return 0;
}