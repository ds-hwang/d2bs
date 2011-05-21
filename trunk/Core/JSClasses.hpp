#pragma once

#include "js32.hpp"

struct JSClassSpec {
	JSClass* classp;
	JSFunctionSpec* methods;
	JSPropertySpec* properties;
	JSFunctionSpec* static_methods;
	JSPropertySpec* static_properties;
};

extern JSClassSpec classes[];

extern JSClass global;
