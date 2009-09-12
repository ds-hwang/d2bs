#include <io.h>
#include <algorithm>

#include "Script.h"
#include "JSGlobalFuncs.h"
#include "Core.h"
#include "JSUnit.h"
#include "Constants.h"
#include "D2Ptrs.h"
#include "D2BS.h"
#include "CDebug.h"
#include "Helpers.h"
#include "ScriptEngine.h"

#include "debugnew/debug_new.h"

using namespace std;

AutoRoot::AutoRoot(JSContext* cx, jsval nvar) :	context(cx), var(nvar), count(0) { Take(); }
AutoRoot::~AutoRoot()
{
	if(!(count--))
	{
		fprintf(stderr, "AutoRoot failed: Count is still %i, but the root is being destroyed", count);
		DebugBreak();
		exit(3);
	}
	JS_RemoveRoot(context, &var);
}
void AutoRoot::Take() { count++; JS_AddRoot(context, &var); }
void AutoRoot::Release()
{
	count--;
	if(count < 0)
	{
		fprintf(stderr, "Improper AutoRoot usage: Count is less than 0");
		DebugBreak();
		exit(3);
	}
}
jsval AutoRoot::value() { return var; }
bool AutoRoot::operator==(AutoRoot& other) { return other.value() == var; }

Script::Script(const char* file, ScriptState state) :
			context(NULL), globalObject(NULL), scriptObject(NULL), script(NULL), execCount(0),
			isAborted(false), isPaused(false), isReallyPaused(false), singleStep(false),
			scriptState(state), threadHandle(NULL), threadId(0), lockThreadId(-1)
{
	if(scriptState != Command && _access(file, 0) != 0)
		throw std::exception("File not found");

	InitializeCriticalSection(&lock);
	EnterCriticalSection(&lock);

	fileName = string(_strlwr(_strdup(file)));
	replace(fileName.begin(), fileName.end(), '/', '\\');
	try {
		context = JS_NewContext(ScriptEngine::GetRuntime(), 0x2000);
		if(!context)
			throw std::exception("Couldn't create the context");

		myUnit* lpUnit = new myUnit; // leaked
		memset(lpUnit, NULL, sizeof(myUnit));

		UnitAny* player = D2CLIENT_GetPlayerUnit();
		lpUnit->dwMode = -1;
		lpUnit->dwClassId = -1;
		lpUnit->dwType = UNIT_PLAYER;
		lpUnit->dwUnitId = player ? player->dwUnitId : NULL;
		lpUnit->_dwPrivateType = PRIVATE_UNIT;

		JS_SetContextThread(context);
		JS_SetContextPrivate(context, this);
		JS_BeginRequest(context);

		globalObject = JS_NewObject(context, &global_obj, NULL, NULL);
		if(!globalObject)
			throw std::exception("Couldn't create the global object");

		if(JS_InitStandardClasses(context, globalObject) == JS_FALSE)
			throw std::exception("Couldn't init standard classes");
		JS_DefineFunctions(context, globalObject, global_funcs);

		InitClass(&file_class_ex.base, file_methods, file_props, file_s_methods, NULL);
		InitClass(&filetools_class, NULL, NULL, filetools_s_methods, NULL);
		InitClass(&sqlite_db_ex.base, sqlite_methods, sqlite_props, NULL, NULL);
		InitClass(&sandbox_class, sandbox_methods, NULL, NULL, NULL);
		InitClass(&frame_class, frame_methods, frame_props, NULL, NULL);
		InitClass(&box_class, box_methods, box_props, NULL, NULL);
		InitClass(&line_class, line_methods, line_props, NULL, NULL);
		InitClass(&text_class, text_methods, text_props, NULL, NULL);
		InitClass(&image_class, image_methods, image_props, NULL, NULL);
		InitClass(&unit_class, unit_methods, unit_props, NULL, NULL);

		meObject = BuildObject(context, &unit_class, unit_methods, me_props, lpUnit);
		if(!meObject)
			throw std::exception("Couldn't create the meObject");

		if(JS_AddRoot(context, &meObject) == JS_FALSE)
			throw std::exception("Couldn't add root");

		JS_DefineProperty(context, globalObject, "me", OBJECT_TO_JSVAL(meObject), NULL, NULL, JSPROP_CONSTANT);

#define DEFCONST(vp) DefineConstant(#vp, vp)
#define DEFEVENT(vp) DEFCONST(EVENT_##vp)
		DEFCONST(FILE_READ);
		DEFCONST(FILE_WRITE);
		DEFCONST(FILE_APPEND);

		DEFEVENT(AREACHANGE);
		DEFEVENT(CHATMSG);
		DEFEVENT(COPYDATA);
		DEFEVENT(GAMEMSG);
		DEFEVENT(HOSTILEMSG);
		DEFEVENT(INPUTLINE);
		DEFEVENT(ITEMSTAT);
		DEFEVENT(KEYDOWN);
		DEFEVENT(KEYUP);
		DEFEVENT(MELIFE);
		DEFEVENT(MISSILEMOVE);
		DEFEVENT(MISSILESTATE);
		DEFEVENT(MOUSEDOWN);
		DEFEVENT(MOUSEUP);
		DEFEVENT(NEWITEM);
		DEFEVENT(NEWNPC);
		DEFEVENT(NPCLIFE);
		DEFEVENT(NPCMOVE);
		DEFEVENT(NPCSTAT);
		DEFEVENT(NPCSTATE);
		DEFEVENT(PARTYMSG);
		DEFEVENT(PLAYERMOVE);
		DEFEVENT(PLAYERSTAT);
		DEFEVENT(PLAYERSTATE);
		DEFEVENT(QUEST);
		DEFEVENT(SCRIPTMSG);
		DEFEVENT(UNITMOVE);
		DEFEVENT(WINDOWFOCUS);
		DEFEVENT(CHATCMD);
		DEFEVENT(PLAYERASSIGN);
#undef DEFEVENT
#undef DEFCONST
		if(state == Command)
			script = JS_CompileScript(context, globalObject, file, strlen(file), "Command Line", 1);
		else
			script = JS_CompileFile(context, globalObject, fileName.c_str());
		if(!script)
			throw std::exception("Couldn't compile the script");

		scriptObject = JS_NewScriptObject(context, script);
		if(!scriptObject)
			throw std::exception("Couldn't create the script object");

		if(JS_AddNamedRoot(context, &meObject, "me object") == JS_FALSE)
			throw std::exception("Couldn't add named root");
		if(JS_AddNamedRoot(context, &scriptObject, "script object") == JS_FALSE)
			throw std::exception("Couldn't add named root");

		JS_EndRequest(context);

		LeaveCriticalSection(&lock);
	} catch(std::exception&) {
		if(scriptObject)
			JS_RemoveRoot(context, &scriptObject);
		if(script && !scriptObject)
			JS_DestroyScript(context, script);
		if(context)
		{
			JS_EndRequest(context);
			JS_DestroyContext(context);
		}
		LeaveCriticalSection(&lock);
		throw;
	}
}

Script::~Script(void)
{
	Stop(true, true);

	JS_SetContextThread(context);
	JS_BeginRequest(context);

	// use the RT version of RemoveRoot to prevent crashes
	JS_RemoveRootRT(ScriptEngine::GetRuntime(), &globalObject);
	JS_RemoveRootRT(ScriptEngine::GetRuntime(), &meObject);
	JS_RemoveRootRT(ScriptEngine::GetRuntime(), &scriptObject);

	JS_DestroyContext(context);

	context = NULL;
	scriptObject = NULL;
	globalObject = NULL;
	script = NULL;

	includes.clear();
	if(threadHandle)
		CloseHandle(threadHandle);
}

void Script::InitClass(JSClass* classp, JSFunctionSpec* methods, JSPropertySpec* props,
					   JSFunctionSpec* s_methods, JSPropertySpec* s_props)
{
	if(!JS_InitClass(context, globalObject, NULL, classp, classp->construct, 0,
		props, methods, s_props, s_methods))
		throw std::exception("Couldn't initialize the class");
}

void Script::DefineConstant(const char* name, int value)
{
	if(!JS_DefineProperty(context, globalObject, name, INT_TO_JSVAL(value), NULL, NULL, JSPROP_CONSTANT))
		throw std::exception("Couldn't initialize the constant");
}

int Script::GetExecutionCount(void)
{
	return execCount;
}

DWORD Script::GetThreadId(void)
{
	return (threadHandle == NULL ? -1 : threadId);
}

void Script::Run(void)
{
	// only let the script run if it's not already running
	if(IsRunning())
		return;

	isAborted = false;
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &threadHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
	threadId = GetCurrentThreadId();

	jsval main = JSVAL_VOID, dummy = JSVAL_VOID;

	JS_SetContextThread(GetContext());
	JS_BeginRequest(GetContext());

	JS_ExecuteScript(GetContext(), globalObject, script, &dummy);

	JS_GetProperty(GetContext(), globalObject, "main", &main);
	if(JSVAL_IS_FUNCTION(GetContext(), main))
	{
		JS_CallFunctionValue(GetContext(), globalObject, main, 0, NULL, &dummy);
	}

	JS_SetContextThread(GetContext());
	JS_EndRequest(GetContext());	

	execCount++;
	Stop();
}

void Script::Pause(void)
{
	if(!IsAborted() && !IsPaused())
		isPaused = true;
}

void Script::Resume(void)
{
	if(!IsAborted() && IsPaused())
		isPaused = false;
}

bool Script::IsPaused(void)
{
	return IsBadReadPtr(this, sizeof(this)) ? false : isPaused;
}

void Script::Stop(bool force, bool reallyForce)
{	
	// Clear the events/hooks before aborting the script
	// otherwise we can't clean up all the events in the context
	ClearAllEvents();
	Genhook::Clean(this);

	isAborted = true;
	isPaused = false;
	isReallyPaused = false;

	int maxCount = (force ? (reallyForce ? 100 : 300) : 500);
	for(int i = 0; IsRunning(); i++)
	{
		// if we pass the time frame, just ignore the wait because the thread will end forcefully anyway
		if(i >= maxCount)
			break;
		Sleep(10);
	}

	if(threadHandle)
		CloseHandle(threadHandle);
	threadHandle = NULL;

}

void Script::EnableSingleStep(void) { singleStep = true; }
void Script::DisableSingleStep(void) { singleStep = false; }
bool Script::IsSingleStep(void) { return singleStep; }

bool Script::IsIncluded(const char* file)
{
	char* fname = _strlwr((char*)file);
	StringReplace(fname, '/', '\\');
	return !!includes.count(string(fname));
}

bool Script::Include(const char* file)
{
	char* fname = _strlwr((char*)file);
	StringReplace(fname, '/', '\\');
	// ignore already included, 'in-progress' includes, and self-inclusion
	if(IsIncluded(fname) || !!inProgress.count(string(fname)) || (fileName == string(fname)))
		return true;
	bool rval = false;
	JS_BeginRequest(GetContext());

	JSScript* script = JS_CompileFile(GetContext(), globalObject, fname);
	if(script)
	{
		jsval dummy;
		inProgress[fname] = true;
		rval = !!JS_ExecuteScript(GetContext(), globalObject, script, &dummy);
		JS_DestroyScript(GetContext(), script);
		if(rval)
			includes[fname] = true;
		inProgress.erase(fname);
	}
	else
		return false;

	// HACK: assume we have to reclaim ownership
	JS_SetContextThread(GetContext());
	JS_EndRequest(GetContext());
	return rval;
}

bool Script::IsRunning(void)
{
	return !IsBadReadPtr(this, sizeof(this)) && !IsBadReadPtr(context, sizeof(context)) &&
					context && !(!JS_IsRunning(context) || IsPaused());
}

bool Script::IsAborted()
{
	return IsBadReadPtr(this, sizeof(this)) ? true : isAborted;
}

void Script::RegisterEvent(const char* evtName, jsval evtFunc)
{
	if(JSVAL_IS_FUNCTION(context, evtFunc))
	{
		AutoRoot* root = new AutoRoot(context, evtFunc);
		root->Take();
		functions[evtName].push_back(root);
	}
}

bool Script::IsRegisteredEvent(const char* evtName, jsval evtFunc)
{
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
		if((*it)->value() == evtFunc)
			return true;

	return false;
}

void Script::UnregisterEvent(const char* evtName, jsval evtFunc)
{
	AutoRoot* func = NULL;
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
	{
		if((*it)->value() == evtFunc)
		{
			func = *it;
			break;
		}
	}
	functions[evtName].remove(func);
	func->Release();
	delete func;
}

void Script::ClearEvent(const char* evtName)
{
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
	{
		AutoRoot* func = *it;
		func->Release();
		delete func;
	}
	functions[evtName].clear();
}

void Script::ClearAllEvents(void)
{
	for(FunctionMap::iterator it = functions.begin(); it != functions.end(); it++)
		ClearEvent(it->first.c_str());
}

JSBool Script::ExecEvent(char* evtName, uintN argc, AutoRoot** argv, jsval* rval)
{
	char msg[50];
	sprintf(msg, "Script::ExecEvent(%s)", evtName);
	CDebug cDbg(msg);

	Pause();

	JSContext* cx = JS_NewContext(ScriptEngine::GetRuntime(), 0x2000);
	if(!cx)
		return JS_FALSE;

	JS_BeginRequest(cx);

	JS_SetContextPrivate(cx, this);
	for(uintN i = 0; i < argc; i++)
		argv[i]->Take();

	if(JS_GetContextThread(context) != GetCurrentThreadId())
		THROW_ERROR(context, globalObject, "Cross-thread attempt to execute an event");

	FunctionList flist = functions[evtName];

	jsval* args = new jsval[argc];
	for(uintN i = 0; i < argc; i++)
		args[i] = argv[i]->value();

	for(FunctionList::iterator it = flist.begin(); it != flist.end(); it++)
		JS_CallFunctionValue(cx, globalObject, (*it)->value(), argc, args, rval);

	delete[] args;

	JS_DestroyContext(cx);
	cx = NULL;

	for(uintN i = 0; i < argc; i++)
		argv[i]->Release();

	delete[] argv;

	Resume();

	return JS_TRUE;
}

void Script::ExecEventAsync(char* evtName, uintN argc, AutoRoot** argv)
{
	if(!IsAborted() && !IsPaused() && functions.count(evtName))
	{
		char msg[50];
		sprintf(msg, "Script::ExecEventAsync(%s)", evtName);
		CDebug cDbg(msg);

		for(uintN i = 0; i < argc; i++)
			argv[i]->Take();

		Event* evt = new Event;
		evt->owner = this;
		evt->functions = functions[evtName];
		evt->argc = argc;
		evt->argv = argv;
		evt->context = JS_NewContext(ScriptEngine::GetRuntime(), 0x2000);
		if(!evt->context)
		{
			delete evt;
			return;
		}
		evt->object = globalObject;
		JS_SetContextPrivate(evt->context, this);

		CreateThread(0, 0, FuncThread, evt, 0, 0);
	}
}

DWORD WINAPI ScriptThread(void* data)
{
	Script* script = (Script*)data;
	if(script)
	{
		script->Run();
		if(Vars.bDisableCache)
			ScriptEngine::DisposeScript(script);
	}
	return 0;
}

DWORD WINAPI FuncThread(void* data)
{
	Event* evt = (Event*)data;
	if(!evt)
		return 0;

	JS_SetContextThread(evt->context);
	JS_BeginRequest(evt->context);

	if(!evt->owner->IsAborted() || !(evt->owner->GetState() == InGame && !GameReady()))
	{
		jsval dummy = JSVAL_VOID;

		jsval* args = new jsval[evt->argc];
		for(uintN i = 0; i < evt->argc; i++)
		{
			args[i] = evt->argv[i]->value();
			if(JS_AddRoot(evt->context, &args[i]) == JS_FALSE)
			{
				if(evt->argv)
					delete[] evt->argv;
				delete evt;
				return NULL;
			}
		}

		for(FunctionList::iterator it = evt->functions.begin(); it != evt->functions.end(); it++)
		{
			JS_CallFunctionValue(evt->context, evt->object, (*it)->value(), evt->argc, args, &dummy);
		}

		for(uintN i = 0; i < evt->argc; i++)
			JS_RemoveRoot(evt->context, &args[i]);
		delete[] args;

		// check if the caller stole the context thread
		if ((DWORD)JS_GetContextThread(evt->context) != evt->owner->GetThreadId())
		{
			JS_ClearContextThread(evt->context);
			JS_SetContextThread(evt->context);	
		}
	}

	JS_DestroyContext(evt->context);
	// assume we have to clean up both the event and the args, and release autorooted vars
	for(uintN i = 0; i < evt->argc; i++)
	{
		evt->argv[i]->Release();
		if(evt->argv[i])
			delete evt->argv[i];
	}
	if(evt->argv)
		delete[] evt->argv;
	delete evt;
	
	return 0;
}
