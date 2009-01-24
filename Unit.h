#pragma once

UnitAny* GetUnit(CHAR* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode, DWORD dwUnitId);
UnitAny* GetNextUnit(UnitAny* pUnit, CHAR* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode);
BOOL CheckUnit(UnitAny* pUnit, CHAR* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode, DWORD dwUnitId);
