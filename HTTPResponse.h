#pragma once

struct HTTPResponsePacket {
	char* version;
	char* statusCode;
	char* reasonPhrase;
	struct HTTPHeader* headers;
	char* body;
	int bodyLength;
};

int initHTTPResponsePacket(HTTPResponsePacket* response);

void addResponseHeader(HTTPResponsePacket* response, const char* key, const char* value);

void serializeHTTPResponsePacket(HTTPResponsePacket* response, char* buf, int* len);

void destroyHTTPResponsePacket(HTTPResponsePacket* response);
