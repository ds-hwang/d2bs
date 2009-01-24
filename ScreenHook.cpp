#include "D2BS.h"
#include "JSScreenHook.h"
#include <vector>

using namespace std;

HookList Genhook::hooks = HookList();

HookList Genhook::GetHooks(void)
{
	HookList currentHooks;
	for(HookIterator it = hooks.begin(); it != hooks.end(); it++)
		currentHooks.push_back(*it);
	return currentHooks;
}

void Genhook::DrawAll(ScreenhookState type)
{
	for(HookIterator it = hooks.begin(); it != hooks.end(); it++)
		if(((*it)->GetGameState() == type || (*it)->GetGameState() == Perm) && (*it)->GetIsVisible())
			(*it)->Draw();
}

HookIterator Genhook::GetFirstHook()
{
	return hooks.begin();
}

HookIterator Genhook::GetLastHook()
{
	return hooks.end();
}

void Genhook::Clean(Script* owner)
{
	HookList currentHooks = GetHooks();
	for(HookIterator it = currentHooks.begin(); it != currentHooks.end(); it++)
		if((*it)->owner == owner)
			delete *it;
}

bool Genhook::Click(int button, POINT* loc)
{
	if(!IsInRange(loc))
		return false;

	if(!owner || !(JSVAL_IS_OBJECT(clicked) && JS_ObjectIsFunction(owner->GetContext(), JSVAL_TO_OBJECT(clicked))))
		return false;

	Lock();

	JS_SetContextThread(owner->GetContext());
	jsval rval;
	JS_AddRoot(owner->GetContext(), &rval);
	jsval args[3] = { INT_TO_JSVAL(button), INT_TO_JSVAL(loc->x), INT_TO_JSVAL(loc->y) };
	JS_AddRoot(owner->GetContext(), &args[0]);
	JS_AddRoot(owner->GetContext(), &args[1]);
	JS_AddRoot(owner->GetContext(), &args[2]);

	JS_CallFunctionValue(owner->GetContext(), owner->GetGlobalObject(), clicked, 3, args, &rval);

	JS_RemoveRoot(owner->GetContext(), &args[0]);
	JS_RemoveRoot(owner->GetContext(), &args[1]);
	JS_RemoveRoot(owner->GetContext(), &args[2]);

	bool result = !!!(JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
	JS_ClearContextThread(owner->GetContext());
	Unlock();

	return result;
}

void Genhook::Hover(POINT* loc)
{
	if(!IsInRange(loc))
		return;
	if(!owner || !(JSVAL_IS_OBJECT(hovered) && JS_ObjectIsFunction(owner->GetContext(), JSVAL_TO_OBJECT(hovered))))
		return;

	Lock();
	JS_SetContextThread(owner->GetContext());

	jsval rval = JSVAL_VOID;
	JS_AddRoot(owner->GetContext(), &rval);
	jsval args[2] = { INT_TO_JSVAL(loc->x), INT_TO_JSVAL(loc->y) };
	JS_AddRoot(owner->GetContext(), &args[0]);
	JS_AddRoot(owner->GetContext(), &args[1]);

	JS_CallFunctionValue(owner->GetContext(), owner->GetGlobalObject(), hovered, 2, args, &rval);

	JS_RemoveRoot(owner->GetContext(), &args[0]);
	JS_RemoveRoot(owner->GetContext(), &args[1]);

	JS_ClearContextThread(owner->GetContext());
	Unlock();
}

void TextHook::Draw(void)
{
	if(GetX() != -1 && GetY() != -1)
	{
		Lock();
		uint x = GetX(), y = GetY(), w = CalculateTextLen(text, font).x;
		x -= (alignment != Center ? (alignment != Right ? 0 : w) : w/2);
		POINT loc = {x, y};
		if(GetIsAutomap())
		{
			x *= 32;
			y *= 32;
			ScreenToAutomap(&loc, x, y);
		}
		myDrawText(text, loc.x, loc.y, color, font);
		Unlock();
	}
}

void ImageHook::Draw(void)
{
	if(GetX() != -1 && GetY() != -1)
	{
		Lock();
		if (IsBadReadPtr(image, sizeof(CellFile)))
			return;
		uint x = GetX(), y = GetY(), w = image->cells[0]->width;
		x += (alignment != Left ? (alignment != Right ? 0 : -1*(w/2)) : w/2);
		POINT loc = {x, y};
		if(GetIsAutomap())
		{
			x *= 32;
			y *= 32;
			ScreenToAutomap(&loc, x, y);
		}
		myDrawAutomapCell(image, loc.x, loc.y, (BYTE)color);
		Unlock();
	}
}

void LineHook::Draw(void)
{
	if(GetX() != -1 && GetY() != -1)
	{
		Lock();
		uint x = GetX(), y = GetY(), x2 = GetX2(), y2 = GetY2();
		POINT loc = {x, y};
		POINT sz = {x2, y2};
		if(GetIsAutomap())
		{
			x *= 32;
			y *= 32;
			x2 *= 32;
			y2 *= 32;
			ScreenToAutomap(&loc, x, y);
			ScreenToAutomap(&sz, x2, y2);
		}
		D2GFX_DrawLine(loc.x, loc.y, sz.x, sz.y, color, 0xFF);
		Unlock();
	}
}

void BoxHook::Draw(void)
{
	if(GetX() != -1 && GetY() != -1)
	{
		Lock();
		uint x = GetX(), y = GetY(), x2 = GetX2(), y2 = GetY2();
		POINT loc = {x, y};
		POINT sz = {x2, y2};
		if(GetIsAutomap())
		{
			x *= 32;
			y *= 32;
			x2 *= 32;
			y2 *= 32;
			ScreenToAutomap(&loc, x, y);
			ScreenToAutomap(&sz, x2, y2);
		}
		D2GFX_DrawRectangle(loc.x, loc.y, sz.x, sz.y, color, opacity);
		Unlock();
	}
}

void FrameHook::Draw(void)
{
	if(GetX() != -1 && GetY() != -1)
	{
		Lock();
		uint x = GetX(), y = GetY(), x2 = GetX2(), y2 = GetY2();
		RECT rect = {x, y, x2, y2};
		D2GFX_DrawFrame(&rect);
		Unlock();
	}
}



VOID CleanScreenhooks(bool bAll)
{
	if(Vars.ScreenhookList.empty())
		return;

	EnterCriticalSection(&Vars.cScreenhookSection);

	for(vector<Screenhook*>::iterator it = Vars.ScreenhookList.begin(); it != Vars.ScreenhookList.end(); it++)
	{
		Screenhook* pScreenhook = *it;

		if(!pScreenhook->bOutOfGame || bAll)
		{
			
			if(pScreenhook->pCellImage)
			{
				delete pScreenhook->pCellImage;
			}

			if(pScreenhook->pText)
			{
				delete[] pScreenhook->pText;
			}

			delete pScreenhook;
			Vars.ScreenhookList.erase(it);
		}
	}

	LeaveCriticalSection(&Vars.cScreenhookSection);
}

VOID DrawScreenhook(Screenhook* pScreenhook)
{
	if(!pScreenhook)
		return;

	if(pScreenhook->dwType >= SH_AUTOMAPLINE && pScreenhook->dwType <= SH_AUTOMAPIMAGE && D2CLIENT_GetUIState(UI_AUTOMAP))
	{
		POINT lpCoord1 = {pScreenhook->dwX * 32, pScreenhook->dwY * 32};
		POINT lpCoord2 = {pScreenhook->dwX2 * 32, pScreenhook->dwY2 * 32};

		if(pScreenhook->dwX != -1 && pScreenhook->dwY != -1)
			ScreenToAutomap(&lpCoord1, lpCoord1.x, lpCoord1.y);

		if(pScreenhook->dwX2 != -1 && pScreenhook->dwY2 != -1)
			ScreenToAutomap(&lpCoord1, lpCoord2.x, lpCoord2.y);

		if(pScreenhook->dwType == SH_AUTOMAPLINE && pScreenhook->dwX != -1 && pScreenhook->dwY != -1 && 
			pScreenhook->dwX2 != -1 && pScreenhook->dwY2 != -1)
		{
			D2GFX_DrawLine(lpCoord1.x, lpCoord1.y,
				lpCoord2.x, lpCoord2.y, pScreenhook->dwColor, 0xFF);
		}
		else if(pScreenhook->dwType == SH_AUTOMAPBOX && pScreenhook->dwX != -1 && pScreenhook->dwY != -1
					&& pScreenhook->dwX2 != -1 && pScreenhook->dwY2 != -1)
		{
			D2GFX_DrawRectangle(lpCoord1.x, lpCoord1.y, lpCoord2.x, lpCoord2.y,
				pScreenhook->dwColor, pScreenhook->dwOpacity);
		}
		else if(pScreenhook->dwType == SH_AUTOMAPIMAGE && pScreenhook->pCellImage && pScreenhook->dwX != -1 && pScreenhook->dwY != -1)
		{
			myDrawAutomapCell(pScreenhook->pCellImage, lpCoord1.x, lpCoord1.y, (BYTE)pScreenhook->dwColor);
		}

		if(pScreenhook->pText && pScreenhook->dwX != -1 && pScreenhook->dwY != -1 && pScreenhook->dwFont != -1 && pScreenhook->dwColor != -1)
		{
			myDrawText(pScreenhook->pText, lpCoord1.x, lpCoord1.y, pScreenhook->dwColor, pScreenhook->dwFont);
		}
	}
	else
	{
		if(pScreenhook->dwType == SH_LINE && pScreenhook->dwX != -1 && pScreenhook->dwY != -1 && 
			pScreenhook->dwX2 != -1 && pScreenhook->dwY2 != -1)
		{
			D2GFX_DrawLine(pScreenhook->dwX, pScreenhook->dwY, pScreenhook->dwX2, pScreenhook->dwY2, pScreenhook->dwColor, 0xFF);
		}
		else if(pScreenhook->dwType == SH_BOX && pScreenhook->dwX != -1 && pScreenhook->dwY != -1
					&& pScreenhook->dwX2 != -1 && pScreenhook->dwY2 != -1)
		{
			D2GFX_DrawRectangle(pScreenhook->dwX, pScreenhook->dwY, pScreenhook->dwX2, pScreenhook->dwY2, pScreenhook->dwColor, pScreenhook->dwOpacity);
		}
		else if(pScreenhook->dwType == SH_IMAGE && pScreenhook->pCellImage && pScreenhook->dwX != -1 && pScreenhook->dwY != -1)
		{
			myDrawAutomapCell(pScreenhook->pCellImage, pScreenhook->dwX, pScreenhook->dwY, (BYTE)pScreenhook->dwColor);
		}

		if(pScreenhook->pText && pScreenhook->dwX != -1 && pScreenhook->dwY != -1 && pScreenhook->dwFont != -1 && pScreenhook->dwColor != -1)
		{
			myDrawText(pScreenhook->pText, pScreenhook->dwX, pScreenhook->dwY, pScreenhook->dwColor, pScreenhook->dwFont);
		}
	}
}
