#pragma once

INT StringTokenize(CHAR * input, CHAR separator, CHAR ** tokenbuf, INT maxtokens);
wchar_t* AnsiToUnicode(const char* str);
char* UnicodeToAnsi(const wchar_t* str);
bool StringToBool(const char* str);
