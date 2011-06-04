#pragma once
#include <windows.h>

#include "js32.hpp"
#define EXPORTING
#include "Exports.hpp"
#undef EXPORTING

#include "Engine.hpp"
#include "Script.hpp"

namespace Core {

class Module {
private:
	Module(const Module&);
	Module& operator=(const Module&);

	JSContext* cx;
	JSObject *obj, *exports, *module;
	bool ready;

	friend Engine;

	Module(JSContext* cx, const wchar_t* path, Engine* engine);
	Module(JSContext* cx, JSObject* obj, JSModuleSpec* module);
	~Module();

public:
	EXPORT inline JSObject* GetExports(void) { return exports; }
	EXPORT inline JSObject* GetModule(void) { return module; }
	EXPORT inline bool IsReady(void) { return ready; }
};

}
