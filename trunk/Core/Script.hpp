#pragma once
#include <windows.h>

#include <unordered_map>

#include "js32.hpp"
#define EXPORTING
#include "Exports.hpp"
#undef EXPORTING

#include "Engine.hpp"

namespace Core {

enum ScriptState
{
	Waiting,
	Running,
	Paused,
	Done,
	Stopping,
	Stopped,
	Failed
};

typedef std::unordered_map<std::string, std::list<jsval>> EventMap;

class Script
{
private:
	Script(const Script&);
	Script& operator=(const Script&);

	Engine* engine;

	JSContext *cx;
	JSObject *obj, *script, *paths;
	ScriptState state, oldState;
	EventMap map;

	HANDLE thread, pause;
	CRITICAL_SECTION lock;

	// NB: this behavior is roughly equivalent to these functions
	// "friend"ing Script but looks just a little neater, IMO.
	static void __cdecl ThreadProc(void*);
	static JSBool OperationCallback(JSContext *cx);

	friend Engine;

	Script(const wchar_t* path, Engine* engine);
	~Script(void);

public:
	EXPORT void Start(void);
	EXPORT void Pause(void);
	EXPORT void Resume(void);
	EXPORT void Stop(void);

	EXPORT void AddListener(const char* evt, jsval callback);
	EXPORT void RemoveListener(const char* evt, jsval callback);
	EXPORT void RemoveListeners(const char* evt);
	EXPORT void RemoveListeners();

	EXPORT inline Engine* GetEngine(void) { return engine; }
	EXPORT inline ScriptState GetState(void) { return state; }
	EXPORT inline JSContext* GetContext(void) { return cx; }
	EXPORT inline JSObject* GetGlobalObject(void) { return obj; }
	inline JSObject* GetSearchPath(void) { return paths; }

	EXPORT void FireEvent(const char* evt, std::list<JS::Anchor<jsval>> args);
};

}
