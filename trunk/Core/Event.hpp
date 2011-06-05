#pragma once
#include <Windows.h>
#include <memory>
#include <vector>
#include <boost/any.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

namespace Core {

typedef std::vector<boost::any> ArgumentList;

struct Event {
	const wchar_t* evtName;
	ArgumentList args;
	HANDLE blocker;
	bool result;
};

}
