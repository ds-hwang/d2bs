#pragma once

#include "js32.hpp"

JSAPI_FUNC(mod_load);
JSAPI_FUNC(mod_include);

JSAPI_PROP(get_mod_sp);
JSAPI_STRICT_PROP(set_mod_sp);

extern JSFunctionSpec module_methods[];
extern JSPropertySpec module_props[];
