#include "ScreenHook.h"
#include "D2BS.h"
#include "D2Ptrs.h"
#include "D2Helpers.h"
#include "Console.h"

#include <vector>

using namespace std;


void DrawLogo(void)
{
	static char img[_MAX_PATH+_MAX_FNAME] = "";
	static char version[] = "D2BS " D2BS_VERSION;
	static int x = (CalculateTextLen(version, 0).x/2);

	if(img[0] == '\0')
		sprintf_s(img, sizeof(img), "%sversion.bmp", Vars.szPath);

	CellFile* vimg = LoadCellFile(img, FALSE);
	int dx = (GetScreenSize().x/2);
	if(!Console::IsVisible())
	{
		DrawAutomapCell(vimg, dx, 10, 0);
		DrawText(version, dx-x, 13, 4, 0);
	} else {
		DrawAutomapCell(vimg, dx, Console::GetHeight()+9, 0);
		DrawText(version, dx-x, Console::GetHeight()+14, 4, 0);
	}
}
