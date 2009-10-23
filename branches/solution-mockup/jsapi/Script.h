#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <list>

#include "yasper.h"

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
	CRITICAL_SECTION lock;

public:
	// create a new execution path and run the current script inside it
	void Run(void);
	// stop the current script gracefully
	void Stop(void);
	// stop the current script forcefully
	void End(void);

	std::string GetFilename(void) { return filename; }

	// intentionally leaving this as a ptr
	void* GetPrivateData(void) { return pData; }
	void SetPrivateData(void* data) { Lock(); pData = data; Unlock(); }

	void Lock(void) { EnterCriticalSection(&lock); }
	void Unlock(void) { LeaveCriticalSection(&lock); }

	// hack to allow ScriptEngine to directly meddle with the ctor/dtor
	friend class ScriptEngine;
	friend class yasper::ptr<Script>;
};

typedef yasper::ptr<Script> ScriptPtr;

}

#endif
