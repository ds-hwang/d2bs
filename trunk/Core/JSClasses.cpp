#include "JSClasses.hpp"


JSClass global = { "Global", JSCLASS_GLOBAL_FLAGS, JSCLASS_DEFAULT_STANDARD_MEMBERS };

JSClassSpec classes[] = {
	{&global, NULL, NULL, NULL, NULL},
};
