#define _CRT_SECURE_NO_WARNINGS
#include<WinSock2.h>
#include<stdio.h>
#include"http.h"
#include"HTTPRequest.h"
#define E_MALLOC_FAIL (-1)

#define MAX_METHOD_LEN (10)
#define MAX_URI_LEN (1024 * 4)
#define MAX_VERSION_LEN (10)
#define INIT_BODY_SIZE (1024 * 4)
#define BODY_SIZE_INCREAMENT (1024 * 4)

#define MAX_KEY_LEN (1024 * 4)
#define MAX_VALUE_LEN (1024 * 4)
struct HTTPHeader* mallocHTTPHeaderNode() {
	struct HTTPHeader* header;
	header = (struct HTTPHeader*)malloc(sizeof(struct HTTPHeader));
	if (!header) {
		return NULL;
	}
	header->key = (char*)malloc(MAX_KEY_LEN);
	if (!header->key) {
		return NULL;
	}
	header->value = (char*)malloc(MAX_VALUE_LEN);
	if (!header->value) {
		return NULL;
	}
	header->next = NULL;
	return header;
}

struct HTTPHeader* createHTTPHeader(const char *key,const char *value) {
	struct HTTPHeader* s = mallocHTTPHeaderNode();
	strcpy(s->key, key);
	strcpy(s->value, value);
	return s;
}

void insertHTTPHeader(struct HTTPHeader** ppHead, struct HTTPHeader* s) {
	if (!s) {
		return;
	}
	struct HTTPHeader** pp = ppHead;
	while (*pp) {
		pp = &((*pp)->next);
	}
	*pp = s;
}
void addResponseHeader(struct HTTPResponsePacket* response, const char* key, const char* value) {
	insertHTTPHeader(&(response->headers), createHTTPHeader(key, value));
}

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

void freeHTTPHeaders(struct HTTPHeader* pHeader) {
	struct HTTPHeader* next;
	while (pHeader) {
		next = pHeader->next;
		free(pHeader);
		pHeader = next;
	}
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

int initHTTPResponsePacket(struct HTTPResponsePacket* response) {
	response->version = (char*)malloc(10);
	if (!response->version) {
		return -1;
	}
	strcpy(response->version, "HTTP/1.1");
	response->statusCode = (char*)malloc(10);
	if (!response->statusCode) {
		free(response->version);
		return -1;
	}
	strcpy(response->statusCode, "200");
	response->reasonPhrase = (char*)malloc(10);
	if (!response->reasonPhrase) {
		free(response->version);
		free(response->statusCode);
		return -1;
	}
	strcpy(response->reasonPhrase, "OK");
	response->headers = NULL;
	response->body = NULL;
	response->bodyLength = 0;

}
void serializeHTTPResponsePacket(struct HTTPResponsePacket* response, char* buf,int len) {
	strcpy(buf, response->version);
	buf += strlen(response->version);

}
void destroyHTTPResponsePacket(struct HTTPResponsePacket* response) {
	free(response->version);
	free(response->statusCode);
	free(response->reasonPhrase);
	freeHTTPHeaders(response->headers);
	if (response->body) {
		free(response->body);
	}
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