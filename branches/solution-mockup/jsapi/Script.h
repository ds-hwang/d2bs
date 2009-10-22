#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <list>

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

namespace botsys
{

class Script
{
private:
	// hide the ctor, but disable the copy ctor/assignment op
	Script(std::string filename);
	~Script(void);
	Script(const Script&);
	Script& operator=(const Script&);

	void* pData;
	std::string filename;

public:
	void Run(void);

	std::string GetFilename(void) { return filename; }
	void* GetPrivateData(void);
	void SetPrivateData(void*);

	// hack to allow ScriptEngine to directly meddle with the ctor/dtor
	friend class ScriptEngine;
};

}

#endif
