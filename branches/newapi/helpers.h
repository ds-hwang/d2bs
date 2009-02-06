#pragma once
#include <cstdlib>

#ifndef __HELPERS_H__
#define __HELPERS_H__

struct Config
{
	char scriptPath[_MAX_FNAME];
};


bool LoadConfig(const char* file, Config* config);

#endif
