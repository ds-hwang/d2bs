#pragma once
#include "js32.hpp"

JSAPI_FUNC(Socket);

extern JSClass socket_class;
extern JSClassSpec socket_spec;
extern JSFunctionSpec socket_methods[];
extern JSPropertySpec socket_props[];
extern JSFunctionSpec socket_static_methods[];
