#pragma once

#ifndef __HELPERS_H__
#define __HELPERS_H__

wchar_t* AnsiToUnicode(const char* str);
char* UnicodeToAnsi(const wchar_t* str);
bool StringToBool(const char* str);
void StringReplace(char* str, const char find, const char replace);

#endif
