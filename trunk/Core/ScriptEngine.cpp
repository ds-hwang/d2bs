#define EXPORTING

#include "ScriptEngine.h"
#include <algorithm>

using namespace std;

EXPORT ScriptEngine::ScriptEngine(const char* path, unsigned int gctime)
{
	runtime = JS_NewRuntime(gctime);
	// normalize the path
	this->path = string(path);
}

EXPORT ScriptEngine::~ScriptEngine()
{
	auto end = scripts.end();
	for(auto it = scripts.begin(); it != end; it++)
		delete it->second;

	JS_DestroyRuntime(runtime);
	JS_ShutDown();
}

EXPORT Script* ScriptEngine::Compile(const char* file, bool recompile)
{
	// normalize the file name
	int len = strlen(file);
	char* fname = new char[len+1];
	std::replace_copy(file, file+len, fname, '/', '\\');
	std::transform(file, file+len, fname, tolower);

	if(!recompile && scripts.find(fname) != scripts.end())
	{
		return scripts[fname];
	}

	char fullpath[MAX_PATH];
	sprintf_s(fullpath, MAX_PATH, "%s\\%s", path.c_str(), fname);
	Script* script = new Script(fullpath);
	scripts[fname] = script;

	return script;
}
