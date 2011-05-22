#define EXPORTING

#include "ScriptEngine.hpp"
#include <algorithm>
#include <iterator>
#include <assert.h>

#include "JSClasses.hpp"

using namespace std;

JSErrorReporter ScriptEngine::reporter = 0;

ScriptEngine::ScriptEngine(const wchar_t* path, unsigned int gctime,
						JSClassInitCallback classCallback)
{
	JS_SetCStringsAreUTF8();
	runtime = JS_NewRuntime(gctime);

	// normalize the path
	int len = wcslen(path);
	wchar_t* pname = new wchar_t[len+1];
	std::replace_copy(path, path+len, pname, L'/', L'\\');
	std::transform(pname, pname+len, pname, towlower);
	this->path = std::wstring(path);

	this->classInit = classCallback;

	JS_SetContextCallback(runtime, ScriptEngine::ContextCallback);

	// since we're almost guaranteed to need at least one context, go ahead and spin one up
	ReleaseContext(GetContext());
}

ScriptEngine::~ScriptEngine()
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
	JS_SetContextPrivate(cx, NULL);
	held.remove(cx);
	free.push_back(cx);
}

void ScriptEngine::SetErrorReporter(JSErrorReporter reporter)
{
	ScriptEngine::reporter = reporter;
}

Script* ScriptEngine::Compile(const wchar_t* file, bool recompile)
{
	// normalize the file name
	int len = wcslen(file);
	wchar_t* fname = new wchar_t[len+1];
	std::replace_copy(file, file+len, fname, L'/', L'\\');
	std::transform(fname, fname+len, fname, tolower);

	bool exists = scripts.find(fname) != scripts.end();
	if(!recompile && exists) return scripts[fname];
	else if(recompile && exists) {
		Script* script = scripts[fname];
		script->Stop();
		scripts.erase(fname);
		delete script;
	}

	wchar_t fullpath[MAX_PATH];
	swprintf_s(fullpath, MAX_PATH, L"%s\\%s", path.c_str(), fname);
	Script* script = new Script(fullpath, this);
	scripts[fname] = script;

	return script;
}

void ScriptEngine::InitClasses(Script* script)
{
	for(JSClassSpec* obj = classes; obj->classp != NULL; obj++)
	{
		JSObject* proto = NULL;
		if(obj->proto != NULL)
		{
			// look up the proto on the global object
			jsval jsproto = JSVAL_NULL;
			if(JS_GetProperty(script->cx, script->obj, obj->proto->name, &jsproto))
				if(jsproto != JSVAL_NULL && jsproto != JSVAL_VOID)
					proto = JSVAL_TO_OBJECT(jsproto);
		}

		JS_InitClass(script->cx, script->obj, proto, obj->classp,
			obj->classp->construct, 0, obj->properties, obj->methods,
			obj->static_properties, obj->static_methods);
	}

	classInit(script);
}

void ScriptEngine::FireEvent(const char* evtName, char* format, ...)
{
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
					  JSOPTION_VAROBJFIX|
					  JSOPTION_XML|
					  JSOPTION_STRICT|
					  JSOPTION_ANONFUNFIX);

	return JS_TRUE;
}

void ScriptEngine::ErrorReporter(JSContext *cx, const char *message, JSErrorReport *report)
{
	ScriptEngine::reporter(cx, message, report);
}
