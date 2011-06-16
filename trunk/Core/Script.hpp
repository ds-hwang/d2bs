#pragma once
#include <windows.h>

#include <unordered_map>

#include "js32.hpp"
#define EXPORTING
#include "Exports.hpp"
#undef EXPORTING

#include "Engine.hpp"
#include "Event.hpp"

namespace Core {

enum ScriptState
{
	Waiting,
	Running,
	Paused,
	Events,
	Done,
	Stopping,
	Stopped,
	Failed
};

class Script;

typedef std::unordered_map<std::wstring, std::list<jsval>> EventMap;

struct ScriptEvent {
	SLIST_ENTRY ItemEntry;
	Event* evt;
	Script* who;
};

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
	SLIST_HEADER events;

	std::wstring name;

	HANDLE thread, pause;
	CRITICAL_SECTION lock;

	static void __cdecl MainProc(void*);
	static DWORD WINAPI EventProc(void*);
	static JSBool OperationCallback(JSContext *cx);

	friend Engine;

	Script(const wchar_t* path, Engine* engine);
	~Script(void);

public:
	EXPORT void Start(void);
	EXPORT void Pause(void);
	EXPORT void Resume(void);
	EXPORT void Stop(void);

	EXPORT void AddListener(const wchar_t* evt, jsval callback);
	EXPORT void RemoveListener(const wchar_t* evt, jsval callback);
	EXPORT void RemoveListeners(const wchar_t* evt);
	EXPORT void RemoveListeners();

	EXPORT inline Engine* GetEngine(void) { return engine; }
	EXPORT inline ScriptState GetState(void) { return state; }
	EXPORT inline JSContext* GetContext(void) { return cx; }
	EXPORT inline JSObject* GetGlobalObject(void) { return obj; }
	inline JSObject* GetSearchPath(void) { return paths; }

	EXPORT void FireEvent(Event* evt);
};

}
