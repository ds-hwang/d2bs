var _NTTMGR_BuyPots;
var _NTTMGR_BuyTP;
var _NTTMGR_BuyID;
var _NTTMGR_BuyKey;
var _NTTMGR_DoGamble;
var _NTTMGR_DoHeal;
var _NTTMGR_DoID;
var _NTTMGR_DoRepair;
var _NTTMGR_DoRevive;
var _NTTMGR_RemoveCurse;


function NTTMGR_TownManager(simple)
{
	if(arguments.length < 1)
		simple = false;

	NTC_ClearCursor();
	NTT_CleanPotions();

	_NTTMGR_BuyPots = NTT_CheckBelt();
	_NTTMGR_BuyTP = (NTTMGR_CheckScrolls(1) < 4);
	_NTTMGR_BuyID = (NTcfgDropWhenFull && NTTMGR_CheckScrolls(0) < 20);
	_NTTMGR_BuyKey = (NTTMGR_CheckKeys() < 2);
	_NTTMGR_DoGamble = (!simple && NTConfig_GambleIt && NTC_StashGoldFull());
	_NTTMGR_DoHeal = NTT_CheckHeal();
	_NTTMGR_DoID = NTT_CheckUnids();
	_NTTMGR_DoRepair = NTT_CheckRepair(40);
	_NTTMGR_DoRevive = NTT_CheckMerc();
	_NTTMGR_RemoveCurse = NTTMGR_CheckIsCursed();

	NTC_PutSkill(115, NTC_HAND_RIGHT);
	
	if(me.classid==6)
		NTC_DoCast(258, NTC_HAND_RIGHT);
		
	switch(me.act)
	{
		case 1:
			if(_NTTMGR_DoHeal || _NTTMGR_RemoveCurse || _NTTMGR_BuyPots || _NTTMGR_BuyTP || _NTTMGR_BuyID || _NTTMGR_BuyKey || _NTTMGR_DoID)
				NTTMGR_VisitAkara();
				
			if(NTT_CheckInventory())
				NTTMGR_VisitStash();
				
			if(_NTTMGR_DoRevive)
				NTTMGR_VisitKashya();
				
			if(_NTTMGR_DoRepair)
				NTTMGR_VisitCharsi();
				
			if(_NTTMGR_DoGamble)
			{
				NTTMGR_VisitGheed();
				
				if(NTT_CheckInventory())
					NTTMGR_VisitStash();
			}
		break;

		case 2:
			if(_NTTMGR_DoHeal || _NTTMGR_RemoveCurse || _NTTMGR_DoRepair)
				NTTMGR_VisitFara();
				
			if(_NTTMGR_BuyPots || _NTTMGR_BuyTP || _NTTMGR_BuyID || _NTTMGR_DoID)
				NTTMGR_VisitDrognan();
				
			if(_NTTMGR_BuyKey)
				NTTMGR_VisitLysander();
				
			if(NTT_CheckInventory())
				NTTMGR_VisitStash();
				
			if(_NTTMGR_DoRevive)
				NTTMGR_VisitGreiz();
				
			if(_NTTMGR_DoGamble)
			{
				NTTMGR_VisitElzix();

				if(NTT_CheckInventory())
					NTTMGR_VisitStash();
			}
		break;

		case 3:
			if(_NTTMGR_DoHeal || _NTTMGR_RemoveCurse || _NTTMGR_BuyPots || _NTTMGR_BuyTP || _NTTMGR_BuyID || _NTTMGR_DoID)
				NTTMGR_VisitOrmus();
				
			if(NTT_CheckInventory())
				NTTMGR_VisitStash();
				
			if(_NTTMGR_DoRevive)
				NTTMGR_VisitAsheara();
				
			//if( _NTTMGR_BuyKey)
			//   NTTMGR_VisitHratli();
			
			if(_NTTMGR_DoGamble)
			{
				NTTMGR_VisitAlkor();

				if(NTT_CheckInventory())
					NTTMGR_VisitStash();
			}
		break;

		case 4:
			if(_NTTMGR_DoHeal || _NTTMGR_RemoveCurse || _NTTMGR_BuyPots || _NTTMGR_BuyTP || _NTTMGR_BuyID || _NTTMGR_BuyKey || _NTTMGR_DoID)
				NTTMGR_VisitJamella(false);
				
			if(_NTTMGR_DoRepair)
				NTTMGR_VisitHalbu();
				
			if(NTT_CheckInventory())
				NTTMGR_VisitStash();
				
			if(_NTTMGR_DoRevive)
				NTTMGR_VisitTyrael();
				
			if(_NTTMGR_DoGamble)
			{
				NTTMGR_VisitJamella(true);

				if(NTT_CheckInventory())
					NTTMGR_VisitStash();
			}
		break;

		case 5:
			if(_NTTMGR_DoHeal || _NTTMGR_RemoveCurse || _NTTMGR_BuyPots || _NTTMGR_BuyTP || _NTTMGR_BuyID || _NTTMGR_BuyKey || _NTTMGR_DoID)
				NTTMGR_VisitMalah();
				
			if(NTT_CheckInventory())
				NTTMGR_VisitStash();
				
			if(_NTTMGR_DoRepair)
				NTTMGR_VisitLarzuk();
				
			if(_NTTMGR_DoRevive)
				NTTMGR_VisitQualKehk();
				
			if(_NTTMGR_DoGamble)
			{
				NTTMGR_VisitAnya();

				if(NTT_CheckInventory())
					NTTMGR_VisitStash();
			}
		break;
	}
}


function NTTMGR_VisitStash()
{
	var _stash;

	NTTM_TownMove("stash");

	for(var i = 0 ; i < 10 ; i++)
	{
		_stash = NTC_GetUnit(NTC_UNIT_OBJECT, NTC_UNIT_STASH);

		if(_stash)
		break;
	}

	if(_stash)
	{
		NTT_ClearInventory();
		NTT_DoInteract(_stash);
		NTT_ManageStash();
		NTT_MenuCancel();
	}
}


function NTTMGR_VisitAkara()
{
	var _npc;

	NTTM_TownMove("akara");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_AKARA);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);

		if(_NTTMGR_BuyPots || _NTTMGR_BuyTP || _NTTMGR_BuyKey || _NTTMGR_DoID)
		{
			if(NTT_DoTrade(_npc))
			{
				if(_NTTMGR_BuyPots)
					NTT_FillBelt(_npc);
					
				if(_NTTMGR_BuyTP)
					NTT_FillTome(_npc, 1);
					
				if(_NTTMGR_BuyID)
				  NTT_FillTome(_npc, 0);
					
				if(_NTTMGR_BuyKey)
					NTT_FillKey(_npc);
					
				if(_NTTMGR_DoID)
					NTTMGR_IDItems(_npc);
			}
		}

		NTT_MenuCancel();
	}
}


function NTTMGR_VisitKashya()
{
	var _npc;

	NTTM_TownMove("kashya");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_KASHYA);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);
		NTT_ReviveMerc(_npc);
		NTT_MenuCancel();

		if(NTConfig_ResetWeapon)
			NTT_ResetWeaponMerc();
	}
}


function NTTMGR_VisitCharsi()
{
	var _npc;

	NTTM_TownMove("charsi");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_CHARSI);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);
		
		if(NTT_DoTrade(_npc))
		{
			NTT_RepairItems(_npc);
			NTT_MenuCancel();
		}
	}
}


function NTTMGR_VisitGheed()
{
	var _npc;

	NTTM_TownMove("gheed");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_GHEED);

		if(_npc)
			break;
	}

	if(_npc)
	{
		while(NTC_MyGold() > NTConfig_GambleStopGold)
		{
			NTT_DoInteract(_npc);
			
			if(NTT_DoTrade(_npc, 1))
			{
				NTT_GambleIt(_npc);
				NTT_MenuCancel();
			}
		}
	}
}


function NTTMGR_VisitFara()
{
	var _npc;

	NTTM_TownMove("fara");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_FARA);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);

		if(_NTTMGR_DoRepair)
		{
			if(NTT_DoTrade(_npc))
			{
				NTT_RepairItems(_npc);
			}
		}

		NTT_MenuCancel();
	}
}


function NTTMGR_VisitDrognan()
{
	var _npc;

	NTTM_TownMove("drognan");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_DROGNAN);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);
		
		if(NTT_DoTrade(_npc))
		{
			if(_NTTMGR_BuyPots)
				NTT_FillBelt(_npc);								
				
			if(_NTTMGR_BuyTP)
				NTT_FillTome(_npc, 1);
				
			if(_NTTMGR_BuyID)
				NTT_FillTome(_npc, 0);				
				
			if(_NTTMGR_DoID)
				NTTMGR_IDItems(_npc);

			NTT_MenuCancel();
		}
	}
}


function NTTMGR_VisitLysander()
{
	var _npc;

	NTTM_TownMove("lysander");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_LYSANDER);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);
		
		if(NTT_DoTrade(_npc))
		{
			NTT_FillKey(_npc);
			NTT_MenuCancel();
		}
	}
}


function NTTMGR_VisitGreiz()
{
	var _npc;

	NTTM_TownMove("greiz");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_GREIZ);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);
		NTT_ReviveMerc(_npc);
		NTT_MenuCancel();

		if(NTConfig_ResetWeapon)
			NTT_ResetWeaponMerc();
	}
}


function NTTMGR_VisitElzix()
{
	var _npc;

	NTTM_TownMove("elzix");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_ELZIX);

		if(_npc)
			break;
	}

	if(_npc)
	{
		while(NTC_MyGold() > NTConfig_GambleStopGold)
		{
			NTT_DoInteract(_npc);
			
			if(NTT_DoTrade(_npc, 1))
			{
				NTT_GambleIt(_npc);
				NTT_MenuCancel();
			}
		}
	}
}


function NTTMGR_VisitOrmus()
{
	var _npc;

	NTTM_TownMove("ormus");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_ORMUS);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);

		if(_NTTMGR_BuyPots || _NTTMGR_BuyTP || _NTTMGR_BuyID || _NTTMGR_DoID)
		{
			if(NTT_DoTrade(_npc))
			{
				if(_NTTMGR_BuyPots)
					NTT_FillBelt(_npc);
					
				if(_NTTMGR_BuyTP)
					NTT_FillTome(_npc, 1);
					
				if(_NTTMGR_BuyID)
				  NTT_FillTome(_npc, 0);					
					
				if(_NTTMGR_DoID)
					NTTMGR_IDItems(_npc);
			}
		}

		NTT_MenuCancel();
	}
}


function NTTMGR_VisitAsheara()
{
	var _npc;

	NTTM_TownMove("asheara");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_ASHEARA);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);
		NTT_ReviveMerc(_npc);
		NTT_MenuCancel();

		if(NTConfig_ResetWeapon)
			NTT_ResetWeaponMerc();
	}
}


function NTTMGR_VisitHratli()
{
	var _npc;

	NTTM_TownMove("hratli");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_HRATLI);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);
		
		if(NTT_DoTrade(_npc))
		{
			if(_NTTMGR_DoRepair)
				NTT_RepairItems(_npc);
				
			if(_NTTMGR_BuyKey)
				NTT_FillKey(_npc);

			NTT_MenuCancel();
		}
	}
}


function NTTMGR_VisitAlkor()
{
	var _npc;

	NTTM_TownMove("alkor");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_ALKOR);

		if(_npc)
			break;
	}

	if(_npc)
	{
		while(NTC_MyGold() > NTConfig_GambleStopGold)
		{
			NTT_DoInteract(_npc);
			
			if(NTT_DoTrade(_npc, 1))
			{
				NTT_GambleIt(_npc);
				NTT_MenuCancel();
			}
		}
	}
}


function NTTMGR_VisitJamella(gamble)
{
	var _npc;

	NTTM_TownMove("jamella");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_JAMELLA);

		if(_npc)
			break;
	}

	if(_npc)
	{
		if(gamble)
		{
			while(NTC_MyGold() > NTConfig_GambleStopGold)
			{
				NTT_DoInteract(_npc);
				
				if(NTT_DoTrade(_npc, 1))
				{
					NTT_GambleIt(_npc);
					NTT_MenuCancel();
				}
			}
		}
		else
		{
			NTT_DoInteract(_npc);

			if(_NTTMGR_BuyPots || _NTTMGR_BuyTP || _NTTMGR_BuyID || _NTTMGR_BuyKey || _NTTMGR_DoID)
			{
				if(NTT_DoTrade(_npc))
				{
					if(_NTTMGR_BuyPots)
						NTT_FillBelt(_npc);
						
					if(_NTTMGR_BuyTP)
						NTT_FillTome(_npc, 1);
					
					if(_NTTMGR_BuyID)
				    NTT_FillTome(_npc, 0);
						
					if(_NTTMGR_BuyKey)
						NTT_FillKey(_npc);
						
					if(_NTTMGR_DoID)
						NTTMGR_IDItems(_npc);
				}
			}

			NTT_MenuCancel();
		}
	}
}


function NTTMGR_VisitHalbu()
{
	var _npc;

	NTTM_TownMove("halbu");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_HALBU);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);
		
		if(NTT_DoTrade(_npc))
		{
			NTT_RepairItems(_npc);
			NTT_MenuCancel();
		}
	}
}


function NTTMGR_VisitTyrael()
{
	var _npc;

	NTTM_TownMove("tyrael");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_TYRAEL_ACT_4);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);
		NTT_ReviveMerc(_npc);
		NTT_MenuCancel();

		if(NTConfig_ResetWeapon)
			NTT_ResetWeaponMerc();
	}
}


function NTTMGR_VisitMalah()
{
	var _npc;

	NTTM_TownMove("malah");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_MALAH);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);

		if(_NTTMGR_BuyPots || _NTTMGR_BuyTP || _NTTMGR_BuyID || _NTTMGR_BuyKey || _NTTMGR_DoID)
		{
			if(NTT_DoTrade(_npc))
			{
				if(_NTTMGR_BuyPots)
					NTT_FillBelt(_npc);
					
				if(_NTTMGR_BuyTP)
					NTT_FillTome(_npc, 1);
					
				if(_NTTMGR_BuyID)
				  NTT_FillTome(_npc, 0);
					
				if(_NTTMGR_BuyKey)
					NTT_FillKey(_npc);
					
				if(_NTTMGR_DoID)
					NTTMGR_IDItems(_npc);
			}
		}

		NTT_MenuCancel();
	}
}


function NTTMGR_VisitLarzuk()
{
	var _npc;

	NTTM_TownMove("larzuk");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_LARZUK);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);
		
		if(NTT_DoTrade(_npc))
		{
			NTT_RepairItems(_npc);
			NTT_MenuCancel();
		}
	}
}


function NTTMGR_VisitQualKehk()
{
	var _npc;

	NTTM_TownMove("qual-kehk");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_QUALKEHK);

		if(_npc)
			break;
	}

	if(_npc)
	{
		NTT_DoInteract(_npc);
		NTT_ReviveMerc(_npc);
		NTT_MenuCancel();

		if(NTConfig_ResetWeapon)
			NTT_ResetWeaponMerc();
	}
}


function NTTMGR_VisitAnya()
{
	var _npc;

	NTTM_TownMove("anya");

	for(var i = 0 ; i < 10 ; i++)
	{
		_npc = NTC_GetUnit(NTC_UNIT_NPC, NTC_NPC_ID_ANYA_TOWN);

		if(_npc)
			break;
	}

	if(_npc)
	{
		while(NTC_MyGold() > NTConfig_GambleStopGold)
		{
			NTT_DoInteract(_npc);
			
			if(NTT_DoTrade(_npc, 1))
			{
				NTT_GambleIt(_npc);
				NTT_MenuCancel();
			}
		}
	}
}


function NTTMGR_CheckScrolls(_scrollType)
{
	var _items;
	var _tomecode;

	_items = NTC_GetItems();

	if(!_items)
		return 0;

	if(_scrollType)
		_tomecode = "tbk";
	else
		_tomecode = "ibk";

	for(var i = 0 ; i < _items.length ; i++)
	{
		if(_items[i].mode == 0 && _items[i].location == ITEMLOC_INVENTORY && _items[i].code == _tomecode)
			return _items[i].getStat(70);
	}

	return 0;
}


function NTTMGR_CheckKeys()
{
	var _items = me.getItems();

	if(!NTConfig_OpenChest || me.classid == NTC_CHAR_CLASS_ASSASSIN)
		return 12;

	if(!_items || _items.length < 1)
		return 0;

	for(var i = 0 ; i < _items.length ; i++)
	{
		if(_items[i].mode == 0 && _items[i].location == ITEMLOC_INVENTORY && _items[i].code == "key")
			return _items[i].getStat(70);
	}

	return 0;
}


function NTTMGR_IDItems(npc)
{
  //Not in shop and npc requested
	if(!getUIFlag(0x0C) && npc)
		return false;
		
	var _tome, _scroll;
	var _result;
	var _items = NTT_GetUnidItems();

	if(!_items || _items.length < 1)
		return true;

	_tome = NTT_GetTome();

	if(_tome && _tome.getStat(70) < _items.length){
	  if(npc){
		  NTT_FillTome(npc);
		}	else {
		  print("KO");
		  return false;
		}
	}

  //print("OK");
	for(var i = 0 ; i < _items.length ; i++)
	{
		var _isEthereal = (_items[i].getFlag(0x400000)) ? "1" : "0";
	  
		if(njipCheckGoodItem(_items[i], NJIP_CHECK_SIMPLE) > 0)
		{
		  // Log only when checking in town
		  if(npc){
			NTC_SendLogToOOG("Kept [" + NTC_ItemQualityToNameList[_items[i].quality] + "] " + _items[i].name);
			// Item Logger Start
			writeLog(_items[i], me, _isEthereal, 0);
			// Item Logger End
		  }
		}
		else
		{
			if(_tome)
				_result = NTT_IdItem(_tome, _items[i]);
			else
			{
				_scroll = NTT_GetScroll();

				if(!_scroll && npc)
				{
					NTT_BuyScrolls(npc);
					_scroll = NTT_GetScroll();
				}

				_result = NTT_IdItem(_scroll, _items[i]);
			}

			if(npc && _result && njipCheckGoodItem(_items[i], NJIP_CHECK_SIMPLE) > 0) 
			{
				NTC_SendLogToOOG("Kept [" + NTC_ItemQualityToNameList[_items[i].quality] + "] " + _items[i].name);
				// Item Logger Start
				writeLog(_items[i], me, _isEthereal, 0);
				// Item Logger End				
			}
			else
			{
				if(npc && _items[i].getFlag(0x4000000) == 0)
				{
					// Item Logger Start
					writeLog(_items[i], me, _isEthereal, 1);
					// Item Logger End
					_items[i].shop(1);
					NTC_PingDelay(300);
				}
			}
		}
	}
	
	delay(750);
	
	return true;
}


// 2 = Poison, 9 = Amplify Damage, 19 = Weaken, 55 = Iron Maiden, 60 = Decrepify, 61 = Lower Resist
function NTTMGR_CheckIsCursed()
{
	var _merc;

	if(NTConfig_SkipHealCurses)
		return false;

	if(me.getState(2) || me.getState(9) || me.getState(19) || me.getState(55) || me.getState(60) || me.getState(61))
		return true;

	_merc = NTC_GetMerc();

	if(_merc)
	{
		if(_merc.getState(2) || _merc.getState(9) || _merc.getState(19) || _merc.getState(55) || _merc.getState(60) || _merc.getState(61))
			return true;
	}

	return false;
}


function NTTMGR_CheckSafe(level)
{
	var _result;

	switch(level)
	{
		case 3:
			_result = (NTT_CheckBelt() || NTTMGR_CheckIsCursed() || NTT_CheckMerc());
		break;
		
		case 2:
			_result = (NTTMGR_CheckIsCursed() || NTT_CheckMerc());
		break;
		
		case 1:
			_result = NTT_CheckMerc();
		break;
		
		default:
			_result = false;
		break;
	}

	if(_result)
	{
		var _prearea = me.area;

		if(!NTTM_CheckAct())
			return false;

		NTTMGR_TownManager(true);

		if(!NTTM_TownMove("portalspot"))
			return false;

		if(!NTM_UsePortal("BluePortal", _prearea))
			return false;

		if(NTConfig_PublicMode)
			NTM_MakeTP();
	}

	return true;
} 