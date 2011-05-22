#pragma once
#include <windows.h>

#define EXPORTING

#include "js32.hpp"
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

typedef std::map<std::string, std::list<jsval>> EventMap;

class Script
{
private:
	Script(const Script&);
	Script& operator=(const Script&);

	ScriptEngine* engine;

	JSContext *cx;
	JSObject *obj, *script;
	ScriptState state;
	EventMap map;

	HANDLE thread;
	CRITICAL_SECTION lock;

	// NB: this behavior is roughly equivalent to these functions
	// "friend"ing Script but looks just a little neater, IMO.
	static void __cdecl ThreadProc(void*);
	static JSBool OperationCallback(JSContext *cx);

	friend ScriptEngine;

public:
	EXPORT Script(const wchar_t* path, ScriptEngine* engine);

	EXPORT void Start(void);
	EXPORT void Stop(void);

	EXPORT void AddListener(const char* evt, jsval callback);
	EXPORT void RemoveListener(const char* evt, jsval callback);
	EXPORT void RemoveListener(const char* evt);
	EXPORT void RemoveListeners();

	EXPORT inline ScriptEngine* GetEngine(void) { return engine; }
	EXPORT inline ScriptState GetState(void) { return state; }
	EXPORT inline JSContext* GetContext(void) { return cx; }
	EXPORT inline JSObject* GetGlobalObject(void) { return obj; }

	EXPORT void FireEvent(const char* evt, std::list<JS::Anchor<jsval>> args);
};
