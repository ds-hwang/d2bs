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
//		EnterCriticalSection(&Vars.cRoomSection);
		EnterCriticalSection(*p_D2COMMON_CriticalRoomSection);
		bEnteredCriticalSection = true;
	}

	void LeaveSection() {
		if(bEnteredCriticalSection) {
			LeaveCriticalSection(*p_D2COMMON_CriticalRoomSection);
//			LeaveCriticalSection(&Vars.cRoomSection);
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
		EnterCriticalSection(p_D2NET_CriticalPacketSection);
//		EnterCriticalSection(&Vars.cMiscSection);
		bEnteredCriticalSection = true;
	}

	void LeaveSection() {
		if(bEnteredCriticalSection) {
			LeaveCriticalSection(p_D2NET_CriticalPacketSection);
//			LeaveCriticalSection(&Vars.cMiscSection);
			bEnteredCriticalSection = false;
		}
	}
};
