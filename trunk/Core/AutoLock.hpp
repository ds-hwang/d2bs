#include <Windows.h>

class AutoLock {
private:
	LPCRITICAL_SECTION lock;
	AutoLock(const AutoLock&);
	AutoLock& operator=(const AutoLock&);
    static void *operator new(size_t) { return 0; };
    static void operator delete(void *, size_t) { };

public:
	AutoLock(LPCRITICAL_SECTION lock) : lock(lock) { EnterCriticalSection(lock); }
	~AutoLock() { LeaveCriticalSection(lock); }
};