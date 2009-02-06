#include "Script.h"
#include "helpers.h"

#include "debugnew/debug_new.h"

using namespace std;

JSRuntime* Script::runtime = NULL;
CRITICAL_SECTION Script::section = {0};
ScriptMap Script::scripts = ScriptMap();
char Script::scriptPath[_MAX_FNAME + _MAX_PATH] = "";
ScriptCallback Script::scriptCallback = NULL;
JSBranchCallback Script::branchCallback = NULL;

Script::Script(ScriptType newType, const char* fname) :
	runCount(0), type(newType), scriptObject(NULL), context(NULL),
	globalObject(NULL), script(NULL)
{
	InitializeCriticalSection(&scriptSection);
	Lock();
	try
	{
		// at this point, the file exists (if it's a file), and we need to initialize everything
		file = string((fname + (strlen(scriptPath))+1));
		includeDirs.push_back(string("libs"));
		context = JS_NewContext(runtime, 0x2000);
		if(!context)
			throw std::exception("Failed to create context");

		JS_SetContextPrivate(context, this);

		JS_BeginRequest(context);
		JS_SetBranchCallback(context, branchBack);

		globalObject = JS_NewObject(context, &global_obj, NULL, NULL);
		if(!globalObject)
			throw std::exception("Failed to create the global object");

		JS_InitStandardClasses(context, globalObject);

		if(type == Command)
			script = JS_CompileScript(context, globalObject, fname, strlen(fname), "Command", 0);
		else if(type == File)
			script = JS_CompileFile(context, globalObject, fname);
		if(!script)
			throw std::exception("Script compilation failed!");

		scriptObject = JS_NewScriptObject(context, script);
		if(!scriptObject)
			throw std::exception("Failed to create script object");

		JS_AddRoot(context, &scriptObject);

		if(scriptCallback)
			scriptCallback(this);

		scripts[file] = this;
		JS_EndRequest(context);
		Unlock();
	}
	catch(std::exception&)
	{
		if(scriptObject)
			JS_RemoveRoot(context, &scriptObject);
		if(script)
			JS_DestroyScript(context, script);
		if(context)
			JS_DestroyContext(context);

		Unlock();
		throw;
	}
}

Script::~Script(void)
{
	// we need to set aborted, join the execution thread, and destroy everything
	Stop();

	Lock();
	JS_SetContextThread(context);
	JS_RemoveRoot(context, &scriptObject);
	JS_DestroyContext(context);
	Unlock();
}

const char* Script::GetScriptPath(void)
{
	return scriptPath;
}

Script* Script::CompileFile(const char* file, bool recompile)
{
	try
	{
		char path[_MAX_FNAME + _MAX_PATH];
		sprintf(path, "%s\\%s", scriptPath, file);
		// see if the file exists, first...
		if(!FileExists(path))
		{
			char msg[_MAX_PATH+_MAX_FNAME];
			sprintf(msg, "File '%s' not found", file);
			throw std::exception(msg);
		}
		string fname = string(file);
		// look in the cache to see if we have this script saved and don't want to recomple it
		if(scripts.count(fname) > 0 && !recompile)
		{
			// TODO: See about allowing the same file to be running and recompiled, possibly keep it disallowed
			if(!scripts[fname]->IsRunning())
			{
				return scripts[fname];
			}
			else
			{
				throw std::exception("Script is currently running and cannot be recompiled!");
			}
		}
		else
		{
			return new Script(File, path);
		}
	}
	catch(std::exception&) { return NULL; }
}

Script* Script::CompileCommand(const char* file, bool recompile)
{
	string fname = string(file);
	// look in the cache to see if we have the command saved and don't want to recompile it
	if(scripts.count(fname) > 0 && !recompile)
	{
		// TODO: See about allowing the same command to be running and recompiled, possibly keep it disallowed
		if(!scripts[fname]->IsRunning())
		{
			return scripts[fname];
		}
		else
		{
			throw std::exception("Command is currently running and cannot be recompiled!");
		}
	}
	else
	{
		return new Script(File, file);
	}
}

void Script::Startup(const char* basePath, JSContextCallback contextCallback, ScriptCallback scriptCallback)
{
	if(!runtime)
	{
		sprintf(scriptPath, basePath);
		runtime = JS_NewRuntime(0x800000);
		InitializeCriticalSection(&section);
		JS_SetContextCallback(runtime, contextCallback);
		Script::scriptCallback = scriptCallback;
	}
}

void Script::Shutdown(void)
{
	if(runtime)
	{
		LockAll();
		ScriptList list = GetAllScripts();
		for(ScriptIterator it = list.begin(); it != list.end(); it++)
		{
			delete *it;
		}
		JS_DestroyRuntime(runtime);
		runtime = NULL;
		JS_ShutDown();
		UnlockAll();
		DeleteCriticalSection(&section);
	}
}

bool Script::IsActive(void)
{
	return !!runtime;
}

void Script::AbortAll(void)
{
	ScriptList list = GetAllScripts();
	for(ScriptIterator it = list.begin(); it != list.end(); it++)
	{
		(*it)->Abort();
	}
}

void Script::PauseAll(void)
{
	ScriptList list = GetAllScripts();
	for(ScriptIterator it = list.begin(); it != list.end(); it++)
	{
		(*it)->Pause();
	}
}

void Script::ResumeAll(void)
{
	ScriptList list = GetAllScripts();
	for(ScriptIterator it = list.begin(); it != list.end(); it++)
	{
		(*it)->Resume();
	}
}

void Script::LockAll(void)
{
	EnterCriticalSection(&section);
}

void Script::UnlockAll(void)
{
	LeaveCriticalSection(&section);
}

ScriptList Script::GetAllScripts(void)
{
	ScriptList list;
	for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
		list.push_back(it->second);
	return list;
}

bool Script::IsRunning(void)
{
	return !!JS_IsRunning(context) && !IsPaused();
}

bool Script::IsPaused(void)
{
	return isPaused;
}

bool Script::IsAborted(void)
{
	return isAborted;
}

void Script::Start(void)
{
	thread = PR_CreateThread(PR_USER_THREAD, ScriptThread, this, PR_PRIORITY_NORMAL, PR_GLOBAL_THREAD, PR_JOINABLE_THREAD, 0);
}

void Script::Run(void)
{
	// set the context thread and enter a request
	JS_SetContextThread(context);
	JS_BeginRequest(context);

	// execute the global code
	jsval dummy, main;
	JS_ExecuteScript(context, globalObject, script, &dummy);

	// find if we have a main function
	JS_GetProperty(context, globalObject, "main", &main);
	JS_AddRoot(context, &main);

	// if we do, execute it
	if(JSVAL_IS_FUNCTION(context, main))
	{
		// ExecuteScript may have stolen the context thread, reset it
		JS_SetContextThread(context);
		JS_CallFunctionValue(context, globalObject, main, 0, 0, &dummy);
	}

	// now that the script has finished executing fully, stop it
	Stop();

	// reset the context thread just in case something stole it
	JS_SetContextThread(context);
	JS_RemoveRoot(context, &main);

	// end the request and clear the context thread
	JS_EndRequest(context);
	JS_ClearContextThread(context);

	runCount++;
}

void Script::Pause(void)
{
	isPaused = true;
}

void Script::Resume(void)
{
	isPaused = false;
}

void Script::Stop(void)
{
	isPaused = false;
	isAborted = true;

	if(thread)
		PR_JoinThread(thread);
}

void Script::Abort(void)
{
	isAborted = true;
}

void Script::Lock(void)
{
	EnterCriticalSection(&scriptSection);
}

void Script::Unlock(void)
{
	LeaveCriticalSection(&scriptSection);
}

void Script::AddIncludePath(const char* dir)
{
	includeDirs.push_back(string(dir));
}

bool Script::IsIncluded(const char* file)
{
	for(IncludePathIterator it = includeDirs.begin(); it != includeDirs.end(); it++)
	{
		char path[_MAX_PATH + _MAX_FNAME];
		sprintf(path, "%s\\%s\\%s", GetScriptPath(), (*it).c_str(), file);
		if(includes.count(string(path)))
			return true;
	}
	return false;
}

bool Script::Include(const char* file)
{
	if(IsIncluded(file))
		return false;

	for(IncludePathIterator it = includeDirs.begin(); it != includeDirs.end(); it++)
	{
		char path[_MAX_PATH + _MAX_FNAME];
		sprintf(path, "%s\\%s\\%s", GetScriptPath(), (*it).c_str(), file);
		if(FileExists(path))
		{
			Lock();

			JSScript* script = JS_CompileFile(context, globalObject, path);
			if(script)
			{
				JSObject* obj = JS_NewScriptObject(context, script);
				JS_AddRoot(context, &obj);

				jsval dummy;
				if(!!JS_ExecuteScript(context, globalObject, script, &dummy))
					includes[string(path)] = true;

				JS_RemoveRoot(context, &obj);
			}

			Unlock();
		}
	}
	return IsIncluded(file);
}

JSBool branchBack(JSContext* cx, JSScript* scr)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	while(script->IsPaused())
		Sleep(50);

	if(script->IsAborted())
		return JS_FALSE;

	JSBranchCallback callback = script->GetBranchCallback();
	return callback ? callback(cx, scr) : JS_TRUE;
}

void ScriptThread(void* lpData)
{
	((Script*)lpData)->Run();
}

void EventThread(void* lpData)
{
}
