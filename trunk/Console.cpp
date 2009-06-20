#include "Console.h"
#include "ScriptEngine.h"

#include <sstream>
#include <string>

bool Console::visible = false;
bool Console::initialized = false;
std::vector<std::string> Console::lines = std::vector<std::string>();
BoxHook* Console::box = NULL;
TextHook* Console::prompt = NULL;
TextHook* Console::text = NULL;
LineHook* Console::cursor = NULL;
TextHook* Console::lineBuffers[14];
unsigned int Console::lineCount = 14;
CRITICAL_SECTION Console::lock = {0};

void Console::Initialize(void)
{
	if(!initialized)
	{
		InitializeCriticalSection(&lock);
		EnterCriticalSection(&lock);
		POINT size = GetScreenSize();
		int xsize = size.x;
		int ysize = size.y;
		// the console height is 30% of the screen size
		int height = (int)(((double)ysize)*.3);
		size = CalculateTextLen(">", 0);
		int charsize = size.x;
		int charheight = size.y-10;
		int cx = charsize+9;

		box = new BoxHook(NULL, 0, 0, xsize, height, 0xdf, 0);
		prompt = new TextHook(NULL, ">", 1, height, 0, 0);
		text = new TextHook(NULL, "", charsize+5, height, 0, 0);
		// the cursor's height is basically hand-crafted, if you change
		// the font, make sure to double check the char height
		cursor = new LineHook(NULL, cx, height-charheight, cx, height-2, 0xff);
		for(unsigned int i = 0; i < lineCount; i++)
		{
			lineBuffers[i] = new TextHook(NULL, "", 2+charheight, 2+charheight+(i*charheight), 0, 0);
			lineBuffers[i]->SetIsVisible(false);
		}

		box->SetIsVisible(false);
		prompt->SetIsVisible(false);
		text->SetIsVisible(false);
		cursor->SetIsVisible(false);

		initialized = true;
		LeaveCriticalSection(&lock);
	}
}

void Console::Destroy(void)
{
	EnterCriticalSection(&lock);
	delete box;
	delete prompt;
	delete text;
	delete cursor;
	initialized = false;
	LeaveCriticalSection(&lock);
	DeleteCriticalSection(&lock);
}

void Console::AddKey(unsigned int key)
{
	const char* cmd = text->GetText();
	int len = strlen(cmd);
	char* newcmd = new char[len+2];
	sprintf(newcmd, "%s%c", cmd, (char)key);
	text->SetText(newcmd);
	delete[] newcmd;
}

void Console::ExecuteCommand(void)
{
	Script* script = ScriptEngine::CompileCommand(text->GetText());
	if(script)
		CreateThread(0, 0, ScriptThread, script, 0, 0);
	text->SetText("");
}

void Console::RemoveLastKey(void)
{
	EnterCriticalSection(&lock);
	char* newcmd = _strdup(text->GetText());
	newcmd[strlen(newcmd)] = '\0';
	text->SetText(newcmd);
	delete newcmd;
	LeaveCriticalSection(&lock);
}

void Console::AddLine(std::string line)
{
	EnterCriticalSection(&lock);
	lines.push_back(line);
	std::vector<std::string>::reverse_iterator it = lines.rbegin();
	for(int i = lineCount-1; i >= 0 && it != lines.rend(); i--, it++)
		lineBuffers[i]->SetText(it->c_str());
	LeaveCriticalSection(&lock);
}

void Console::Clear(void) { lines.clear(); }

void Console::Draw(void)
{
	// update the hooks to their necessary values
	static DWORD count = 0;

	if(IsVisible())
	{
		if(count % 30 == 0)
			cursor->SetIsVisible(!cursor->GetIsVisible());

		// TODO: make this respect wrapping
		char* t = const_cast<char*>(text->GetText());
		int width = GetScreenSize().x;
		POINT textlen = CalculateTextLen(t, 0);
		int newsize = textlen.x + text->GetX();
		cursor->SetX(newsize);
		cursor->SetX2(newsize);
		if(box->GetXSize() != width)
		{
			// screen resolution reset, time to adjust all coordinates as necessary
			box->SetXSize(width);
		}
	}

	// clear old lines as necessary
	// TODO: This is slow, replace with a better solution later
	// TEMPORARY HACK: clear the old lines out every 2 minutes
	if(lines.size() > lineCount && count % 3600)
	{
		std::vector<std::string> tmp;
		for(unsigned int i = 0; i < lineCount; i++)
			tmp.push_back(lines[i]);
		lines.clear();
		for(unsigned int i = 0; i < lineCount; i++)
			lines.push_back(tmp[i]);
	}

	count++;
}
