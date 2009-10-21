#pragma once

#ifndef __WINDOWSHOOKS_H__
#define __WINDOWSHOOKS_H__

#include <windows.h>

LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyPress(int code, WPARAM wParam, LPARAM lParam);
LONG WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
