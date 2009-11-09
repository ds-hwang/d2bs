#pragma once

#ifndef __SM_ENGINE__
#define __SM_ENGINE__

#include "Engine.h"

#define XP_WIN

#include "jsapi.h"

namespace botsys
{

class SMEngine : Engine
{
private:
	JSRuntime* runtime;

public:
	SMEngine() { runtime = NULL; }
};

}

#endif
