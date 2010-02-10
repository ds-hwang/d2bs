#include "D2Ptrs.h"

// TODO: Refactor a lot of the JSGame functions here

void SendGold(int nGold, int nMode)
{
	*p_D2CLIENT_GoldDialogAmount = nGold;
	*p_D2CLIENT_GoldDialogAction = nMode;
	D2CLIENT_PerformGoldDialogAction();
}
