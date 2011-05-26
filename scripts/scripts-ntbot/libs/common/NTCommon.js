/// <reference path="/../../d2bsAPI.js" /> 


//include() getBaseStat() item table object for NTC_GetBaseStat()
include('objects/itemTable.json');
include('common/NTCommonExt.dbl');

// Public D2BS varants

// Character Class
var NTC_CHAR_CLASS_AMAZON = 0;
var NTC_CHAR_CLASS_SORCERESS = 1;
var NTC_CHAR_CLASS_NECROMANCER = 2;
var NTC_CHAR_CLASS_PALADIN = 3;
var NTC_CHAR_CLASS_BARBARIAN = 4;
var NTC_CHAR_CLASS_DRUID = 5;
var NTC_CHAR_CLASS_ASSASSIN = 6;

// Act 1 NPCs
var NTC_NPC_ID_AKARA = 148;
var NTC_NPC_ID_WARRIV_ACT_1 = 155;
var NTC_NPC_ID_CHARSI = 154;
var NTC_NPC_ID_GHEED = 147;
var NTC_NPC_ID_KASHYA = 150;
var NTC_NPC_ID_CAIN_TRISTRAM = 146;
var NTC_NPC_ID_CAIN_ACT_1 = 244;

// Act 2 NPCs
var NTC_NPC_ID_FARA = 178;
var NTC_NPC_ID_LYSANDER = 202;
var NTC_NPC_ID_GREIZ = 198;
var NTC_NPC_ID_ELZIX = 199;
var NTC_NPC_ID_MESHIF_ACT_2 = 210;
var NTC_NPC_ID_DROGNAN = 177;
var NTC_NPC_ID_ATMA = 176;
var NTC_NPC_ID_WARRIV_ACT_2 = 175; 
var NTC_NPC_ID_CAIN_ACT_2 = 245;
var NTC_NPC_ID_JERHYN = 201;
var NTC_NPC_ID_TYRAEL_ACT_2 = 251;

// Act 3 NPCs
var NTC_NPC_ID_MESHIF_ACT_3 = 264;
var NTC_NPC_ID_CAIN_ACT_3 = 246;
var NTC_NPC_ID_ALKOR = 254;
var NTC_NPC_ID_ASHEARA = 252;
var NTC_NPC_ID_ORMUS = 255;
var NTC_NPC_ID_HRATLI = 253;

// Act 4 NPCs
var NTC_NPC_ID_CAIN_ACT_4 = 265;
var NTC_NPC_ID_HALBU = 257;
var NTC_NPC_ID_TYRAEL_ACT_4 = 367;
var NTC_NPC_ID_JAMELLA = 405;

// Act 5 NPCs
var NTC_NPC_ID_CAIN_ACT_5 = 520;
var NTC_NPC_ID_TYRAEL_ACT_5 = 521;
var NTC_NPC_ID_LARZUK = 511;
var NTC_NPC_ID_ANYA_FROZEN_RIVER = 527;
var NTC_NPC_ID_ANYA_TOWN = 512;
var NTC_NPC_ID_MALAH = 513;
var NTC_NPC_ID_QUALKEHK = 515;
var NTC_NPC_ID_NIHLATHAK_TOWN = 514;
var NTC_NPC_ID_NIHLATHAK_HALLS_OF_VAUGHT = 526;

// UNIT types
var NTC_UNIT_PLAYER = 0;
var NTC_UNIT_NPC = 1;
var NTC_UNIT_MERC = NTC_UNIT_NPC;
var NTC_UNIT_MONSTER = NTC_UNIT_NPC;
var NTC_UNIT_OBJECT = 2;
var NTC_UNIT_MISSILE = 3;
var NTC_UNIT_ITEM = 4;
var NTC_UNIT_TILE = 5;
var NTC_UNIT_STASH = 267;

var NTC_AREA_ID_ACT_1 = 1;
var NTC_AREA_ID_ACT_2 = 40;
var NTC_AREA_ID_ACT_3 = 75;
var NTC_AREA_ID_ACT_4 = 103;
var NTC_AREA_ID_ACT_5 = 109;

var NTC_ActNumberToTownID = new Array(5);
NTC_ActNumberToTownID[0] = 1;
NTC_ActNumberToTownID[1] = 40;
NTC_ActNumberToTownID[2] = 75;
NTC_ActNumberToTownID[3] = 103;
NTC_ActNumberToTownID[4] = 109;

// COLOR types
var NTC_COLOR_BASE_WHITE = 0;
var NTC_COLOR_BASE_RED = 1;
var NTC_COLOR_BASE_GREEN = 2;
var NTC_COLOR_BASE_BLUE = 3;
var NTC_COLOR_BASE_GOLD = 4;
var NTC_COLOR_BASE_GRAY = 5;
var NTC_COLOR_BASE_BLACK = 6;
var NTC_COLOR_BASE_GOLD2 = 7;
var NTC_COLOR_BASE_ORANGE = 8;
var NTC_COLOR_BASE_YELLOW = 9;

var NTC_HAND_RIGHT = 0;
var NTC_HAND_LEFT = 1;

var NTC_CLICK_LDOWN = 0;
var NTC_CLICK_LHOLD = 1;
var NTC_CLICK_LUP = 2;
var NTC_CLICK_RDOWN = 3;
var NTC_CLICK_RHOLD = 4;
var NTC_CLICK_RUP = 5;

var NTC_SHIFT_NONE = 0;
var NTC_SHIFT_HOLD = 1;

var NTC_DELAY_FRAME = 40;

var NTC_CharClassToNameList = new Array(7);
NTC_CharClassToNameList[0] = "Amazon";
NTC_CharClassToNameList[1] = "Sorceress";
NTC_CharClassToNameList[2] = "Necromancer";
NTC_CharClassToNameList[3] = "Paladin";
NTC_CharClassToNameList[4] = "Barbarian";
NTC_CharClassToNameList[5] = "Druid";
NTC_CharClassToNameList[6] = "Assassin";

var NTConfig_AttackSkill =[];
	NTConfig_AttackSkill[0] = 0;		// Cast your first spell once. Set to 0 if you won't
	NTConfig_AttackSkill[1] = 0;		// The primary skill used to attack the boss.
	NTConfig_AttackSkill[2] = 0;		// Cast your primary spell to others.
	NTConfig_AttackSkill[3] = 0;		// Cast your primary untimed spell if primary spell is timed spell. Set to 0 if you won't
	NTConfig_AttackSkill[4] = 0;		// Secondary untimed skill if boss is immune to primary untimed skill!
	NTConfig_AttackSkill[5] = 0;		// Cast your secondary untimed spell if secondary spell is timed spell. Set to 0 if you won't

var NTC_ItemQualityToNameList = new Array(9);
NTC_ItemQualityToNameList[0] = "None";
NTC_ItemQualityToNameList[1] = "Low Quality";
NTC_ItemQualityToNameList[2] = "Normal";
NTC_ItemQualityToNameList[3] = "Superior";
NTC_ItemQualityToNameList[4] = "Magic";
NTC_ItemQualityToNameList[5] = "Set";
NTC_ItemQualityToNameList[6] = "Rare";
NTC_ItemQualityToNameList[7] = "Unique";
NTC_ItemQualityToNameList[8] = "Crafted";

function attackPrint(nString)
{
	if(NTConfig_PrintAttack)
		print(nString);
}

function debugPrint(str)
{
	if(bobDebug) 
		sendCopyData(null, "OOG", 0,str+" " );
}

function NTC_Include(filepath)
{
	if(isIncluded(filepath))
		return true;
	//load("libs/" + filepath);
	return include(filepath);
}

function NTC_IncludeConfig()
{
	var _configname;

	switch(me.classid)
	{
	case NTC_CHAR_CLASS_AMAZON:
		_configname = "NTConfig_Amazon_" + me.charname + ".dbl"
		break;

	case NTC_CHAR_CLASS_SORCERESS:
		_configname = "NTConfig_Sorceress_" + me.charname + ".dbl"
		break;

	case NTC_CHAR_CLASS_NECROMANCER:
		_configname = "NTConfig_Necromancer_" + me.charname + ".dbl"
		break;

	case NTC_CHAR_CLASS_PALADIN:
		_configname = "NTConfig_Paladin_" + me.charname + ".dbl"
		break;

	case NTC_CHAR_CLASS_BARBARIAN:
		_configname = "NTConfig_Barbarian_" + me.charname + ".dbl"
		break;

	case NTC_CHAR_CLASS_DRUID:
		_configname = "NTConfig_Druid_" + me.charname + ".dbl"
		break;

	case NTC_CHAR_CLASS_ASSASSIN:
		_configname = "NTConfig_Assassin_" + me.charname + ".dbl"
		break;
	}

	if (!(FileTools.exists("libs/config/" + _configname))){
	  try{
		print(" trying to make config file me.classid :"+me.classid);
		FileTools.copy("libs/config/NTConfig_" + NTC_CharClassToNameList[me.classid] + ".dbl", "libs/config/NTConfig_" + NTC_CharClassToNameList[me.classid] + "_" + me.name + ".dbl" );
		
		NTCreatedConfig=true;
		
      }
      catch(fileException){
         print(" ");
         print(fileException);
       //  stop();
      }
   }
  NTC_Include("config/" + _configname);
  
   
}

function NTC_IncludeLibs()
{
	NTC_Include("common/NTAreas.js");
	NTC_Include("common/NTAttack.js");
	NTC_Include("common/NTMove.js");
	NTC_Include("common/NTPrecast.js");
	NTC_Include("common/NTSnagIt.js");
	NTC_Include("common/NTTown.js");
	NTC_Include("common/NTTownManager.js");
	NTC_Include("common/NTTownMove.js");
}

function NTC_DoCast(skillid, usehand, x, y, retrys)
{
	if (!retrys)
		retrys = 5;
	debugPrint("ntc DoCast "+skillid);
	var _hand;
	var _shift;

	if(arguments.length < 1)
		return false;

	if(arguments.length == 1)
	{
		_hand = NTC_CLICK_RDOWN;
		_shift = NTC_SHIFT_NONE;
		usehand = NTC_HAND_RIGHT;
	}
	else
	{
		if(usehand == NTC_HAND_RIGHT)
		{
			_hand = NTC_CLICK_RDOWN;
			_shift = NTC_SHIFT_HOLD;
		}
		else if(usehand == NTC_HAND_LEFT)
		{
			_hand = NTC_CLICK_LDOWN;
			_shift = NTC_SHIFT_HOLD;
		}
		else
		{
			_hand = NTC_CLICK_LDOWN;
			_shift = NTC_SHIFT_HOLD;
			usehand = NTC_HAND_LEFT;
		}
	}

	if(NTC_PutSkill(skillid, usehand))
	{
		if(!NTC_ClearCursor())
			return false;

		for(var i = 0 ; i < 5 ; i++)
		{
		
			switch(arguments.length)
			{
				case 1:
				case 2:
					NTC_DoClick(_hand, _shift);
					break;

				case 3:
					if(x.type != NTC_UNIT_MONSTER)
						return false;

					NTC_DoClick(_hand,_shift, x);
					break;

				case 4:
					NTC_DoClick(_hand,_shift, x, y);
					break;
				case 5:
					NTC_DoClick(_hand,_shift, x, y);
					break;
			}
			
				if(NTC_CheckDoCastInt())
					return NTC_WaitCastDone();
			
		}
	}

	return false;
}
function castdelay() {
	var _mode;
	for(var i = 0 ; i < 10 ; i++)
	{
		_mode = me.mode;
		if(_mode == 7 || _mode == 8 || (_mode >= 10 && _mode <= 16) || _mode == 18)
			return true;
		delay(5);
	}

return false;
}

function NTC_PutSkill(skillid, hand)
{
	NTC_WaitCastDone();
	
	var _sethand;

	if(NTC_IsPassiveSkill(skillid))
		return false;

	if(!NTC_CheckSkill(skillid))
		return false;

	if(hand == NTC_HAND_RIGHT)
		_sethand = 2;
	else
		_sethand = 3;

	if(me.getSkill(_sethand) != skillid)
	{
		debugPrint("me.setskill hand:"+_sethand +" skill: "+skillid);
		
		for(var i = 0 ; i < 60 ; i++)
		{
			if((i % 20) == 0)
			{
				debugPrint("me.setskill i:"+i +"mode: "+me.mode);
				me.setSkill(skillid, hand);
				debugPrint("me.setskill complete i:"+i +"mode: "+me.mode);
			}
			
			if(me.getSkill(_sethand) == skillid)
			{
				debugPrint("me.setskill i:"+i +"returning true");
				return true;
			}
		}
		return false;
	}
	return true;
}

function NTC_CheckSkill(skillid)
{
	debugPrint("getskill " );
	var _skill = me.getSkill(skillid, 1);

	if(_skill > 0)
		return _skill;

	return false;
}

function NTC_GetCastDelay(skillid)
{
	switch(skillid)
	{
	// Amazon
	case 15: return 500;  // "Poison Javelin"
	case 25: return 4000; // "Plague Javelin"
	case 27: return 1000; // "Immolation Arrow"

	// Sorceress
	case 51: return 1400; // "Fire Wall"
	case 56: return 1200; // "Meteor"
	case 62: return 2000; // "Hydra"
	case 59: return 1800; // "Blizzard"
	case 64: return 1000; // "Frozen Orb"

	// Paladin
	case 121: return 1000; // "Fist of the Heavens"

	// Druid
	case 225: return 600;  // "Firestorm"
	case 229: return 2000; // "Molten Boulder"
	case 234: return 2000; // "Eruption"
	case 244: return 4000; // "Volcano"
	case 249: return 6000; // "Armageddon"
	case 250: return 6000; // "Hurricane"
	case 223: return 1000; // "Werewolf"
	case 228: return 1000; // "Werebear"
	case 247: return 1000; // "Grizzly Bear"

	// Assassin
	case 275: return 1000; // "Dragon Flight"
	case 277: return 2000; // "Blade Sentinel"
	case 268: return 6000; // "Shadow Warrior"
	case 279: return 6000; // "Shadow Master"
	}

	return 0;
}

function NTC_IsLeftSkill(skillid)
{
	if(NTC_IsPassiveSkill(skillid))
		return false;

	var _leftskilllist = [0,2,4,5,6,7,10,11,12,14,15,16,19,20,21,22,24,25,26,27,30,
			31,34,35,36,38,39,41,45,47,49,53,55,64,67,73,84,93,96,97,
			101,106,107,111,112,116,121,126,132,133,139,140,143,144,
			147,151,152,225,229,230,232,233,238,239,240,242,243,245,
			248,251,254,255,256,257,259,260,265,266,269,270,274,275,280];

	for(var i = 0 ; i < _leftskilllist.length ; i++)
	{
		if(_leftskilllist[i] == skillid)
			return true;
	}

	return false;
}

function NTC_IsPassiveSkill(skillid)
{
	var _passiveskilllist = [9,13,18,23,29,33,37,61,63,65,69,79,89,127,128,
				129,134,135,136,141,145,148,153,224,252,263];

	for(var i = 0 ; i < _passiveskilllist.length ; i++)
	{
		if(_passiveskilllist[i] == skillid)
			return true;
	}

	return false;
}

// spectype : 0x00 = Normal Monster, 0x01 = Super Unique, 0x02 = Champion, 0x04 = Boss, 0x08 = Minion
function NTC_FindMonster(classid, spectype)
{
	var _monster;

	if(arguments.length < 1)
		return false;

	if(arguments.length < 2)
		spectype = 0;
		
	debugPrint("getunit monster" );
	_monster = NTC_GetUnit(NTC_UNIT_MONSTER, classid);

	if(_monster)
	{
		if(spectype)
		{
			do
			{
				if(_monster.type == NTC_UNIT_MONSTER && (_monster.spectype & spectype)){
					debugPrint("Copy Unit in find monster" );
					return copyUnit(_monster);
				}
			} while(_monster.getNext());
		}
		else
			return _monster;
	}

	return false;
}

function NTC_DoWeaponSwitch(switchto)
{

if(arguments.length < 1)
		switchto = (me.weaponswitch == 0) ? 1 : 0;		
	else if(me.weaponswitch == switchto){
		debugPrint("doWeponswitch switchto:"+switchto +"already switched" );	
		return true;
	}
debugPrint("doWeponswitch switchto:"+switchto );	
	if(!NTC_WaitCastDone()){
		debugPrint("doWeponswitch waitcast retrn false " );
		return false;
	}
	for(var i = 0 ; i < 150 ; i++)
	{
		if((i % 25) == 0)
			weaponSwitch(0);

		NTC_Delay(NTC_DELAY_FRAME);

		if(me.weaponswitch == switchto){
			debugPrint("doWeponswitch Success " );
			return true;
		}
	}

	return false;
}

function NTC_DoClick(button, shift, x, y)
{
	debugPrint("ntcDoClick" );
	
	if(button != NTC_CLICK_LDOWN)
		button = NTC_CLICK_RDOWN;

	if(!NTC_WaitCastDone())
		return false;

	switch(arguments.length)
	{
		case 2:
			clickMap(button, shift, me.x, me.y);
			NTC_Delay(50);
			clickMap(button+2, shift, me.x, me.y);

			return true;

		case 3:
			clickMap(button, shift, x);
			NTC_Delay(50);
			clickMap(button+2, shift, x);

			return true;

		case 4:
			clickMap(button, shift, x, y);
			NTC_Delay(50);
			clickMap(button+2, shift, x, y);

			return true;
	}

	return false;
}

function NTC_MyGold()
{
	debugPrint("ntc My gold" );	
	var ret= me.getStat(14) + me.getStat(15);	
	return ret;
}

function NTC_MyGoldCarry()
{
	return me.getStat(14);
}

function NTC_StashGoldFull()
{
	return (NTConfig_GambleStashThreshold <= me.getStat(15));
}

function NTC_CharGoldFull()
{
	return (me.getStat(14) >= (me.getStat(12)*10000));
}

function NTC_CheckOwner(object, ownername)
{
	debugPrint("check owner" );
	

	if(!object)
		return false;

	var _owner = object.getParent();
	if(!_owner)
		return false;

	if(arguments.length < 2)
	{
		if(_owner == me.name)// || _owner.name == me.name)
			return true;
	}
	else
	{
	
		if(_owner == ownername)//|| _owner.name == ownername)
		
			return true;
	}

	return false;
}

function NTC_ItemToCursor(item)
{

debugPrint("ntc item to cursor" );	
if(me.itemoncursor)
		return true;
for (var b = 1 ; b < 37 ; b++){
	if(getUIFlag(b)){
	debugPrint("ntc item to cursor ui:"+b );	
	}
}
	var _itemloc = item.location;	
	var _itemy = item.y;
	var _itemx = item.x;
 	var _click = 0;
	if (_itemloc==255) _itemloc=5;
	for(var i = 0 ; i < 30 ; i++)
	{
		if((i % 10) == 0){
	
			//if (getUIFlag(12) ){
			//	_click = 5;
			debugPrint("ntc item click:"+_click +" item "+item.name +"loc:"+_itemloc+"X:"+_itemx+"Y:"+_itemy);
			clickItem(_click,_itemx,_itemy,_itemloc);
			//}else{
				//clickItem(0, item);
			//}
			debugPrint("ntc item click returned safely");
			NTC_PingDelay(50);
		}
		debugPrint("me.itemoncursor");
		if(me.itemoncursor)
		{
		debugPrint("me.itemoncursor retrned ");
			NTC_PingDelay(200);

			return true;
		}
	}

	return false;
}

function NTC_ClearCursor()
{
	if(!me.itemoncursor)
		return true;

	for(var i = 0 ; i < 60 ; i++)
	{
		if((i % 20) == 0)
			NTC_DoClick(NTC_CLICK_LDOWN, NTC_SHIFT_NONE);

		NTC_Delay(NTC_DELAY_FRAME);

		if(!me.itemoncursor)
			return true;
	}

	return false;
}

function NTC_GetMerc()
{
	debugPrint("ntc get merc" );
	var _merc;

	for(var i = 0 ; i < 2 ; i++)
	{
		_merc = me.getMerc();

		if(_merc)
		{
			if(_merc.hp > 0 && _merc.mode != 12)
				return _merc;

			break;
		}

		NTC_Delay(NTC_DELAY_FRAME);
	}

	return false;
}

function NTC_OpenChest(chest)
{
	if(!chest)
		return false;

	if(chest.mode > 0)
		return false;

	for(var i = 0 ; i < 20 ; i++)
	{
		if((i % 5) == 0)
			chest.interact();

		NTC_Delay(100);

		if(chest.mode > 0)
			return true;
	}

	return false;
}

function NTC_Delay(amount, random)
{
	while (NT_Pause)
	{
		
	 	delay (500);
	}
	
	//debugPrint("ntc_delay:"+amount );
	
	if(arguments.length < 2)
		delay(amount+1);
	else
		delay(amount+rnd(1, random));
}

function NTC_PingDelay(amount)
{
	while (NT_Pause)
	{
	 //debugPrint("NT_Pause...");
		delay (500);
	}

	//debugPrint("me.ping delay" );
	
	var _currentping = me.ping; 

	if(_currentping > 100)
		_currentping -= 99;
	else
		_currentping = 0;

	NTC_Delay(amount+_currentping);
}

function NTC_InTown(who)
{
	
	var _area;

	if(arguments.length < 1)
		who = me;
debugPrint("ntc intown who:"+who.name+" my area:"+who.area +"arg:"+arguments.length);
	while(who.area == 0)
		NTC_Delay(NTC_DELAY_FRAME);

	_area = who.area;

	return (_area == NTC_AREA_ID_ACT_1 || _area == NTC_AREA_ID_ACT_2 || _area == NTC_AREA_ID_ACT_3 || _area == NTC_AREA_ID_ACT_4 || _area == NTC_AREA_ID_ACT_5);
}

function NTC_InMyParty(playerName)
{
	debugPrint("ntc inmyparty" );
	var _player;
	var _myPartyId;

	for(var i = 0 ; i < 2 ; i++)
	{
		_player = getParty();

		if(_player)
		{
			_myPartyId = _player.partyid;

			if(_myPartyId != 65535)
			{
				do
				{
					if(_player.name == playerName && _myPartyId == _player.partyid)
						return true;
				} while(_player.getNext());
			}

			break;
		}

		NTC_Delay(NTC_DELAY_FRAME);
	}

	return false;
}

function NTC_GetUnit(unittype, classid, retries)
{
	if(arguments.length < 3)
		retries = 3;
		
	debugPrint('called NTC_GetUnit('+ unittype +', '+ classid +', '+ retries +')');
	
	var _unit;

	for(var i = 0 ; i < retries ; i++)
	{
		if(arguments.length < 2)
		{
			debugPrint("getUnit("+unittype+")");
			_unit = getUnit(unittype);
		}
		else
		{
			debugPrint("getUnit("+unittype+","+classid+")");
			_unit = getUnit(unittype, classid);
		}
		
		if(_unit && _unit.type == unittype)
		{
			
			return _unit;
		}
	}
	
	debugPrint("Unit Not found");
	
	return false;
}

function NTC_GetPresetUnits(unittype, classid, area)
{
debugPrint("ntc getpresetunits" );
	var _unit;

	if(arguments.length < 3)
		area = me.area;

	for(var i = 0 ; i < 3 ; i++)
	{
		if(arguments.length < 2)
			_unit = getPresetUnits(area, unittype);
		else
			_unit = getPresetUnits(area, unittype, classid);
		debugPrint("ntc getpresetunits return" );
		if(_unit && _unit.length > 0 && _unit[0].type == unittype)
			return _unit;

		NTC_Delay(NTC_DELAY_FRAME);
	}

debugPrint("ntc getpresetunits FAILED" );

	return false;
}
function getPotsInCol(col){
var count=0;
var items = NTC_GetItems(me);
for (var i = 0; i<items.length;i++){
	if(items[i].mode==2){
		//if ((items[i].x==col) || (items[i].x==col+4) || (items[i].x==col+8 ) || (items[i].x==col+12))
		for (var a=col;	a<(col+13); a=a+4){
			if (items[i].x==a)
				count++
		}
	}
}

return count;
}
function getFirstPotCode(col){

var items = NTC_GetItems(me);
for (var i = 0; i<items.length;i++){
	if(items[i].mode==2){
		if (items[i].x==col)
			return items[i].code;		
	}
}
return false;
}
function debugPrintBeltGID(){

var items = NTC_GetItems(me);
for (var i = 0; i<items.length;i++){
	if(items[i].mode==2){
		debugPrint(items[i].gid);
	}
}
}
function NTC_GetItems(who)
{

	var _items;
	
	if(arguments.length < 1)
		who = me;
		debugPrint("ntcGetItems who:"+who.name);
	for(var i = 0 ; i < 2 ; i++)
	{
		_items = who.getItems();

		if(_items)
			return _items;

		NTC_Delay(NTC_DELAY_FRAME);
	}

	return false;
}

function NTC_GetBeltSize()
{
	debugPrint("ntcGetbeltsize" );
	var _items = NTC_GetItems();

	if(!_items)
		return 1;

	for(var i = 0 ; i < _items.length ; i++)
	{
		if(_items[i].mode == 1 && _items[i].bodylocation == 8)
		{
			if(_items[i].code == "lbl" || _items[i].code == "vbl")
				return 2;
			else if(_items[i].code == "mbl" || _items[i].code == "tbl")
				return 3;

			return 4;
		}
	}

	return 1;
}

function NTC_FindScript(scriptname)
{
	debugPrint("ntc findScript" );
	var _script;
	var _scriptpath;

	_script = getScript();

	if(_script)
	{
		do
		{
			_scriptpath = _script.name.toLowerCase();

			if(_scriptpath.indexOf(scriptname.toLowerCase()) != -1)
				return _script;
		} while(_script.getNext());
	}

	return false;
}

function NTC_SendMsgToScript(scriptname, msg)
{
	//debugPrint("ntcSentMsgToScript" );
	var _script = NTC_FindScript(scriptname);

	if(_script)
	{
		_script.send(msg);

		return true;
	}

	return false;
}

function NTC_StopScript(scriptname)
{
	debugPrint("ntc Stop Script" );
	var _script = NTC_FindScript(scriptname);

	if(_script)
	{
		_script.stop();

		return true;
	}

	return false;
}

function NTC_WaitCastDone()
{
	debugPrint("ntc WaitCastDone" );
	
	var _mode;

	for(var i = 0 ; i < 75 ; i++)
	{
		_mode = me.mode;

		if(!(_mode == 7 || _mode == 8 || (_mode >= 10 && _mode <= 16) || _mode == 18))
		{
			debugPrint("ntc WaitCastDone return true mode: "+me.mode );
			return true;
		}
		
		NTC_Delay(25);
	}
	
	debugPrint("ntc WaitCastDone return false" );
	
	return false;
}

function NTC_SendLogToOOG(logString)
{
	debugPrint("ntc sendlogtooog: "+logString );
	return sendCopyData(null, "D2Manager 2.5", (65535<<16)|65535, me.windowtitle+";"+logString);
}

// Internal function
function NTC_CheckDoCastInt()
{
	debugPrint("ntc CheckDoCast" );
	var _mode;

	for(var i = 0 ; i < 10 ; i++)
	{
		_mode = me.mode;

		if(_mode == 7 || _mode == 8 || (_mode >= 10 && _mode <= 16) || _mode == 18){
			debugPrint("ntc CheckDoCast returning true mode:"+me.mode );
			return true;
		}
		NTC_Delay(NTC_DELAY_FRAME);
	}
	debugPrint("ntc CheckDoCast returning false mode:"+me.mode );
	return false;
}

/**
*	@name	NTC_GetBaseStat()
*
*	@args	tableid
*			classid
*			statid
*
*	@desc	Wrapper for getBaseStat() to handle a potential crash condition in D2BS v1.4
*	@see	http://docs.d2bs.org/group__globalFunctions.html#gb4ebba6bce777cb9705e199af1d08bc8
*/
function NTC_GetBaseStat(tableid, classid, statid)
{
	//	handle arguments
	//	TODO:	make a tableArgsInRange() closure?
	if(arguments.length != 3)
	{
		print('NTC_GetBaseStat() failed, incorrect argument number');
		return undefined;
	}
	
	var output;

	// if crash condition getBaseStat() called on item table**	(no longer 1.4 specific.. crashes in item table can happen often in 1.3.3 as well)
	if(tableid == 0)
	{
		//check for itemTable object
		if(!isIncluded('objects/itemTable.json'))
			NTC_Include('objects/itemTable.json');
		if(typeof(itemTable) != 'object')
		{
			print('ÿc1Error: NTC_GetBaseStat() couldn\'t find itemTable object to reference..');
			return undefined;
		}
			
		//collect array of itemTable object keys
		var tablekeys = Object.keys(itemTable);

		//check type of statid and do requested lookup
		if(typeof(statid) == 'string')
		{
			if((tablekeys.indexOf(statid) != -1) && (classid < itemTable[statid].length))
			{
				return itemTable[statid][classid];
			}
		}
		else if(typeof(statid) == 'number')
		{
			if((statid < tablekeys.length) && (classid < itemTable[tablekeys[statid]].length))
			{
				return itemTable[tablekeys[statid]][classid];
			}
		}

		//if here, object statid didn't exist, or classid array would have been out of bounds;
		return undefined;
	}
	else	//not an item table lookup, so call getBaseStat() with given args then return output;
	{
		output = getBaseStat.apply(this, arguments);
	}

	return output;
}


function NTC_OpenAreaChests()
{
	var ChestIds = [5, 6, 87, 92, 143, 140, 141, 144, 146, 147, 148, 176, 177, 181, 183, 198, 
					240, 241, 242, 243, 329, 330, 331, 332, 333, 334, 335, 336, 354, 355, 356, 
					371, 387, 389, 390, 391, 397, 405, 406, 407, 413, 420, 424, 425, 430, 431, 
					432, 433, 454, 455, 501, 502, 504, 505, 580, 581];

	var ChestLocs =[];

	var unit = getPresetUnits(me.area);
	
	if(unit)
	{
		for(var j = 0; j < unit.length; j++)
		{
			for(var c = 0; c < ChestIds.length; c++)
			{
				if(ChestIds[c] == unit[j].id)
				{
					ChestLocs.push([unit[j].roomx*5+unit[j].x,unit[j].roomy*5+unit[j].y]);
				}
			}
		}
	}

	while(ChestLocs.length > 0)
	{
		ChestLocs.sort(NTA_SortRoomInt);
		
		var _room = ChestLocs.shift();
		NTM_MoveTo(_room[0], _room[1]);		
		
		var chest = NTC_GetUnit(NTC_UNIT_OBJECT, "chest");
		if(chest)
		{
			do
			{
				if(getDistance(me.x,me.y,chest.x,chest.y)<5)
				{
					if(NTC_OpenChest(copyUnit(chest)))
						NTSI_PickItems();
				}
			}while(chest.getNext());
		}
	}
}


function NTC_Say(input)
{
	if(input != undefined && input.isString && !input.isEmpty)
		say(input);
}