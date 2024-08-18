#include"http.h"
#include "HTTPRequest.h"

void initParseContext(struct ParseContext* parseContext) {
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
int parseMethodOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
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
int parseURIOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
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
int parseVersionOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
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

int getContentLength(struct HTTPRequestPacket* request)
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

int parseHeaderKeyOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
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
int parseHeaderValueOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
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

int parseBodyOperation(char ch, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
{
	request->body[parseCtx->bodyLen++] = ch;
	if (parseCtx->bodyLen == request->bodyLength) {
		return PSEND;
	}
	return PSCONTINUE;
}


int (*findParseOperationByState(int state))(char, struct HTTPRequestPacket*, struct ParseContext* parseCtx)
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

int parseBufToRequestPacket(const char* buf, int len, struct HTTPRequestPacket* request, struct ParseContext* parseCtx)
{
	int ret = PSCONTINUE;

	for (int i = 0; i < len && ret == PSCONTINUE; i++) {
		ret = findParseOperationByState(parseCtx->parseState)(buf[i], request, parseCtx);
	}

	return ret;
}