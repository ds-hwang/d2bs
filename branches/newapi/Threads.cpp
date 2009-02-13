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
		// this processes at 50 just fine, but has an upper bound of ~75 events/sec
		PR_Sleep(1000);
		PulseEvent();
	}
	HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, "Local\\D2BSShutdownEvent");
	SetEvent(hEvent);
	CloseHandle(hEvent);
}
