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
protected:
	// hide the ctor, but disable the copy ctor/assignment op
	Script(std::string filename);
	Script(const Script&);
	Script& operator=(const Script&);

	void* pData;
	std::string filename;
	CRITICAL_SECTION lock;

public:
	virtual ~Script(void) = 0;

	// create a new execution path and run the current script inside it
	virtual void Run(void) = 0;
	// stop the current script gracefully
	virtual void Stop(void) = 0;
	// stop the current script forcefully
	virtual void End(void) = 0;

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
