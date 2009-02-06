#include <windows.h>
#include "Script.h"
#include "nspr/prthread.h"

#include "debugnew/debug_new.h"

void MainThread(LPVOID lpData)
{
	Script::CompileFile("default.dbj", false)->Start();

	while(Script::IsActive())
	{
		// TODO: Process stuff here, like events.
		Sleep(10);
	}
}
