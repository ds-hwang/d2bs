#pragma once

#ifndef __INTERCEPTS_H__
#define __INTERCEPTS_H__

#include "Structs.h"

void GameMinimize_Intercept();
void GameInput_Intercept();
void GameInternalDraw_Intercept();
void GamePacketReceived_Intercept();
UnitAny* GameGetSelectedUnit_Intercept(void);
void GameExternalDraw_Intercept(void);
void GameWhisper_Intercept();
void GameAttack_Intercept();
void GameCrashFix_Intercept();
void GamePlayerAssign_Intercept();
void ClientShutdown_Intercept(void);

#endif
