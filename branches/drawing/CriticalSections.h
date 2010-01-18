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
		EnterCriticalSection(&Vars.cGameLoopSection);
		bEnteredCriticalSection = true;
	}

	void LeaveSection() {
		if(bEnteredCriticalSection) {
			LeaveCriticalSection(&Vars.cGameLoopSection);
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
		EnterCriticalSection(&Vars.cGameLoopSection);
		bEnteredCriticalSection = true;
	}

	void LeaveSection() {
		if(bEnteredCriticalSection) {
			LeaveCriticalSection(&Vars.cGameLoopSection);
			bEnteredCriticalSection = false;
		}
	}
};
