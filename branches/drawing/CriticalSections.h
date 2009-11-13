#pragma once

#include "D2BS.h"

class CriticalRoom
{
private:
	bool bEnteredCriticalSection;

public:
	CriticalRoom() : bEnteredCriticalSection(false) {}
	~CriticalRoom() { LeaveSection(); }

	void EnterSection() {
		EnterCriticalSection(&Vars.cRoomSection);
		bEnteredCriticalSection = true;
	}

	void LeaveSection() {
		if(bEnteredCriticalSection) {
			LeaveCriticalSection(&Vars.cRoomSection);
			bEnteredCriticalSection = false;
		}
	}
};

class CriticalMisc
{
private:
	bool bEnteredCriticalSection;

public:
	CriticalMisc() : bEnteredCriticalSection(false) {}
	~CriticalMisc()  { LeaveSection(); }

	void EnterSection() {
		EnterCriticalSection(&Vars.cMiscSection);
		bEnteredCriticalSection = true;
	}

	void LeaveSection() {
		if(bEnteredCriticalSection) {
			LeaveCriticalSection(&Vars.cMiscSection);
			bEnteredCriticalSection = false;
		}
	}
};
