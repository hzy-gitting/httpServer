#define _CRT_SECURE_NO_WARNINGS
#include "HTTPResponse.h"
#include"httpHeader.h"

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
	return 0;
}

void addResponseHeader(struct HTTPResponsePacket* response, const char* key, const char* value) {
	insertHTTPHeader(&(response->headers), createHTTPHeader(key, value));
}

void serializeHTTPResponsePacket(struct HTTPResponsePacket* response, char* buf, int* len) {
	*len = 0;
	strcpy(buf, response->version);
	buf += strlen(response->version);
	*len += strlen(response->version);
	strcpy(buf, " ");
	buf += 1;
	*len += 1;
	strcpy(buf, response->statusCode);
	buf += strlen(response->statusCode);
	*len += strlen(response->statusCode);
	strcpy(buf, " ");
	buf += 1;
	*len += 1;
	strcpy(buf, response->reasonPhrase);
	buf += strlen(response->reasonPhrase);
	*len += strlen(response->reasonPhrase);
	strcpy(buf, "\r\n");
	buf += 2;
	*len += 2;
	struct HTTPHeader* header = response->headers;
	while (header) {
		strcpy(buf, header->key);
		buf += strlen(header->key);
		*len += strlen(header->key);
		strcpy(buf, ": ");
		buf += 2;
		*len += 2;
		strcpy(buf, header->value);
		buf += strlen(header->value);
		*len += strlen(header->value);
		strcpy(buf, "\r\n");
		buf += 2;
		*len += 2;
		header = header->next;
	}
	strcpy(buf, "\r\n");
	buf += 2;
	*len += 2;
	memcpy(buf, response->body, response->bodyLength);
	*len += response->bodyLength;
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
