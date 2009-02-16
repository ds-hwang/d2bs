#include "ScreenHook.h"
#include "JSScreenHook.h"

#include "debugnew/debug_new.h"

using namespace std;

HookList Genhook::hooks = HookList();

HookList Genhook::GetHooks(void)
{
	HookList currentHooks;
	for(HookIterator it = hooks.begin(); it != hooks.end(); it++)
		currentHooks.push_back(*it);
	return currentHooks;
}

bool zOrderSort(Genhook* first, Genhook* second)
{
	return first->GetZOrder() < second->GetZOrder();
}

void Genhook::DrawAll(ScreenhookState type)
{
	HookList currentHooks = GetHooks();
	currentHooks.sort(zOrderSort);
	for(HookIterator it = currentHooks.begin(); it != currentHooks.end(); it++)
		if(((*it)->GetGameState() == type || (*it)->GetGameState() == Perm) && (*it)->GetIsVisible() &&
			(!(*it)->GetIsAutomap() || ((*p_D2CLIENT_AutomapOn) && (*it)->GetIsAutomap())))
		{
			(*it)->Draw();
		}
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
	{
		if((*it)->owner == owner)
		{
			(*it)->SetIsVisible(false);
		}
	}
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
		JS_RemoveRoot(owner->GetContext(), &clicked);
	if(!JSVAL_IS_VOID(hovered))
		JS_RemoveRoot(owner->GetContext(), &hovered);

	hooks.remove(this);
	Unlock();
	DeleteCriticalSection(&hookSection);
	owner = NULL;
	location.x = -1;
	location.y = -1;
}

bool Genhook::Click(int button, POINT* loc)
{
	if(!IsInRange(loc))
		return false;

	if(!owner || !JSVAL_IS_FUNCTION(owner->GetContext(), clicked))
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
	if(!owner || !(JSVAL_IS_FUNCTION(owner->GetContext(), hovered)))
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

void Genhook::SetClickHandler(jsval handler)
{
	Lock();
	if(!JSVAL_IS_VOID(clicked))
		JS_RemoveRoot(owner->GetContext(), &clicked);
	if(JSVAL_IS_FUNCTION(owner->GetContext(), handler))
		clicked = handler;
	if(!JSVAL_IS_VOID(clicked))
		JS_AddRoot(owner->GetContext(), &clicked);
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
		JS_AddRoot(owner->GetContext(), &hovered);
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

bool ImageHook::IsInRange(int dx, int dy)
{
	int x = GetX(), y = GetY(), w = image->cells[0]->width, h = image->cells[0]->height,
		xp = x - (alignment != Left ? (alignment != Right ? w/2 : w) : -1*w),
		yp = y - (h/2);
	return (xp < dx && yp < dy && (xp+w) > dx && (yp+h) > dy);
}

void ImageHook::SetImage(const char* nimage)
{
	Lock();
	free(location);
	delete[] image;
	location = _strdup(nimage);
	image = LoadCellFile(location);
	Unlock();
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
		uint x = GetX(), y = GetY(), x2 = GetXSize(), y2 = GetYSize();
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

bool BoxHook::IsInRange(int dx, int dy)
{
	int x = GetX(), y = GetY(), x2 = GetXSize(), y2 = GetYSize();
	return (x < dx && y < dy && (x+x2) > dx && (y+y2) > dy);
}

void FrameHook::Draw(void)
{
	if(GetX() != -1 && GetY() != -1)
	{
		Lock();
		uint x = GetX(), y = GetY(), x2 = GetXSize(), y2 = GetYSize();
		RECT rect = {x, y, x+x2, y+y2};
		D2GFX_DrawFrame(&rect);
		Unlock();
	}
}

bool FrameHook::IsInRange(int dx, int dy)
{
	int x = GetX(), y = GetY(), x2 = GetXSize(), y2 = GetYSize();
	return (x < dx && y < dy && (x+x2) > dx && (y+y2) > dy);
}
