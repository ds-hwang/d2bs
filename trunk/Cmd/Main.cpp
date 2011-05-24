#define EXPORTED
#include "ScriptEngine.hpp"

#include <io.h>
#include <direct.h>
#include <iostream>

JSAPI_FUNC(console_print);

static JSClass console = { "Console", 0, JSCLASS_DEFAULT_STANDARD_MEMBERS };
JSFunctionSpec console_methods[] = {
	JS_FS("print", console_print, 1, 0),
	{0}
};

static JSClassSpec classes[] = {
	{&console, NULL, NULL, NULL, console_methods, NULL},
	{0}
};

JSAPI_FUNC(console_print)
{
	JSString* str = NULL;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &str))
		return JS_FALSE;

	JS::Anchor<JSString*> anchor(str);
	const jschar* output = JS_GetStringCharsZ(cx, str);
	std::wcout << output << std::endl;
	return JS_TRUE;
}

void InitClasses(Script* script)
{
	JS_DefineClasses(script->GetContext(), script->GetGlobalObject(), classes);
}

void reporter(JSContext *cx, const char *message, JSErrorReport *report)
{
	std::cout << "[";
	if(JSREPORT_IS_STRICT(report->flags))
		std::cout << "Strict ";
	if(JSREPORT_IS_WARNING(report->flags))
		std::cout << "Warning";
	else
		std::cout << "Error";

	std::cout << "] ";
	if(report->filename != NULL)
		std::cout << "At " << report->filename << ":" << report->lineno;
	std::cout << " (" << report->errorNumber << ") ";
	std::cout << message << std::endl;
}

int main(int argc, const char** argv)
{
	wchar_t path[MAX_PATH] = L"", script[MAX_PATH] = L"";
	_wgetcwd(path, MAX_PATH);
	swprintf_s(script, MAX_PATH, L"%s\\%s", path, L"test.js");

	if(_waccess(script, 0) != -1)
	{
		ScriptEngine engine(path, 0x80000000, InitClasses);
		ScriptEngine::SetErrorReporter(reporter);
		std::cout << "Starting test.js" << std::endl;
		Script* script = engine.Compile(L"test.js");
		script->Start();
		system("PAUSE");
	}
}
