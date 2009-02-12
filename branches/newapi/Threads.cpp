#include <windows.h>

#include "helpers.h"
#include "Script.h"
#include "Events.h"
#include "prthread.h"

#include "debug_new.h"

void MainThread(void* lpData)
{
	Script* script = Script::CompileFile("default.dbj", false);
	if(script)
		script->Start();

	while(Script::IsActive())
	{
		// TODO: Process stuff here, like events.
		Sleep(1000);
		PulseEvent();
	}
	SetEvent(OpenEvent(EVENT_MODIFY_STATE, FALSE, "D2BS\\ShutdownEvent"));
}
