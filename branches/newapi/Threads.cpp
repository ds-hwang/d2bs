#include <windows.h>
#include "Script.h"
#include "nspr/prthread.h"

#include "debugnew/debug_new.h"

void MainThread(LPVOID lpData)
{
	while(Script::IsActive())
	{
		// TODO: Add the code to execute scripts here...
		Sleep(10);
	}
}

void ScriptThread(LPVOID lpData)
{
}

void EventThread(LPVOID lpData)
{
}
