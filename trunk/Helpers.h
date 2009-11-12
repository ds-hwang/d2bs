#pragma once

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "Script.h"

wchar_t* AnsiToUnicode(const char* str);
char* UnicodeToAnsi(const wchar_t* str);
bool StringToBool(const char* str);
void StringReplace(char* str, const char find, const char replace);
bool ExecCommand(const char* command);
bool StartScript(const char* script, ScriptState state);
void Reload(void);
bool ProcessCommand(const char* command, bool unprocessedIsCommand);

#endif
