#pragma once
#include "js32.hpp"

JSAPI_FUNC(Pipe);

extern JSClass pipe_class;
extern JSClassSpec pipe_spec;
extern JSFunctionSpec pipe_methods[];
extern JSPropertySpec pipe_props[];
extern JSFunctionSpec pipe_static_methods[];
