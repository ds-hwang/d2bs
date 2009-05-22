#ifndef __FILE_H__
#define __FILE_H__

#include "js32.h"

const char* readLine(FILE* fptr);
char fpeek(FILE* fptr);
bool writeValue(FILE* fptr, JSContext* cx, jsval value, bool isBinary = false);
bool isValidPath(const char* name);

#endif
