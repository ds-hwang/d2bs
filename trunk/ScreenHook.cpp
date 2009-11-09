#include "ScreenHook.h"
#include "JSScreenHook.h"
#include "D2BS.h"

using namespace std;

HookList Genhook::hooks = HookList();
CRITICAL_SECTION Genhook::globalSection = {0};

HookList Genhook::GetHooks(void)
{
	EnterCriticalSection(&globalSection);
	HookList currentHooks;
	for(HookIterator it = hooks.begin(); it != hooks.end(); it++)
		currentHooks.push_back(*it);
	LeaveCriticalSection(&globalSection);
	return currentHooks;
}

bool zOrderSort(Genhook* first, Genhook* second)
{
	return first->GetZOrder() < second->GetZOrder();
}

void Genhook::DrawAll(ScreenhookState type)
{
	EnterCriticalSection(&globalSection);
	HookList currentHooks = GetHooks();
	currentHooks.sort(zOrderSort);
	for(HookIterator it = currentHooks.begin(); it != currentHooks.end(); it++)
		if(((*it)->GetGameState() == type || (*it)->GetGameState() == Perm) && (*it)->GetIsVisible() &&
			(!(*it)->GetIsAutomap() || ((*it)->GetIsAutomap() && (*p_D2CLIENT_AutomapOn))))
		{
			(*it)->Draw();
		}
	LeaveCriticalSection(&globalSection);
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
	EnterCriticalSection(&globalSection);
	HookList currentHooks = GetHooks();
	for(HookIterator it = currentHooks.begin(); it != currentHooks.end(); it++)
	{
		if((*it)->owner == owner)
		{
			(*it)->SetIsVisible(false);
			(*it)->owner = NULL;
		}
	}
	LeaveCriticalSection(&globalSection);
}

Genhook::Genhook(Script* nowner, uint x, uint y, ushort nopacity, bool nisAutomap, Align nalign, ScreenhookState ngameState) :
	owner(nowner), isAutomap(nisAutomap), isVisible(true), alignment(nalign), opacity(nopacity), gameState(ngameState), zorder(1)
{
	InitializeCriticalSection(&hookSection);
	clicked = JSVAL_VOID;
	hovered = JSVAL_VOID;
	SetX(x); SetY(y);
	hooks.push_back(this);
}

Genhook::~Genhook(void) {
	Lock();
	if(!JSVAL_IS_VOID(clicked))
		JS_RemoveRootRT(ScriptEngine::GetRuntime(), &clicked);
	if(!JSVAL_IS_VOID(hovered))
		JS_RemoveRootRT(ScriptEngine::GetRuntime(), &hovered);

	EnterCriticalSection(&globalSection);
	hooks.remove(this);
	LeaveCriticalSection(&globalSection);
	owner = NULL;
	location.x = -1;
	location.y = -1;
	Unlock();
	DeleteCriticalSection(&hookSection);
}

// TODO: make this properly manage the context thread...
bool Genhook::Click(int button, POINT* loc)
{
	if(!IsInRange(loc))
		return false;

	if(!owner || !JSVAL_IS_FUNCTION(owner->GetContext(), clicked))
		return false;

	Lock();

	jsval rval;
	if(JS_AddRoot(owner->GetContext(), &rval) == JS_FALSE)
	{
		Unlock();
		return false;
	}
	jsval args[3] = { INT_TO_JSVAL(button), INT_TO_JSVAL(loc->x), INT_TO_JSVAL(loc->y) };
	if(JS_AddRoot(owner->GetContext(), &args[0]) == JS_FALSE)
	{
		Unlock();
		return false;
	}
	if(JS_AddRoot(owner->GetContext(), &args[1]) == JS_FALSE)
	{
		Unlock();
		return false;
	}
	if(JS_AddRoot(owner->GetContext(), &args[2]) == JS_FALSE)
	{
		Unlock();
		return false;
	}

	JS_CallFunctionValue(owner->GetContext(), owner->GetGlobalObject(), clicked, 3, args, &rval);

	JS_RemoveRoot(owner->GetContext(), &args[0]);
	JS_RemoveRoot(owner->GetContext(), &args[1]);
	JS_RemoveRoot(owner->GetContext(), &args[2]);
	JS_RemoveRoot(owner->GetContext(), &rval);

	bool result = !!!(JSVAL_IS_BOOLEAN(rval) && JSVAL_TO_BOOLEAN(rval));
	Unlock();

	return result;
}

// TODO: make this properly manage the context thread...
void Genhook::Hover(POINT* loc)
{
	if(!IsInRange(loc))
		return;
	if(!owner || !(JSVAL_IS_FUNCTION(owner->GetContext(), hovered)))
		return;

	Lock();

	jsval rval = JSVAL_VOID;
	if(JS_AddRoot(owner->GetContext(), &rval) == JS_FALSE)
	{
		Unlock();
		return;
	}
	jsval args[2] = { INT_TO_JSVAL(loc->x), INT_TO_JSVAL(loc->y) };
	if(JS_AddRoot(owner->GetContext(), &args[0]) == JS_FALSE)
	{
		Unlock();
		return;
	}
	if(JS_AddRoot(owner->GetContext(), &args[1]) == JS_FALSE)
	{
		Unlock();
		return;
	}
	JS_CallFunctionValue(owner->GetContext(), owner->GetGlobalObject(), hovered, 2, args, &rval);

	JS_RemoveRoot(owner->GetContext(), &args[0]);
	JS_RemoveRoot(owner->GetContext(), &args[1]);

	Unlock();
}

void Genhook::SetClickHandler(jsval handler)
{
	Lock();
	if(!JSVAL_IS_VOID(clicked))
		JS_RemoveRoot(owner->GetContext(), &clicked);
	if(JSVAL_IS_FUNCTION(owner->GetContext(), handler))
		clicked = handler;
	if(!JSVAL_IS_VOID(clicked))
	{
		if(JS_AddRoot(owner->GetContext(), &clicked) == JS_FALSE)
		{
			Unlock();
			return;
		}
	}
	Unlock();
}

void Genhook::SetHoverHandler(jsval handler)
{
	Lock();
	if(!JSVAL_IS_VOID(hovered))
		JS_RemoveRoot(owner->GetContext(), &hovered);
	if(JSVAL_IS_FUNCTION(owner->GetContext(), handler))
		hovered = handler;
	if(!JSVAL_IS_VOID(hovered))
	{
		if(JS_AddRoot(owner->GetContext(), &hovered) == JS_FALSE)
		{
			Unlock();
			return;
		}
	}
	Unlock();
}

void TextHook::Draw(void)
{
	if(GetIsVisible() && GetX() != -1 && GetY() != -1)
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
		EnterCriticalSection(&Vars.cTextHookSection);
		myDrawText(text, loc.x, loc.y, color, font);
		LeaveCriticalSection(&Vars.cTextHookSection);
		Unlock();
	}
}

bool TextHook::IsInRange(int dx, int dy)
{
	POINT size = CalculateTextLen(text, font);
	int x = GetX(), y = GetY(), w = size.x, h = size.y,
		xp = x - (alignment != Center ? (alignment != Right ? 0 : w) : w/2);
	return (xp < dx && y > dy && (xp+w) > dx && (y-h) < dy);
}

void TextHook::SetText(const char* ntext)
{
	Lock();
	if(text)
		free(text);
	text = NULL;
	if(ntext)
		text = _strdup(ntext);
	Unlock();
}

void ImageHook::Draw(void)
{
	if(GetIsVisible() && GetX() != -1 && GetY() != -1 && GetImage() != NULL)
	{
		if (IsBadReadPtr(image, sizeof(CellFile)))
			return;
		Lock();
		uint x = GetX(), y = GetY(), w = image->cells[0]->width;
		x += (alignment != Left ? (alignment != Right ? 0 : -1*(w/2)) : w/2);
		POINT loc = {x, y};
		if(GetIsAutomap())
		{
			x *= 32;
			y *= 32;
			ScreenToAutomap(&loc, x, y);
		}
		EnterCriticalSection(&Vars.cImageHookSection);
		myDrawAutomapCell(image, loc.x, loc.y, (BYTE)color);
		LeaveCriticalSection(&Vars.cImageHookSection);
		Unlock();
	}
}

bool ImageHook::IsInRange(int dx, int dy)
{
	if(image)
	{
		int x = GetX();
		int y = GetY();
		int w = image->cells[0]->width;
		int h = image->cells[0]->height;
		int xp = x - (alignment != Left ? (alignment != Right ? w/2 : w) : -1*w);
		int yp = y - (h/2);
		return (xp < dx && yp < dy && (xp+w) > dx && (yp+h) > dy);
	}

	return false;
}

void ImageHook::SetImage(const char* nimage)
{
	Lock();
	if(location)
		free(location);
	delete[] image;

	location = _strdup(nimage);
	image = LoadCellFile(location);
	Unlock();
}

void LineHook::Draw(void)
{
	if(GetIsVisible() && GetX() != -1 && GetY() != -1)
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
		EnterCriticalSection(&Vars.cLineHookSection);
		D2GFX_DrawLine(loc.x, loc.y, sz.x, sz.y, color, 0xFF);
		LeaveCriticalSection(&Vars.cLineHookSection);
		Unlock();
	}
}

void BoxHook::Draw(void)
{
	if(GetIsVisible() && GetX() != -1 && GetY() != -1)
	{
		Lock();
		uint x = GetX(), y = GetY(), x2 = GetXSize(), y2 = GetYSize();
		if(alignment == Center)
		{
			x -= x2/2;
		}
		else if (alignment == Right)
		{
			x += x2/2;
		}
		POINT loc = {x, y};
		POINT sz = {x+x2, y+y2};
		if(GetIsAutomap())
		{
			x *= 32;
			y *= 32;
			x2 *= 32;
			y2 *= 32;
			ScreenToAutomap(&loc, x, y);
			ScreenToAutomap(&sz, x2, y2);
		}
		EnterCriticalSection(&Vars.cBoxHookSection);
		D2GFX_DrawRectangle(loc.x, loc.y, sz.x, sz.y, color, opacity);
		LeaveCriticalSection(&Vars.cBoxHookSection);
		Unlock();
	}
}

bool BoxHook::IsInRange(int dx, int dy)
{
	int x = GetX(), y = GetY(), x2 = GetXSize(), y2 = GetYSize();
	return (x < dx && y < dy && (x+x2) > dx && (y+y2) > dy);
}

void FrameHook::Draw(void)
{
	if(GetIsVisible() && GetX() != -1 && GetY() != -1)
	{
		Lock();
		uint x = GetX(), y = GetY(), x2 = GetXSize(), y2 = GetYSize();
		if(alignment == Center)
		{
			x -= x2/2;
		}
		else if (alignment == Right)
		{
			x += x2/2;
		}
		RECT rect = {x, y, x+x2, y+y2};
		EnterCriticalSection(&Vars.cFrameHookSection);
		D2GFX_DrawFrame(&rect);
		LeaveCriticalSection(&Vars.cFrameHookSection);
		Unlock();
	}
}

bool FrameHook::IsInRange(int dx, int dy)
{
	int x = GetX(), y = GetY(), x2 = GetXSize(), y2 = GetYSize();
	return (x < dx && y < dy && (x+x2) > dx && (y+y2) > dy);
}
