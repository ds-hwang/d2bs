#pragma once
#include "js32.hpp"

JSAPI_FUNC(File);

extern JSClass file_class;
extern JSClassSpec file_spec;
extern JSFunctionSpec file_methods[];
extern JSPropertySpec file_props[];
extern JSFunctionSpec file_static_methods[];
