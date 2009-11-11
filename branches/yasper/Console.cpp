#include <sstream>
#include <string>

#include "Console.h"
#include "ScriptEngine.h"
#include "Helpers.h"

bool Console::visible = false;
bool Console::enabled = false;
bool Console::initialized = false;
std::deque<std::string> Console::lines = std::deque<std::string>();
std::deque<std::string> Console::commands = std::deque<std::string>();
BoxHook* Console::box = NULL;
TextHook* Console::prompt = NULL;
TextHook* Console::text = NULL;
LineHook* Console::cursor = NULL;
TextHook* Console::lineBuffers[14];
unsigned int Console::lineCount = 14;
unsigned int Console::commandPos = 0;

void Console::Initialize(void)
{
	if(!initialized)
	{
		EnterCriticalSection(&Vars.cConsoleSection);
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
		LeaveCriticalSection(&Vars.cConsoleSection);
	}
}

void Console::Destroy(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	delete box;
	box = NULL;
	delete prompt;
	prompt = NULL;
	delete text;
	text = NULL;
	delete cursor;
	cursor = NULL;
	for(unsigned int i = 0; i < lineCount; i++)
	{
		delete lineBuffers[i];
		lineBuffers[i] = NULL;
	}
	initialized = false;
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::AddKey(unsigned int key)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	const char* cmd = text->GetText();
	int newlen = strlen(cmd)+2;
	char* newcmd = new char[newlen];
	sprintf_s(newcmd, newlen, "%s%c", cmd, (char)key);
	text->SetText(newcmd);
	delete[] newcmd;
	newcmd = NULL;
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ExecuteCommand(void)
{
	const char* cmd = text->GetText();
	if(strlen(cmd) < 1)
		return;

	char* buf = _strdup(cmd);
	char* next_token1;
	char* argv = strtok_s(buf, " ", &next_token1);

	commands.push_back(std::string(cmd));
	commandPos = commands.size();

	if(argv == NULL)
	{
		text->SetText("");
		return;
	}

	if(!_strcmpi(argv, "start"))
	{
		char file[_MAX_PATH+_MAX_FNAME];
		sprintf_s(file, sizeof(file), "%s\\default.dbj", Vars.szScriptPath);
		Script* script = ScriptEngine::CompileFile(file, InGame);
		if(script)
		{
			AddLine("ÿc2D2BSÿc0 :: Starting default.dbj");
			CreateThread(0, 0, ScriptThread, script, 0, 0);
		}
		else
			AddLine("ÿc2D2BSÿc0 :: Failed to start default.dbj!");
	}
	else if(!_strcmpi(argv, "stop"))
	{
		if(ScriptEngine::GetCount() > 0)
			AddLine("ÿc2D2BSÿc0 :: Stopping all scripts!");

		ScriptEngine::StopAll(true);
	}
	else if(!_strcmpi(argv, "flush"))
	{
		if(!Vars.bDisableCache)
		{
			AddLine("ÿc2D2BSÿc0 :: Flushing the script cache...");
			ScriptEngine::FlushCache();
		}
	}
	else if(!_strcmpi(argv, "load"))
	{
		const char* arg = cmd+5;
		if(strlen(arg) > 0)
		{
			char msg[256];
			sprintf_s(msg, sizeof(msg), "ÿc2D2BSÿc0 :: Loading %s", arg);
			AddLine(msg);

			char Path[_MAX_PATH+_MAX_FNAME] = "";
			sprintf_s(Path, sizeof(Path), "%s\\%s", Vars.szScriptPath, arg);

			Script* script = ScriptEngine::CompileFile(Path, InGame, true);
			if(script)
				CreateThread(0, 0, ScriptThread, script, 0, 0);
			else
			{
				sprintf_s(msg, sizeof(msg), "ÿc2D2BSÿc0 :: Failed to load %s!", arg);
				AddLine(msg);
			}
		}
	}
	else if(!_strcmpi(argv, "reload"))
	{
		if(ScriptEngine::GetCount() > 0)
		{
			AddLine("ÿc2D2BSÿc0 :: Stopping all scripts...");
			ScriptEngine::StopAll(true);
		}

		if(!Vars.bDisableCache)
		{
			AddLine("ÿc2D2BSÿc0 :: Flushing the script cache...");
			ScriptEngine::FlushCache();
		}
		Sleep(1000); // wait for things to catch up

		AddLine("ÿc2D2BSÿc0 :: Starting default.dbj...");
		char file[_MAX_PATH+_MAX_FNAME];
		sprintf_s(file, sizeof(file), "%s\\default.dbj", Vars.szScriptPath);
		Script* script = ScriptEngine::CompileFile(file, InGame);
		if(script)
			CreateThread(0, 0, ScriptThread, script, 0, 0);
		else
			AddLine("ÿc2D2BSÿc0 :: Failed to start default.dbj!");
	}
	else
	{
		Script* script = ScriptEngine::CompileCommand(cmd);
		if(script)
			CreateThread(0, 0, ScriptThread, script, 0, 0);
	}
	text->SetText("");
}

void Console::RemoveLastKey(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	char* newcmd = _strdup(text->GetText());
	if(strlen(newcmd) != 0)
	{
		newcmd[strlen(newcmd)-1] = '\0';
		text->SetText(newcmd);
		delete[] newcmd;
		newcmd = NULL;
	}
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::PrevCommand(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);

	if(commandPos < 1 || commandPos > commands.size())
	{
		text->SetText("");
	}
	else
	{
		if(commandPos >= 1)
			commandPos--;
		text->SetText(commands[commandPos].c_str());
	}

	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::NextCommand(void)
{
	if(commandPos >= commands.size())
		return;

	EnterCriticalSection(&Vars.cConsoleSection);

	text->SetText(commands[commandPos].c_str());

	if (commandPos < commands.size() - 1)
		commandPos++;

	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::AddLine(std::string line)
{
	if(!IsReady())
		Initialize();

	EnterCriticalSection(&Vars.cConsoleSection);

	// add the new line to the list
	lines.push_back(line);

	// update all of the screenhooks to use the new lines
	std::deque<std::string>::reverse_iterator it = lines.rbegin();
	for(int i = lineCount-1; i >= 0 && it != lines.rend(); i--, it++)
		lineBuffers[i]->SetText(it->c_str());

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
	prompt->SetIsVisible(false);
	cursor->SetIsVisible(false);
	text->SetIsVisible(false);
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::HideBuffer(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	visible = false;
	if(IsEnabled())
		HidePrompt();
	box->SetIsVisible(false);
	for(unsigned int i = 0; i < lineCount; i++)
		lineBuffers[i]->SetIsVisible(false);

	Vars.image->SetY(10);
	Vars.text->SetY(15);
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
	prompt->SetIsVisible(true);
	cursor->SetIsVisible(true);
	text->SetIsVisible(true);
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ShowBuffer(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);

	visible = true;

	if(!box)
		DebugBreak();
	box->SetIsVisible(true);

	for(unsigned int i = 0; i < lineCount; i++)
		lineBuffers[i]->SetIsVisible(true);

	Vars.image->SetY(box->GetYSize()+9);
	Vars.text->SetY(box->GetYSize()+14);
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Draw(void)
{
	// update the hooks to their necessary values
	static DWORD count = 0;
	if(!IsReady())
		Initialize();

	EnterCriticalSection(&Vars.cConsoleSection);
	if(IsVisible())
	{
		if(count % 15 == 0 && IsEnabled())
			cursor->SetIsVisible(!cursor->GetIsVisible());

		// TODO: make this respect wrapping
		char* t = const_cast<char*>(text->GetText());
		uint width = GetScreenSize().x;
		POINT textlen = CalculateTextLen(t, 0);
		int newsize = textlen.x + text->GetX();
		cursor->SetX(newsize);
		cursor->SetX2(newsize);
		if(box->GetXSize() != width)
		{
			// screen resolution reset, time to adjust all coordinates as necessary
			// TODO: fix up the lines when the screen resizes
			box->SetXSize(width);
		}
	}
	LeaveCriticalSection(&Vars.cConsoleSection);

	count++;
}
