#include <windows.h>
#include "helpers.h"
#include "Script.h"
#include "nspr/prthread.h"

#include "debugnew/debug_new.h"

void MainThread(void* lpData)
{
	Script* script = Script::CompileFile("default.dbj", false);
	if(script)
		script->Start();

	while(Script::IsActive())
	{
		// TODO: Process stuff here, like events.
		Sleep(10);
	}
}
