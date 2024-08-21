#define _CRT_SECURE_NO_WARNINGS
#include<WinSock2.h>
#include<stdio.h>
#include"httpHeader.h"
#include"HTTPRequest.h"
#define E_MALLOC_FAIL (-1)

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

void freeHTTPHeaders(struct HTTPHeader* pHeader) {
	struct HTTPHeader* next;
	while (pHeader) {
		next = pHeader->next;
		free(pHeader);
		pHeader = next;
	}
}