#pragma once

#define EXPORTING

#include "js32.hpp"
#include <windows.h>
#include "Exports.hpp"

#include "ScriptEngine.hpp"

enum ScriptState
{
	Waiting,
	Running,
	Done,
	Stopping,
	Stopped
};

class EXPORT Script
{
private:
	Script(const Script&);
	Script& operator=(const Script&);

	JSContext *cx;
	JSObject *obj, *script;
	ScriptState state;

	HANDLE thread;

	// NB: this behavior is roughly equivalent to these functions
	// "friend"ing Script but looks just a little neater, IMO.
	static void __cdecl ThreadProc(void*);
	static JSBool OperationCallback(JSContext *cx);

	friend ScriptEngine;

public:
	Script(const char* path, ScriptEngine* engine);

	void Start(void);
	void Stop(void);
};
