#ifndef D2INTERCEPTS_H
#define D2INTERCEPTS_H

VOID GameDraw_STUB();;
VOID GameInput_Interception();
VOID GameMinimize_Interception();
VOID GamePacketReceived_STUB();
UnitAny* myGetSelectedUnit(VOID);
VOID Whisper_Intercept();
VOID GameAttack_STUB();
VOID PlayerAssignment_STUB();
VOID GameCrashFix_STUB();

#endif
