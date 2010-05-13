#include <sstream>
#include <string>

#include "Console.h"
#include "ScriptEngine.h"
#include "D2Ptrs.h"
#include "Core.h"
#include "D2Helpers.h"
#include "Helpers.h"

bool Console::visible = false;
bool Console::enabled = false;
std::deque<std::string> Console::lines = std::deque<std::string>();
std::deque<std::string> Console::commands = std::deque<std::string>();
std::stringstream Console::cmd = std::stringstream();
unsigned int Console::lineCount = 14;
unsigned int Console::commandPos = 0;
unsigned int Console::height = 0;

void Console::AddKey(unsigned int key)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	cmd << (char)key;
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ExecuteCommand(void)
{
	if(cmd.str().length() < 1)
		return;

	commands.push_back(cmd.str());
	commandPos = commands.size();

	ProcessCommand(cmd.str().c_str(), true);

	cmd.str("");
}

void Console::RemoveLastKey(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	int len = cmd.str().length()-1;
	if(len >= 0)
	{
		cmd.str(cmd.str().substr(0, len));
		if(len > 0)
		{
			cmd.seekg(len);
			cmd.seekp(len);
		}
	}
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::PrevCommand(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);

	if(commandPos < 1 || commandPos > commands.size())
	{
		cmd.str("");
	}
	else
	{
		if(commandPos >= 1)
			commandPos--;
		cmd.str("");
		cmd << commands[commandPos];
	}

	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::NextCommand(void)
{
	if(commandPos >= commands.size())
		return;

	EnterCriticalSection(&Vars.cConsoleSection);

	cmd.str("");
	cmd << commands[commandPos];

	if (commandPos < commands.size() - 1)
		commandPos++;

	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::AddLine(std::string line)
{
	EnterCriticalSection(&Vars.cConsoleSection);

	// add the new line to the list
	lines.push_back(line);

	// clear out old lines
	while(lines.size() > lineCount)
		lines.pop_front();

	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Clear(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	lines.clear();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Toggle(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	ToggleBuffer();
	TogglePrompt();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::TogglePrompt(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	if(!IsEnabled())
		ShowPrompt();
	else
		HidePrompt();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ToggleBuffer(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	if(!IsVisible())
		ShowBuffer();
	else
		HideBuffer();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Hide(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	HidePrompt();
	HideBuffer();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::HidePrompt(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	enabled = false;
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::HideBuffer(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	visible = false;
	if(IsEnabled())
		HidePrompt();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Show(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	ShowBuffer();
	ShowPrompt();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ShowPrompt(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	enabled = true;
	if(!IsVisible())
		ShowBuffer();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ShowBuffer(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	visible = true;
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Draw(void)
{
	static DWORD count = 0;
	EnterCriticalSection(&Vars.cConsoleSection);
	if(IsVisible())
	{
		int font = 0;
		POINT size = GetScreenSize();
		int xsize = size.x;
		size = CalculateTextLen(">", font);
		// charsize is wrong like this, but it's even more wrong with how it was being determined
		// we need to see if d2 has a monospaced font, and if not we need to do some trickery to
		// get it to recognize the real line length per-line
		int charsize = 7; // size.x;
		int charheight = size.y-10;
		// the default console height is enough for lineCount lines
		int height = (charheight+1)*lineCount;
		int cx = charsize+9;
		int linelen = (xsize / charsize) - 1;
		Console::height = height;

		std::string cmdstr = cmd.str();
		int cmdsize = CalculateTextLen(cmdstr.c_str(), 0).x;
		cmdsize = (cmdsize+charsize*2)/xsize;
		if(cmdsize > 0)
			Console::height += cmdsize * charheight + 1;

		// draw the box large enough to hold the whole thing
		D2GFX_DrawRectangle(0, 0, xsize, Console::height, 0xdf, 0);

		std::deque<std::string>::reverse_iterator it = lines.rbegin();
		for(int i = lineCount-1; i >= 0 && it != lines.rend(); i--, it++)
		{
			if(CalculateTextLen(it->c_str(), 0).x > (xsize - (2+charheight)*2))
			{
				std::list<std::string> buf;
				SplitLines(*it, linelen, ' ', buf);
				for(std::list<std::string>::iterator it = buf.begin(); it != buf.end(); it++)
					DrawText(it->c_str(), 2+charheight, 2+charheight+((i--)*charheight), 0, font);
			} else
				DrawText(it->c_str(), 2+charheight, 2+charheight+(i*charheight), 0, font);
		}

		if(IsEnabled())
		{
			DrawText(">", 1, height, 0, font);
			int lx = cx + cmdsize - charsize + 5,
				ly = height-charheight;
			if(count % 30)
				D2GFX_DrawLine(lx, ly, lx, height-2, 0xFF, 0xFF);

			if(cmdstr.length() > 0)
			{
				if(((cmdsize+charsize*2)/xsize) > 0)
				{
					std::list<std::string> lines;
					SplitLines(cmdstr, linelen, ' ', lines);
					int i = 0;
					for(std::list<std::string>::iterator it = lines.begin(); it != lines.end(); it++, i++)
						DrawText(it->c_str(), charsize+5, height+(charheight*i)+1, 0, font);
				} else
					DrawText(cmdstr.c_str(), charsize+5, height+1, 0, font);
			}
		}
	}
	LeaveCriticalSection(&Vars.cConsoleSection);

	count++;
}

unsigned int Console::GetHeight(void)
{
	return height;
}
