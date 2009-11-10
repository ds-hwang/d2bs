#include "AutoRoot.h"
#include "ScriptEngine.h"

AutoRoot::AutoRoot(jsval nvar) : var(nvar) { JS_AddRoot(&var); }
AutoRoot::~AutoRoot() { JS_RemoveRoot(&var); }
