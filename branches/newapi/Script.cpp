#include "Script.h"

using namespace std;

JSRuntime* Script::runtime = NULL;
CRITICAL_SECTION Script::section = {0};
ScriptMap Script::scripts = ScriptMap();

Script::Script(ScriptType newType, const char* fname) :
	runCount(0), type(newType), file(string(fname)), scriptObject(NULL),
	globalObject(NULL), meObject(NULL), context(NULL), script(NULL)
{
}

Script::~Script(void)
{
}

Script* Script::CompileFile(const char* file, bool recompile)
{
	string fname = string(file);
	return new Script(File, file);
}

Script* Script::CompileCommand(const char* file, bool recompile)
{
	return new Script(Command, file);
}

void Script::Startup(void)
{
	if(!runtime)
	{
		runtime = JS_NewRuntime(0x800000);
		InitializeCriticalSection(&section);
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
