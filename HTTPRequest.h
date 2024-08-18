#pragma once

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

void initParseContext(ParseContext* parseContext);

int parseBufToRequestPacket(const char* buf, int len, HTTPRequestPacket* request, ParseContext* parseCtx);
