#pragma once

#ifndef __AUTOROOT_H__
#define __AUTOROOT_H__

#include "js32.h"
#include "yasper.h"

class AutoRoot
{
private:
	jsval var;

	AutoRoot(const AutoRoot&);
	AutoRoot& operator=(const AutoRoot&);
public:
	AutoRoot() {}
	AutoRoot(jsval var);
	~AutoRoot();
	jsval value() { return var; }
	jsval operator* () { return value(); }
	bool operator==(AutoRoot& other) { return value() == other.value(); }
};

typedef yasper::ptr<AutoRoot> AutoRootPtr;

#endif
