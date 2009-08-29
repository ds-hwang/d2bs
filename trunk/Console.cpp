#include "Console.h"
#include "ScriptEngine.h"
#include "Helpers.h"

#include <sstream>
#include <string>

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
	const char* cmd = text->GetText();
	char *argv[2];
	INT argc = StringTokenize(const_cast<char*>(cmd), ' ', argv, 2);

	commands.push_back(std::string(cmd));
	commandPos = commands.size();

	if(!_strcmpi(argv[0], "start"))
	{
		char file[_MAX_PATH+_MAX_FNAME];
		sprintf(file, "%s\\default.dbj", Vars.szScriptPath);
		Script* script = ScriptEngine::CompileFile(file, InGame);
		if(script)
		{
			AddLine("ÿc2D2BSÿc0 :: Starting default.dbj");
			CreateThread(0, 0, ScriptThread, script, 0, 0);
		}
		else
			AddLine("ÿc2D2BSÿc0 :: Failed to start default.dbj!");
	}
	else if(!_strcmpi(argv[0], "stop"))
	{
		if(ScriptEngine::GetCount() > 0)
			AddLine("ÿc2D2BSÿc0 :: Stopping all scripts!");

		ScriptEngine::StopAll(true);
	}
	else if(!_strcmpi(argv[0], "flush"))
	{
		if(!Vars.bDisableCache)
		{
			AddLine("ÿc2D2BSÿc0 :: Flushing the script cache...");
			ScriptEngine::FlushCache();
		}
	}
	else if(!_strcmpi(argv[0], "load"))
	{
		if(argc >= 2)
		{
			char msg[256];
			sprintf(msg, "ÿc2D2BSÿc0 :: Loading %s", argv[1]);
			AddLine(msg);

			CHAR szPath[8192] = "";
			sprintf(szPath, "%s\\%s", Vars.szScriptPath, argv[1]);

			Script* script = ScriptEngine::CompileFile(szPath, InGame, true);
			if(script)
				CreateThread(0, 0, ScriptThread, script, 0, 0);
			else
			{
				sprintf(msg, "ÿc2D2BSÿc0 :: Failed to load %s!", argv[1]);
				AddLine(msg);
			}
		}
	}
	else if(!_strcmpi(argv[0], "reload"))
	{
		if(ScriptEngine::GetCount() > 0)
			AddLine("ÿc2D2BSÿc0 :: Stopping all scripts...");
		ScriptEngine::StopAll(true);

		if(!Vars.bDisableCache)
		{
			AddLine("ÿc2D2BSÿc0 :: Flushing the script cache...");
			ScriptEngine::FlushCache();
		}

		AddLine("ÿc2D2BSÿc0 :: Starting default.dbj...");
		char file[_MAX_PATH+_MAX_FNAME];
		sprintf(file, "%s\\default.dbj", Vars.szScriptPath);
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
	EnterCriticalSection(&lock);
	char* newcmd = _strdup(text->GetText());
	if(strlen(newcmd) != 0)
	{
		newcmd[strlen(newcmd)-1] = '\0';
		text->SetText(newcmd);
		delete newcmd;
	}
	LeaveCriticalSection(&lock);
}

void Console::PrevCommand(void)
{
	if(commandPos < 1)
		return;

	commandPos--;
	text->SetText(commands[commandPos].c_str());
}

void Console::NextCommand(void)
{
	if(commandPos >= commands.size())
		return;

	commandPos++;
	text->SetText(commands[commandPos].c_str());
}

void Console::AddLine(std::string line)
{
	EnterCriticalSection(&lock);

	// add the new line to the list
	lines.push_back(line);

	// update all of the screenhooks to use the new lines
	std::deque<std::string>::reverse_iterator it = lines.rbegin();
	for(int i = lineCount-1; i >= 0 && it != lines.rend(); i--, it++)
		lineBuffers[i]->SetText(it->c_str());

	// clear out old lines
	while(lines.size() > lineCount)
		lines.pop_front();

	LeaveCriticalSection(&lock);
}

void Console::Clear(void)
{
	lines.clear();
}

void Console::Toggle(void)
{
	ToggleBuffer();
	TogglePrompt();
}

void Console::TogglePrompt(void)
{
	if(!IsEnabled())
		ShowPrompt();
	else
		HidePrompt();
}

void Console::ToggleBuffer(void)
{
	if(!IsVisible())
		ShowBuffer();
	else
		HideBuffer();
}

void Console::Hide(void)
{
	EnterCriticalSection(&lock);
	HidePrompt();
	HideBuffer();
	LeaveCriticalSection(&lock);
}

void Console::HidePrompt(void)
{
	EnterCriticalSection(&lock);
	enabled = false;
	prompt->SetIsVisible(false);
	cursor->SetIsVisible(false);
	LeaveCriticalSection(&lock);
}

void Console::HideBuffer(void)
{
	EnterCriticalSection(&lock);
	visible = false;
	if(IsEnabled())
		HidePrompt();
	box->SetIsVisible(false);
	text->SetIsVisible(false);
	for(unsigned int i = 0; i < lineCount; i++)
		lineBuffers[i]->SetIsVisible(false);

	Vars.image->SetY(10);
	Vars.text->SetY(15);
	LeaveCriticalSection(&lock);
}

void Console::Show(void)
{
	EnterCriticalSection(&lock);
	ShowBuffer();
	ShowPrompt();
	LeaveCriticalSection(&lock);
}

void Console::ShowPrompt(void)
{
	EnterCriticalSection(&lock);
	enabled = true;
	if(!IsVisible())
		ShowBuffer();
	prompt->SetIsVisible(true);
	cursor->SetIsVisible(true);
	LeaveCriticalSection(&lock);
}

void Console::ShowBuffer(void)
{
	EnterCriticalSection(&lock);
	visible = true;
	box->SetIsVisible(true);
	text->SetIsVisible(true);
	for(unsigned int i = 0; i < lineCount; i++)
		lineBuffers[i]->SetIsVisible(true);

	Vars.image->SetY(box->GetYSize()+9);
	Vars.text->SetY(box->GetYSize()+14);
	LeaveCriticalSection(&lock);
}

void Console::Draw(void)
{
	// update the hooks to their necessary values
	static DWORD count = 0;
	if(!IsReady())
		Initialize();

	if(IsVisible())
	{
		if(count % 15 == 0 && IsEnabled())
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
			// TODO: fix up the lines when the screen resizes
			box->SetXSize(width);
		}
	}

	count++;
}