#pragma once

#ifndef __JSUTILITIES_H__
#define __JSUTILITIES_H__

#include "js32.h"
#include "Script.h"
#include "D2Utilities.h"

JSBool InitContext(JSContext* cx, uintN contextOp);
void __fastcall InitScript(Script* script, bool isDisposing);
JSBool branch(JSContext* cx, JSScript* script);
void reportError(JSContext *cx, const char *message, JSErrorReport *report);

#endif