#include <windows.h>
#include "nspr/prthread.h"

void MainThread(LPVOID lpData);
void ScriptThread(LPVOID lpData);
void EventThread(LPVOID lpData);
