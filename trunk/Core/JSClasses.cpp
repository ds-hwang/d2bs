#include "JSClasses.hpp"
#include "Modules.hpp"

JSClass global = { "Global", JSCLASS_GLOBAL_FLAGS, JSCLASS_DEFAULT_STANDARD_MEMBERS };

// IMPORTANT: Ordering is critical here! If your object has a
// defined prototype, _THAT PROTOTYPE MUST BE LISTED ABOVE IT_
JSClassSpec classes[] = {
//	classp,		proto,		methods,		props,			static methods,		static props
	{&global,	NULL,		module_methods,	module_props,	NULL,				NULL},
	{0},
};
