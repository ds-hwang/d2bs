#include "Engine.hpp"

#include "Stream.hpp"

#include <io.h>
#include <direct.h>
#include <process.h>
#include <iostream>

#include <boost/make_shared.hpp>

using namespace std;
using namespace Core;

JSAPI_FUNC(console_print);
JSAPI_FUNC(console_sleep);

JSAPI_EMPTY_CTOR(Console)

JSClass console = { "Console", 0, JSCLASS_DEFAULT_WITH_CTOR(Console) };
JSFunctionSpec console_methods[] = {
	JS_FS("print", console_print, 1, JSPROP_STATIC),
	JS_FS("sleep", console_sleep, 1, JSPROP_STATIC),
	JS_FS_END
};

JSClassSpec console_classes[] = {
	JS_CS(&console, nullptr, Console, 0, nullptr, nullptr, console_methods, nullptr),
	JS_CS_END
};

JSModuleSpec console_mods[] = {
	JS_MS("console", console_classes, nullptr, nullptr),
	JS_MS_END
};

JSAPI_FUNC(console_print)
{
	JSString* str = nullptr;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &str))
		return JS_FALSE;

	JS::Anchor<JSString*> anchor(str);
	const jschar* output = JS_GetStringCharsZ(cx, str);
	wcout << output << endl;
	return JS_TRUE;
}

JSAPI_FUNC(console_sleep)
{
	uint32 amount = 1;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "u", &amount))
		return JS_FALSE;
	Sleep(amount);
	return JS_TRUE;
}

void InitClasses(JSContext* cx, JSObject* obj)
{
	// no global classes to init
}

void reporter(JSContext *cx, const char *message, JSErrorReport *report)
{
	cout << "[";
	if(JSREPORT_IS_STRICT(report->flags)) cout << "Strict ";
	if(JSREPORT_IS_WARNING(report->flags)) cout << "Warning";
	else cout << "Error";
	cout << "] ";

	if(report->filename != nullptr) cout << "At " << report->filename << ":" << report->lineno;
	cout << " (" << report->errorNumber << ") " << message << endl;
}

int main(int argc, const char** argv)
{
	wchar_t path[MAX_PATH] = L"", script[MAX_PATH] = L"";
	_wgetcwd(path, MAX_PATH);
	swprintf_s(script, MAX_PATH, L"%s\\%s", path, L"test.js");

	if(_waccess(script, 0) == -1) return 0;

	Engine engine(path, 0x80000, InitClasses, reporter);
	engine.RegisterModule(console_mods);
	engine.RegisterModule(stream_modules);

	cout << "Starting test.js" << endl;
	engine.CompileScript(L"test.js")->Start();

	getchar();

	return 0;
}
