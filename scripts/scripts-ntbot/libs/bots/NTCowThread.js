/****************************************************************/
/*		NT_CowThread script for D2BS v.1.3.3 written by Shaggi	*/
/****************************************************************/
const NTC_UNIT_MONSTER = 1;
var NTCOW_KingChicken = 80;

addEventListener("scriptmsg",msgHandler)

function main() {

	var CowKing, _cowCurHp;

	COW_Print("CowThread Loaded");

	while(true) {
		while(me.area != 39)
			delay(1000);

		if(!CowKing) CowKing = COW_GetKing();
		else {
			_cowCurHp = parseInt(CowKing.hp / CowKing.hpmax * 100);
			if(_cowCurHp < NTCOW_KingChicken) {
				COW_Print("Chickening at " + _cowCurHp + " - Cow is dying :(!")
				quit();
			}
		}

		delay(500);
	}
	return true;
}

function COW_GetKing() {
	var possiblecow = getUnit(NTC_UNIT_MONSTER,getLocaleString(2850)), _king = false;
	if(possiblecow) {
		do
		{
			if(possiblecow.uniqueid == 39) {
				_king = possiblecow;
				break;
			}
		}while(possiblecow.getNext())
	}
	return _king;
}

function COW_Print(szMsg) {
	if(arguments.length < 1) return false;
	return print("ÿc2NTCowThread ÿc1:: ÿc3" + szMsg);
}

function msgHandler(msg) {
	if(msg.search("Exit") != -1) stop(true);
	var Value = msg.substr(msg.search("=")+1);
	if(Value) NTCOW_KingChicken = parseInt(Value);
}