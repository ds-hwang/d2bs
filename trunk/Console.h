#pragma once
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "Screenhook.h"
#include "D2BS.h"
#include <vector>
#include <string>

class Console
{
private:
	static bool visible, initialized;
	static std::vector<std::string> lines;
	static BoxHook* box;
	static TextHook* text;
	static LineHook* cursor;
	static TextHook* lineBuffers[14];

public:
	static void Initialize(void);
	static void Destroy(void);
	static bool IsReady(void) { return initialized; }

	static void Toggle(void) { if(IsVisible()) Hide(); else Show(); }
	static void Hide(void)
	{
		visible = false;

		box->SetIsVisible(false);
		text->SetIsVisible(false);
		cursor->SetIsVisible(false);
		for(int i = 0; i < 15; i++)
			lineBuffers[i]->SetIsVisible(false);

		Vars.image->SetY(10);
		Vars.text->SetY(15);
	}
	static void Show(void)
	{
		visible = true;

		box->SetIsVisible(true);
		text->SetIsVisible(true);
		cursor->SetIsVisible(true);
		for(int i = 0; i < 15; i++)
			lineBuffers[i]->SetIsVisible(true);

		Vars.image->SetY(box->GetYSize()+9);
		Vars.text->SetY(box->GetYSize()+14);
	}
	static bool IsVisible(void) { return visible; }

	static void AddKey(unsigned int key);
	static void ExecuteCommand(void);
	static void RemoveLastKey(void);
	static void AddLine(std::string line);
	static void Clear(void);
	static void Draw(void);
};

#endif
