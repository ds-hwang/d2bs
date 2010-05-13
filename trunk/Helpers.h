#pragma once

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "Script.h"

wchar_t* AnsiToUnicode(const char* str);
char* UnicodeToAnsi(const wchar_t* str);
bool StringToBool(const char* str);

void Print(const char * szFormat, ...);
void Log(char* szFormat, ...);

void StringReplace(char* str, const char find, const char replace, size_t buflen);
bool SplitLines(const std::string & str, size_t maxlen, const char delim, std::list<std::string> & lst);
bool InArea(int x, int y, int x2, int y2, int sizex, int sizey);
bool ProfileExists(const char* profile);
void InitSettings(void);
bool InitHooks(void);
bool ExecCommand(const char* command);
bool StartScript(const char* script);
void Reload(void);
bool ProcessCommand(const char* command, bool unprocessedIsCommand);

void GameJoined(void);
void MenuEntered(bool beginStarter);

#endif
