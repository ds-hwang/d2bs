#include <errno.h>
#include <io.h>
#include <windows.h>
#include <cstring>
#include <ctime>
#include <cstdio>

#include "helpers.h"

#include "debug_new.h"

bool LoadConfig(const char* file, Config* config)
{
	if(!FileExists(file))
		return false;

	GetPrivateProfileString("settings", "ScriptPath", "scripts", config->scriptPath, _MAX_FNAME, file);

	return true;
}

void WriteConfig(const char* file)
{
	if(FileExists(file))
		return;

	WritePrivateProfileString("settings", "ScriptPath", DefaultConfig.scriptPath, file);
}

bool FileExists(const char* fname)
{
	return !!!(_access(fname, 0) == -1 && errno == ENOENT);
}

void Log(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	int len = _vscprintf(format, args);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	vsprintf(str, format, args);

	va_end(args);

	time_t now;
	time(&now);
	char time[128];
	strftime(time, sizeof(time), "%b %d %Y - %H:%M:%S", localtime(&now));

	fprintf(stderr, "[%s] %s\n", time, str);
	fflush(stderr);

	delete[] str;
}

wchar_t* AnsiToUnicode(const char* str)
{
	wchar_t* buf = NULL;
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, buf, 0);
	buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, str, -1, buf, len);
	return buf;
}

char* UnicodeToAnsi(const wchar_t* str)
{
	char* buf = NULL;
	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, buf, 0, "?", NULL);
	buf = new char[len];
	WideCharToMultiByte(CP_ACP, 0, str, -1, buf, len, "?", NULL);
	return buf;
}