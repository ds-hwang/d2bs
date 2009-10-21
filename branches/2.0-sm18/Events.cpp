#include "Script.h"
#include "Events.h"
#include "js32.h"

#include "debug_new.h"

void PulseEvent()
{
	if(!Script::IsActive())
		return;

	ScriptList scripts = Script::GetAllScripts();
	for(ScriptIterator it = scripts.begin(); it != scripts.end(); it++)
	{
		JSContext* cx = (*it)->GetContext();
		jsval time = JSVAL_VOID;
		JS_NewNumberValue(cx, (jsdouble)GetTickCount(), &time);

		jsval args[] = {time};
		(*it)->ExecEvent("pulse", 1, args);
	}
}
