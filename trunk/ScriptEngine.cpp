#include "ScriptEngine.h"
#include "D2BS.h"
#include "Core.h"

#include <algorithm>

using namespace std;

JSRuntime* ScriptEngine::runtime = NULL;
ScriptMap ScriptEngine::scripts = ScriptMap();
EngineState ScriptEngine::state = Stopped;
CRITICAL_SECTION ScriptEngine::lock = {0};

Script* ScriptEngine::CompileFile(const char* file, ScriptState state, bool recompile)
{
	file = _strlwr(_strdup(file));
	try {
		if(!Vars.bDisableCache) {
			if(recompile && scripts.count(file) > 0) {
				scripts[file]->Stop(true, true);
				delete scripts[file];
			} else if(scripts.count(file) > 0) {
				scripts[file]->Stop(true, true);
				return scripts[file];
			}
		}
		Script* script = new Script(file, state);
		scripts[file] = script;
		return script;
	} catch(std::exception e) {
		Print(const_cast<char*>(e.what()));
		return NULL;
	}
}

Script* ScriptEngine::CompileCommand(const char* command)
{
	char* file = _strlwr(_strdup(command));
	try {
		if(!Vars.bDisableCache) {
			if(scripts.count(file) > 0) {
				return scripts[file];
			}
		}
		Script* script = new Script(command, Command);
		scripts[file] = script;
		return script;
	} catch(std::exception e) {
		Print(const_cast<char*>(e.what()));
		return NULL;
	}
}

void ScriptEngine::DisposeScript(Script* script)
{
	if(scripts.count(script->GetFilename()))
		scripts.erase(script->GetFilename());
	delete script;
}

unsigned int ScriptEngine::GetCount(bool active, bool unexecuted)
{
	ScriptList list;
	GetScripts(list);
	unsigned int count = list.size();
	for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!active && it->second->IsRunning() && !it->second->IsAborted())
			count--;
		if(!unexecuted && it->second->GetExecutionCount() == 0)
			count--;
	}
	return count;
}

void ScriptEngine::Startup(void)
{
	if(!runtime)
	{
		state = Starting;
		// set the memory limit at 200mb
		runtime = JS_NewRuntime(0xC80000);
		JS_SetGCCallbackRT(runtime, gcCallback);
		//JS_SetThrowHook(runtime, exceptionCallback, NULL);
		//JS_SetCallHook(runtime, executeCallback, NULL);
		//JS_SetExecuteHook(runtime, executeCallback, NULL);
		//JS_SetDebuggerHandler(runtime, debuggerCallback, NULL);
		InitializeCriticalSection(&lock);
		state = Running;
	}
}

void ScriptEngine::Shutdown(void)
{
	// bring the engine down properly
	state = Stopping;
	StopAll(true);

	// delete all scripts now that they're stopped
	for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
		delete (it->second);

	scripts.clear();

	if(runtime)
	{
		JS_DestroyRuntime(runtime);
		JS_ShutDown();
		runtime = NULL;
	}

	DeleteCriticalSection(&lock);
	state = Stopped;
}

typedef pair<const string, Script*> ScriptPair;

struct Stop : public unary_function<ScriptPair, void> {
	void operator() (ScriptPair& script) {
		script.second->Stop(true, (ScriptEngine::GetState() == Stopping));
	}
};

struct Pause : public unary_function<ScriptPair, void> {
	void operator() (ScriptPair& script) {
		script.second->Pause();
	}
};

struct Resume : public unary_function<ScriptPair, void> {
	void operator() (ScriptPair& script) {
		script.second->Resume();
	}
};

void ScriptEngine::StopAll(bool forceStop) { for_each(scripts.begin(), scripts.end(), Stop()); }
void ScriptEngine::PauseAll(void) { for_each(scripts.begin(), scripts.end(), Pause()); }
void ScriptEngine::ResumeAll(void) { for_each(scripts.begin(), scripts.end(), Resume()); }

void ScriptEngine::FlushCache(void)
{
	static bool isFlushing = false;
	if(isFlushing || Vars.bDisableCache)
		return;

	EnterCriticalSection(&Vars.cFlushCacheSection);

	isFlushing = true;

	ScriptList list;
	GetScripts(list);
	for(ScriptList::iterator it = list.begin(); it != list.end(); it++)
	{
		if(!(*it)->IsRunning())
		{
			scripts.erase((*it)->GetFilename());
			delete *it;
		}
	}

	isFlushing = false;

	LeaveCriticalSection(&Vars.cFlushCacheSection);
}

void ScriptEngine::GetScripts(ScriptList& list)
{
	list.clear();
	for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
		list.push_back(it->second);
}

void ScriptEngine::ExecEvent(char* evtName, AutoRoot** argv, uintN argc)
{
	jsval dummy;
	ScriptList list;
	GetScripts(list);
	for(ScriptList::iterator it = list.begin(); it != list.end(); it++)
		if(!(*it)->IsAborted())
			(*it)->ExecEvent(evtName, argc, argv, &dummy);
}

void ScriptEngine::ExecEventAsync(char* evtName, AutoRoot** argv, uintN argc)
{
	ScriptList list;
	GetScripts(list);
	for(ScriptList::iterator it = list.begin(); it != list.end(); it++)
		if(!(*it)->IsAborted())
			(*it)->ExecEventAsync(evtName, argc, argv);
}

JSTrapStatus exceptionCallback(JSContext *cx, JSScript *script, jsbytecode *pc, jsval *rval, void *closure)
{
	return JSTRAP_CONTINUE;
}

void* executeCallback(JSContext* cx, JSStackFrame* frame, JSBool before, JSBool* ok, void* closure)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	if(!script)
		return NULL;

	static JSBool dummy;
	dummy = (script->IsSingleStep() ? JS_TRUE : JS_FALSE);
	return dummy ? &dummy : NULL;
}

JSTrapStatus debuggerCallback(JSContext *cx, JSScript *jsscript, jsbytecode *pc, jsval *rval, void *closure)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	if(!script)
		return JSTRAP_CONTINUE;

	script->EnableSingleStep();
	return JSTRAP_CONTINUE;
}

JSBool branchCallback(JSContext* cx, JSScript*)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	bool pause = script->IsPaused();

	jsrefcount depth = JS_SuspendRequest(cx);

	if(pause)
		script->SetPauseState(true);
	while(script->IsPaused())
		Sleep(50);
	if(pause)
		script->SetPauseState(false);
	// assume it was trampled.
	JS_SetContextThread(cx);
	JS_ResumeRequest(cx, depth);
	// there is no need to use up MORE processor time for an if statement when it can be collapsed down.
	// JS_TRUE and JS_FALSE are simply typecasted 1 and 0 respectively.
	return !!!(JSBool)(script->IsAborted() || ((script->GetState() != OutOfGame) && !D2CLIENT_GetPlayerUnit()));
}

JSBool gcCallback(JSContext *cx, JSGCStatus status)
{
	if(status == JSGC_BEGIN)
	{
		if(Vars.bDebug)
			Log("*** ENTERING GC ***");
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
	}
	else if(status == JSGC_END)
	{
		if(Vars.bDebug)
			Log("*** LEAVING GC ***");
		ScriptEngine::FlushCache();
	}
	return JS_TRUE;
}

void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	bool warn = JSREPORT_IS_WARNING(report->flags);
	bool isStrict = JSREPORT_IS_STRICT(report->flags);
	const char* type = (warn ? "Warning" : "Error");
	const char* strict = (isStrict ? "Strict " : "");
	const char* filename = (report->filename ? report->filename + strlen(Vars.szScriptPath)+1 : "<unknown>");
	Log("[%s%s] Code (%d) %s/line %d: %s\nLine: %s", strict, type, report->errorNumber, 
				filename, report->lineno, message, report->linebuf);

	Print("[ÿc%d%s%sÿc0 (%d)] %s/line %d: %s", (warn ? 9 : 1), strict, type, report->errorNumber,
					filename, report->lineno, message);

	if(Vars.bQuitOnError && D2CLIENT_GetPlayerUnit() && !JSREPORT_IS_WARNING(report->flags))
		D2CLIENT_ExitGame();
}
