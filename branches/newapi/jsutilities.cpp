#include "jsutilities.h"
#include "Script.h"
#include "helpers.h"
#include "JSGlobalFuncs.h"
#include "Script.h"

#include "debug_new.h"

JSBool InitContext(JSContext* cx, uintN contextOp)
{
	if(contextOp == JSCONTEXT_NEW)
	{
		JS_SetErrorReporter(cx, reportError);
		JS_SetOptions(cx, JSOPTION_STRICT|JSOPTION_VAROBJFIX|JSOPTION_XML|JSOPTION_NATIVE_BRANCH_CALLBACK);
		JS_SetVersion(cx, JSVERSION_1_7);
	}
	return JS_TRUE;
}

void __fastcall InitScript(Script* script, bool isDisposing)
{
	if(!isDisposing)
	{
		JS_DefineFunctions(script->GetContext(), script->GetGlobalObject(), global_funcs);
	}
}

JSBool branch(JSContext* cx, JSScript* scr)
{
	// TODO: Add script exit conditions (if any)
	return JS_TRUE;
}

void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	bool warn = JSREPORT_IS_WARNING(report->flags);
	bool isStrict = JSREPORT_IS_STRICT(report->flags);
	const char* type = (warn ? "Warning" : "Error");
	const char* strict = (isStrict ? "Strict " : "");
	const char* filename = (report->filename+strlen(Script::GetScriptPath())+1);

	Log("[%s%s] %s/line %d: (%d) %s\n\t\tLine: %s", strict, type, filename, report->lineno,
					report->errorNumber, message, report->linebuf);

	// all potential cases are handled inside Print now
	//Print("[ÿc%d%s%sÿc0] %s/line %d: (%d) %s", (warn ? 9 : 1), strict, type,
	//				filename, report->lineno, report->errorNumber, message);

	//if(Vars.bQuitOnError && D2CLIENT_GetPlayerUnit() && !JSREPORT_IS_WARNING(report->flags))
	//	D2CLIENT_ExitGame();
}
