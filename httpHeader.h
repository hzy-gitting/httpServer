#pragma once
#include<WinSock2.h>
struct HTTPHeader {
	char* key;
	char* value;
	struct HTTPHeader* next;
};


HTTPHeader* mallocHTTPHeaderNode();

HTTPHeader* createHTTPHeader(const char* key, const char* value);

void insertHTTPHeader(HTTPHeader** ppHead, HTTPHeader* s);

void freeHTTPHeaders(HTTPHeader* pHeader);
