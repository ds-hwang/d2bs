#pragma once

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <cstdlib>

// TODO: Fill this in with different settings
struct Config
{
	char scriptPath[_MAX_FNAME];
};

// TODO: Keep this up with the struct above
static Config DefaultConfig = 
{
	"scripts"
};


bool LoadConfig(const char* file, Config* config);
void WriteConfig(const char* file);
bool FileExists(const char* fname);
void Log(const char* format, ...);

#endif
