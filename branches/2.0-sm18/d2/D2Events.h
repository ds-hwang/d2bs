#pragma once

#ifndef __D2EVENTS_H__
#define __D2EVENTS_H__

#include <windows.h>

bool KeyPressEvent(WPARAM key, bool up);
bool MouseClickEvent(unsigned short button, POINT& pt, bool up);
void MouseMoveEvent(POINT& pt);
void CopyDataEvent(DWORD mode, const char* msg);

#endif
