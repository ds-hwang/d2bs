#pragma once

#include "js32.h"
#include "Script.h"
#include "D2Helpers.h"
#include <list>
#include <windows.h>

typedef unsigned short ushort;

class Genhook;

typedef std::list<Genhook*> HookList;
typedef HookList::iterator HookIterator;

enum Align { Left, Right, Center };
enum ScreenhookState { OOG, IG, Perm };

class Genhook
{
private:
	static HookList hooks;

protected:
	Script* owner;
	ScreenhookState gameState;
	Align alignment;
	jsval clicked, hovered;
	bool isAutomap, isVisible, isLocked;
	ushort opacity;
	POINT location;
	LPCRITICAL_SECTION hookSection;

	Genhook(const Genhook&);
	Genhook& operator=(const Genhook&);

public:
	Genhook::Genhook(Script* nowner, uint x, uint y, ushort nopacity, bool nisAutomap = false, Align nalign = Left,
		ScreenhookState ngameState = Perm);
	~Genhook(void);

	static void DrawAll(ScreenhookState type);
	static HookList GetHooks(void);
	static HookIterator GetFirstHook(void);
	static HookIterator GetLastHook(void);
	static void Clean(Script* owner);

	virtual void Draw(void) = 0;
	bool Click(int button, POINT* loc);
	void Hover(POINT* loc);

	bool IsInRange(POINT* pt) { return IsInRange(pt->x, pt->y); }
	virtual bool IsInRange(int dx, int dy) = 0;

	void Move(POINT* dist) { Move(dist->x, dist->y); }
	void Move(uint nx, uint ny) { SetX(GetX()+nx); SetY(GetY()+ny); }

	void SetX(uint x) { Lock(); location.x = x; Unlock(); }
	void SetY(uint y) { Lock(); location.y = y; Unlock(); }
	void SetAlign(Align nalign) { Lock(); alignment = nalign; Unlock(); }
	void SetOpacity(ushort nopacity) { Lock(); opacity = nopacity; Unlock(); }
	void SetIsVisible(bool nisVisible) { Lock(); isVisible = nisVisible; Unlock(); }
	void SetClickHandler(jsval handler);
	void SetHoverHandler(jsval handler);

	POINT GetLocation(void) const { return location; }
	uint GetX(void) const { return location.x; }
	uint GetY(void) const { return location.y; }
	Align GetAlign(void) const { return alignment; }
	ushort GetOpacity(void) const { return opacity; }
	ScreenhookState GetGameState(void) const { return gameState; }
	bool GetIsAutomap(void) const { return isAutomap; }
	bool GetIsVisible(void) const { return isVisible; }
	jsval GetClickHandler(void) { return clicked; }
	jsval GetHoverHandler(void) { return hovered; }

	void Lock() { EnterCriticalSection(hookSection); isLocked = true; }
	void Unlock() { if(!IsLocked()) return; LeaveCriticalSection(hookSection); isLocked = false; }
	bool IsLocked() { return isLocked; }
};

class TextHook : public Genhook
{
private:
	char* text;
	ushort font, color;

	TextHook(const TextHook&);
	TextHook& operator=(const TextHook&);
public:
	TextHook(Script* owner, char* text, uint x, uint y, bool automap,
			ushort nfont, ushort ncolor, ushort opacity, Align align,
			ScreenhookState state) :
			Genhook(owner, x, y, opacity, automap, align, state), font(nfont), color(ncolor)
	{ this->text = _strdup(text); }
	~TextHook(void) { delete[] text; }

	void Draw(void);
	bool IsInRange(int dx, int dy);

	void SetFont(ushort nfont) { Lock(); font = nfont; Unlock(); }
	void SetColor(ushort ncolor) { Lock(); color = ncolor; Unlock(); }
	void SetText(const char* ntext);

	ushort GetFont(void) const { return font; }
	ushort GetColor(void) const { return color; }
	const char* GetText(void) const { return text; }
};

class ImageHook : public Genhook
{
private:
	char* location;
	CellFile* image;
	ushort color;

	ImageHook(const ImageHook&);
	ImageHook& operator=(const ImageHook&);
public:
	ImageHook(Script* owner, const char* nloc, uint x, uint y, bool automap,
		ushort ncolor, ushort opacity, Align align, ScreenhookState state) :
		Genhook(owner, x, y, opacity, automap, align, state), color(ncolor)
	{ location = _strdup(nloc); image = LoadCellFile(location); }
	~ImageHook(void) { delete[] location; }
	void Draw(void);
	bool IsInRange(int dx, int dy);

	void SetImage(const char* nimage) { Lock(); delete[] location; location = _strdup(nimage); image = LoadCellFile(location); Unlock(); }
	void SetColor(ushort ncolor) { Lock(); color = ncolor; Unlock(); }

	const char* GetImage(void) const { return location; }
	ushort GetColor(void) const { return color; }
};

class LineHook : public Genhook
{
private:
	uint x2, y2;
	ushort color;

	LineHook(const LineHook&);
	LineHook& operator=(const LineHook&);
public:
	LineHook(Script* owner, uint x, uint y, uint nx2, uint ny2, bool automap,
		ushort ncolor, ushort opacity, Align align, ScreenhookState state) :
		Genhook(owner, x, y, opacity, automap, align, state), x2(nx2), y2(ny2), color(ncolor) {}
	~LineHook(void) {}
	void Draw(void);
	bool IsInRange(int dx, int dy) { return false; }

	void SetX2(uint nx2) { Lock(); x2 = nx2; Unlock(); }
	void SetY2(uint ny2) { Lock(); y2 = ny2; Unlock(); }
	void SetColor(ushort ncolor) { Lock(); color = ncolor; Unlock(); }

	uint GetX2(void) const { return x2; }
	uint GetY2(void) const { return y2; }
	ushort GetColor(void) const { return color; }
};

class BoxHook : public Genhook
{
private:
	uint x2, y2;
	ushort color;

	BoxHook(const BoxHook&);
	BoxHook& operator=(const BoxHook&);
public:
	BoxHook(Script* owner, uint x, uint y, uint nx2, uint ny2, bool automap,
		ushort ncolor, ushort opacity, Align align, ScreenhookState state) :
		Genhook(owner, x, y, opacity, automap, align, state), x2(nx2), y2(ny2), color(ncolor) {}
	~BoxHook(void) {}
	void Draw(void);
	bool IsInRange(int dx, int dy);

	void SetX2(uint nx2) { Lock(); x2 = nx2; Unlock(); }
	void SetY2(uint ny2) { Lock(); y2 = ny2; Unlock(); }
	void SetColor(ushort ncolor) { Lock(); color = ncolor; Unlock(); }

	uint GetX2(void) const { return x2; }
	uint GetY2(void) const { return y2; }
	ushort GetColor(void) const { return color; }
};

class FrameHook : public Genhook
{
private:
	uint x2, y2;

	FrameHook(const FrameHook&);
	FrameHook& operator=(const FrameHook&);
public:
	FrameHook(Script* owner, uint x, uint y, uint nx2, uint ny2, bool automap,
		ushort opacity, Align align, ScreenhookState state) :
		Genhook(owner, x, y, opacity, automap, align, state), x2(nx2), y2(ny2) {}
	~FrameHook(void) {}
	void Draw(void);
	bool IsInRange(int dx, int dy);

	void SetX2(uint nx2) { Lock(); x2 = nx2; Unlock(); }
	void SetY2(uint ny2) { Lock(); y2 = ny2; Unlock(); }

	uint GetX2(void) const { return x2; }
	uint GetY2(void) const { return y2; }
};
