#include <windows.h>

#include "D2Events.h"

#include "Script.h"

#include "js32.h"

#include "debug_new.h"

bool KeyPressEvent(WPARAM mkey, bool bUp)
{
	if(!Script::IsActive())
		return false;

	ScriptList scripts = Script::GetAllScripts();
	for(ScriptIterator it = scripts.begin(); it != scripts.end(); it++)
	{
		JSContext* cx = (*it)->GetContext();
		jsval key = JSVAL_VOID;
		JS_NewNumberValue(cx, (jsdouble)mkey, &key);

		jsval args[] = {key};
		(*it)->ExecEvent((bUp ? "keyup" : "keydown"), 1, args);
	}
	return false;
}

bool MouseClickEvent(unsigned short mbutton, POINT& pt, bool bUp)
{
	if(!Script::IsActive())
		return false;

	ScriptList scripts = Script::GetAllScripts();
	for(ScriptIterator it = scripts.begin(); it != scripts.end(); it++)
	{
		JSContext* cx = (*it)->GetContext();
		jsval button = JSVAL_VOID, x = JSVAL_VOID, y = JSVAL_VOID;
		JS_NewNumberValue(cx, (jsdouble)mbutton, &button);
		JS_NewNumberValue(cx, (jsdouble)pt.x, &x);
		JS_NewNumberValue(cx, (jsdouble)pt.y, &y);

		jsval args[] = {button, x, y};
		(*it)->ExecEvent((bUp ? "mouseup" : "mousedown"), 3, args);
	}
	return false;
}

void MouseMoveEvent(POINT& pt)
{
	if(!Script::IsActive())
		return;

	ScriptList scripts = Script::GetAllScripts();
	for(ScriptIterator it = scripts.begin(); it != scripts.end(); it++)
	{
		JSContext* cx = (*it)->GetContext();
		jsval x = JSVAL_VOID, y = JSVAL_VOID;
		JS_NewNumberValue(cx, (jsdouble)pt.x, &x);
		JS_NewNumberValue(cx, (jsdouble)pt.y, &y);

		jsval args[] = {x, y};
		(*it)->ExecEvent("mousemove", 2, args);
	}
}

void CopyDataEvent(DWORD mode, const char* msg)
{
	if(!Script::IsActive())
		return;

	ScriptList scripts = Script::GetAllScripts();
	for(ScriptIterator it = scripts.begin(); it != scripts.end(); it++)
	{
		JSContext* cx = (*it)->GetContext();
		jsval cdMode = JSVAL_VOID, cdMsg = JSVAL_VOID;
		JS_NewNumberValue(cx, (jsdouble)mode, &cdMode);
		cdMsg = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, msg));

		jsval args[] = {cdMode, cdMsg};
		(*it)->ExecEvent("copydata", 2, args);
	}
}
