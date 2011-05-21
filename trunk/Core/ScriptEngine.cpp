#define EXPORTING

#include "ScriptEngine.hpp"
#include <algorithm>
#include <assert.h>

#include "JSClasses.hpp"

using namespace std;

EXPORT ScriptEngine::ScriptEngine(const char* path, unsigned int gctime)
{
	JS_SetCStringsAreUTF8();
	runtime = JS_NewRuntime(gctime);

	// normalize the path
	int len = strlen(path);
	char* pname = new char[len+1];
	std::replace_copy(path, path+len, pname, '/', '\\');
	std::transform(pname, pname+len, pname, tolower);
	this->path = string(path);

	JS_SetContextCallback(runtime, ScriptEngine::ContextCallback);

	// since we're almost guaranteed to need at least one context, go ahead and spin one up
	ReleaseContext(GetContext());
}

EXPORT ScriptEngine::~ScriptEngine()
{
	auto end = scripts.end();
	for(auto it = scripts.begin(); it != end; it++)
		delete it->second;

	JS_DestroyRuntime(runtime);
	JS_ShutDown();
}

JSContext* ScriptEngine::GetContext()
{
	JSContext* cx = NULL;
	if(free.size() > 0)
	{
		JSContext* cx = free.front();
		free.remove(cx);
		held.push_back(cx);
	}
	else
	{
		JSContext* cx = JS_NewContext(runtime, 0x2000);
		held.push_back(cx);
	}

	return cx;
}

void ScriptEngine::ReleaseContext(JSContext* cx)
{
	assert(std::find(held.begin(), held.end(), cx) != held.end());
	held.remove(cx);
	free.push_back(cx);
}

EXPORT Script* ScriptEngine::Compile(const char* file, bool recompile)
{
	// normalize the file name
	int len = strlen(file);
	char* fname = new char[len+1];
	std::replace_copy(file, file+len, fname, '/', '\\');
	std::transform(fname, fname+len, fname, tolower);

	if(!recompile && scripts.find(fname) != scripts.end()) return scripts[fname];

	char fullpath[MAX_PATH];
	sprintf_s(fullpath, MAX_PATH, "%s\\%s", path.c_str(), fname);
	Script* script = new Script(fullpath, this);
	scripts[fname] = script;

	return script;
}

void ScriptEngine::InitClasses(Script* script)
{
	for(JSClassSpec* classp = classes; classp->classp != NULL; classp++)
		JS_InitClass(script->cx, script->obj, NULL, classp->classp,
			classp->classp->construct, 0, classp->properties, classp->methods,
			classp->static_properties, classp->static_methods);

	// TODO: let the engine consumer supply a callback to call here for init'ing their
	// own classes
}

JSBool ScriptEngine::ContextCallback(JSContext *cx, uintN contextOp)
{
	if(contextOp != JSCONTEXT_NEW) return JS_TRUE;

	// set options, register handlers
	JS_SetOperationCallback(cx, Script::OperationCallback);
	JS_SetErrorReporter(cx, ScriptEngine::ErrorReporter);
	JS_SetVersion(cx, JSVERSION_LATEST);
	JS_SetOptions(cx, JSOPTION_JIT|
					  JSOPTION_METHODJIT|
					  JSOPTION_RELIMIT|
					  JSOPTION_NO_SCRIPT_RVAL|
					  JSOPTION_VAROBJFIX|
					  JSOPTION_XML|
					  JSOPTION_STRICT|
					  JSOPTION_ANONFUNFIX);

	return JS_TRUE;
}

void ScriptEngine::ErrorReporter(JSContext *cx, const char *message, JSErrorReport *report)
{
	// TODO: let the engine consumer supply a callback to call here for displaying
	// the error however they desire
}
