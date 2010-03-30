#ifndef D2INTERCEPTS_H
#define D2INTERCEPTS_H

#include "D2Structs.h"

void GameDraw_STUB();;
void GameInput_Interception();
void GamePacketReceived_STUB();
UnitAny* myGetSelectedUnit(void);
void Whisper_Intercept();
void GameAttack_STUB();
void PlayerAssignment_STUB();
void GameCrashFix_STUB();
void GameDraw_Interception(void);
void GameActChange_STUB(void);
void GameActChange2_STUB(void);
void GameLeave_STUB(void);

#endif
