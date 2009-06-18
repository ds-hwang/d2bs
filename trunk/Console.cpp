#include "Console.h"
#include "ScriptEngine.h"

#include <sstream>
#include <string>

bool Console::visible = false;
bool Console::initialized = false;
std::vector<std::string> Console::lines = std::vector<std::string>();
BoxHook* Console::box = NULL;
TextHook* Console::text = NULL;
LineHook* Console::cursor = NULL;
TextHook* Console::lineBuffers[14];

void Console::Initialize(void)
{
	POINT size = GetScreenSize();
	int xsize = size.x;
	int ysize = size.y;
	// the console height is 30% of the screen size
	int height = (int)(((double)ysize)*.3);
	size = CalculateTextLen("#", 0);
	int charsize = size.x;
	int charheight = size.y-10;
	// the cursor should be 4 pixels away from the edge of the prompt
	int cx = charsize+4;

	box = new BoxHook(NULL, 0, 0, xsize, height, 0xdf, 0);
	text = new TextHook(NULL, "# ", 1, height, 0, 0);
	// the cursor's height is basically hand-crafted, if you change
	// the font, make sure to double check the char height
	cursor = new LineHook(NULL, cx, height-charheight, cx, height-2, 0xff);
	for(int i = 0; i < 14; i++)
	{
		lineBuffers[i] = new TextHook(NULL, "", 2+charheight, 2+charheight+(i*charheight), 0, 0);
		lineBuffers[i]->SetIsVisible(false);
	}

	box->SetIsVisible(false);
	text->SetIsVisible(false);
	cursor->SetIsVisible(false);

	initialized = true;
}

void Console::Destroy(void)
{
	delete box;
	delete text;
	delete cursor;
	initialized = false;
}

void Console::AddKey(unsigned int key)
{
	std::stringstream oldText;
	oldText << text->GetText() << (char)key;
	text->SetText(oldText.str().c_str());
}

void Console::ExecuteCommand(void)
{
	std::string command(text->GetText());
	command = command.substr(2);
	Script* script = ScriptEngine::CompileCommand(command.c_str());
	if(script)
		CreateThread(0, 0, ScriptThread, script, 0, 0);
	text->SetText("# ");
}

void Console::RemoveLastKey(void)
{
	if(strlen(text->GetText()) == 2)
		return;

	std::string old(text->GetText());
	text->SetText(old.substr(0, old.length()-1).c_str());
}

void Console::AddLine(std::string line) { lines.push_back(line); }
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
		int newsize = textlen.x + 4;
		cursor->SetX(newsize);
		cursor->SetX2(newsize);
		if(box->GetXSize() != width)
		{
			// screen resolution reset, time to adjust all coordinates as necessary
			box->SetXSize(width);
		}
		std::vector<std::string>::reverse_iterator it = lines.rbegin();
		for(int i = 13; i >= 0 && it != lines.rend(); i--, it++)
			lineBuffers[i]->SetText(it->c_str());
	}

	count++;
}
