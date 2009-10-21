#pragma once

#ifndef __UNITS_H__
#define __UNITS_H__

class Unit {
	protected:
		UnitAny* pUnit;
		virtual BOOL IsValid() { return IsBadReadPtr(pUnit, sizeof(UnitAny)); }

	public:
		Unit(UnitAny* unit) { pUnit = unit; };

		DWORD GetType () { return ((IsValid()) ? pUnit->dwType : -1);};
		DWORD GetMode () { return ((IsValid()) ? pUnit->dwMode : -1);};
		DWORD GetUnitId () { return ((IsValid()) ? pUnit->dwUnitId : -1);};
		DWORD GetId () { return ((IsValid()) ? pUnit->dwTxtFileNo : -1);};
		DWORD GetAct () { return ((IsValid()) ? pUnit->dwAct + 1 : -1);};
		DWORD GetArea () { return ((IsValid()) ? pUnit->pPath->pRoom1->pRoom2->pLevel->dwLevelNo : -1);};

};

class Player : public Unit {
	protected:
		RosterUnit* pRoster;
		BOOL IsValid() { return (!(IsBadReadPtr(pUnit, sizeof(UnitAny)) || IsBadReadPtr(pRoster, sizeof(RosterUnit)))); }
	public:
		Player(UnitAny* player) : Unit(player) {szName = pUnit->pPlayerData->szName; pRoster = FindPlayerRoster(GetUnitId());};
		CHAR* szName;

		DWORD GetX() { return ((IsValid()) ? pUnit->pPath->xPos : -1);};//Gets Players X Position
		DWORD GetY() { return ((IsValid()) ? pUnit->pPath->yPos : -1);};//Gets Players Y Position
		DWORD GetLevel() { return ((IsValid()) ? GetUnitStat(pUnit, 12, 0) : -1);};//Gets players level
		DWORD GetStrength() { return ((IsValid()) ? GetUnitStat(pUnit, 0, 0) : -1);};//Gets players strength
		DWORD GetDexterity() { return ((IsValid()) ? GetUnitStat(pUnit, 2, 0) : -1);};//Gets players dexterity
		DWORD GetVelocity() { return ((IsValid()) ? GetUnitStat(pUnit, 67, 0) : -1);};//Gets players velocity
		DWORD GetAttackRate() { return ((IsValid()) ? GetUnitStat(pUnit, 68, 0) : -1);};//Gets players attack rate
		DWORD GetClass () { return ((IsValid()) ? pUnit->dwTxtFileNo : -1);};//Gets player class, same as GetId()
		DWORD GetLifePercent () { return ((IsValid()) ? pRoster->dwPartyLife : -1);};//Gets player's life if in range.
		WORD GetPartyId () { return ((IsValid()) ? pRoster->wPartyId : -1);};//Gets the player's party id, 0xFFFF if not partied.
		BOOL IsPartied ();//Checks if they are in the same party as you.
		BOOL IsHostile ();//Checks if they are hostile against you.
		BOOL IsInvited ();//Checks if they have been invited to your party.
		BOOL IsInParty ();//Checks if they are in party, not always your party.
};

#endif