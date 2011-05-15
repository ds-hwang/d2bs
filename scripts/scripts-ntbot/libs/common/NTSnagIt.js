if(!isIncluded("njip.dbl")) include("njip.dbl");

var _NTSI_ItemQueue = null;
var _NTSI_CheckedItems = new Array();
var _NTSI_IgnoredList = new Array();
var NTSI_GID_AREA = new Array();
function NTSI_LoadNIPFiles()
{
	var _filename = "";

	if(NTConfig_NIPFilePath.length == 0)
		return false;

	for(var i = 0 ; i < NTConfig_NIPFilePath.length ; i++)
	{
		_filename = "settings/" + NTConfig_NIPFilePath[i];

		njipOpenFile(_filename);
	}
return true;
}

function NTSI_PickItems(item)
{
	debugPrint("NTSI_PickItems");
	
	var tmpTeleportSetting = me.useTeleport;
	if(NTConfig_UseTeleportForPickit)
		me.useTeleport = true;
	else
		me.useTeleport = false;
	
	var i, _mode;

	while(1)
	{
		_mode = me.mode;

		if(!(_mode == 7 || _mode == 8 || (_mode >= 10 && _mode <= 16) || _mode == 18))
		{
			break;
		}
		
		NTC_Delay(NTC_DELAY_FRAME);
	}

	NTSI_SnagIt(item);
	NTC_PingDelay(NTConfig_SnagDelay);
	me.useTeleport = tmpTeleportSetting;
}

function NTSI_SnagIt(item)
{
	debugPrint("NTSI_Snagit");
	
	var _itemtosnag;

	_NTSI_ItemQueue = new NTSI_ItemListInt(item, NTConfig_SnagRange);
	_NTSI_ItemQueue.Update();

	_itemtosnag = _NTSI_ItemQueue.getClosest();

	if(_itemtosnag)
	{
		var _itemfound;
		var _itemunit;
		var _prearea;

		do
		{
			_itemfound = false;
			_itemunit = NTC_GetUnit(NTC_UNIT_ITEM, _itemtosnag.ClassID);

			if(_itemunit)
			{
				do
				{
					if(_itemunit.type == NTC_UNIT_ITEM && _itemunit.gid == _itemtosnag.GID && (_itemunit.mode == 3 || _itemunit.mode == 5))
					{
						_itemfound = true;
						break;
					}
				} 
				while(_itemunit.getNext());
			}

			if(!_itemfound)
			{
				_itemtosnag.ShouldSnag = 0;
				_itemtosnag.HasBeenSnagged = true;
			}

			if(_itemtosnag.ShouldSnag == 10)
			{
				if(NTT_CheckSpace(_itemunit.sizex, _itemunit.sizey))
				{
					switch(_itemunit.quality)
					{
					case 4:
						print("Pick up ÿc3" + _itemunit.name);
						break;
					case 5:
						print("Pick up ÿc2" + _itemunit.name);
						break;
					case 6:
						print("Pick up ÿc9" + _itemunit.name);
						break;
					case 7:
						print("Pick up ÿc4" + _itemunit.name);
						break;
					case 8:
						print("Pick up ÿc8" + _itemunit.name);
						break;
					default:
						print("Pick up ÿc0" + _itemunit.name);

						if(_itemunit.itemType != 4 && (_itemunit.itemType < 76 || _itemunit.itemType > 81)) {
							NTC_SendLogToOOG("Kept [" + NTC_ItemQualityToNameList[_itemunit.quality] + "] " + _itemunit.name);
							// Item Logger Start
							var _isEthereal = (_itemunit.getFlag(0x400000)) ? "1" : "0";
							writeLog(_itemunit, me, _isEthereal, 0);
							// Item Logger End
						}
						break;
					}

					_itemtosnag.HasBeenSnagged = NTSI_PickUpItemInt(_itemunit);
				}
				else
				{
					if(_itemtosnag.Retry)
						_itemtosnag.HasBeenSnagged = true;
					else
					{
						_prearea = me.area;

						if(NTTM_CheckAct() && NTcfgSellWhenFull)
						{
							_itemtosnag.Retry = true;

							NTTMGR_TownManager(true);

							if(!NTTM_TownMove("portalspot"))
								return false;

							if(!NTM_UsePortal("BluePortal", _prearea))
								return false;
						}
					}
				}
			}

			_itemtosnag = _NTSI_ItemQueue.getClosest();
		} while(_itemtosnag);
	}

	return true;
}

// Internal function
function NTSI_ItemListInt(_items, _range)
{
	this._list = new Array();
	this._mylastarea = me.area;
	this.Range = _range;

	this.Update = NTSI_ItemListInt_update;
	this.getClosest = NTSI_ItemListInt_getclosest;
	this.rvCount=0 ;
	this.hpCount=0;
	this.mpCount=0;
	this.tpCount=0;
	this.idCount=0;
	this.keyCount=0;
	
	if(_items instanceof Array)
		this._list = _items;

	if(!this.Range)
		this.Range = 40;

	function NTSI_ItemListInt_update(type)
	{
		if(this._mylastarea != me.area || this._list.length > 100 || _NTSI_CheckedItems > 100)
		{
			this._list.length = 0;
			_NTSI_CheckedItems.length = 0;
			this._mylastarea = me.area;
		}

		var i;
		var _grounditem = NTC_GetUnit(NTC_UNIT_ITEM);
		var _skipitem;

		if(_grounditem)
		{
			do
			{
				if(_grounditem.type == NTC_UNIT_ITEM && (_grounditem.mode == 3 || _grounditem.mode == 5))
				{
					_skipitem = false;

					if(NTSI_IsItemIgnoredInt(_grounditem.classid))
						continue;

					for(i = 0 ; i < this._list.length ; i++)
					{
						if(this._list[i].GID == _grounditem.gid)
							_skipitem = true;
					}

					if(_skipitem)
						continue;

					for(i = 0 ; i < _NTSI_CheckedItems.length ; i++)
					{
						if(_NTSI_CheckedItems[i] == _grounditem.gid)
							_skipitem = true;
					}

					if(_skipitem)
						continue;

					if(getDistance(me, _grounditem) < this.Range)
					{
						this._list.push(new NTSI_ItemInt(_grounditem));
						_NTSI_CheckedItems.push(_grounditem.gid);
					}
				}
			} while(_grounditem.getNext());
		}
	}

	function NTSI_ItemListInt_getclosest(range, x, y)
	{
		if(!x)
			x = me.x;

		if(!y)
			y = me.y;

		var i;
		var _itemdist = 999;
		var _itemindex = null;

		for(i = 0 ; i < this._list.length ; i++)
		{
			if(this._list[i].IsBeingSnagged && !this._list[i].HasBeenSnagged)
				return this._list[i];
		}

		if(range)
		{
			for(i = 0 ; i < this._list.length ; i++)
			{
				if(this._list[i].Distance < _itemdist && _distance < range && this._list[i].ShouldSnag > 0 && !this._list[i].HasBeenSnagged)
				{
					_itemdist = this._list[i].Distance;
					_itemindex = i;
				}
			}
		}
		else
		{
			for(i = 0 ; i < this._list.length ; i++)
			{
				if(this._list[i].Distance < _itemdist && this._list[i].ShouldSnag > 0 && !this._list[i].HasBeenSnagged)
				{
					_itemdist = this._list[i].Distance;
					_itemindex = i;
				}
			}
		}

		if(_itemindex != null)
		{
			this._list[_itemindex].IsBeingSnagged = true;
			return this._list[_itemindex];
		}
		else
			return null;
	}
}

function NTSI_ItemInt(_item)
{
debugPrint("NtSI ItemInt");
	this.ClassID = _item.classid;
	this.GID = _item.gid;
	this.Distance = getDistance(me, _item);
	this.ShouldSnag = 0;
	this.IsBeingSnagged = false;
	this.HasBeenSnagged = false;
	this.Retry = false;
	if (NT_PickUtility){
	
			if (_item.code == "key"){ //keys
				if (NTTMGR_CheckKeys(true) && (10-_NTSI_ItemQueue.keyCount)){				
				this.ShouldSnag = 4; //pick key
				_NTSI_ItemQueue.keyCount ++;
					return true;// dont njipcheckgooditem
				}
			}
			if (_item.code == "tsc"){ //town scroll
				if (NTTMGR_CheckScrolls(true) < (17-_NTSI_ItemQueue.tpCount)){				
				this.ShouldSnag = 10; //pick the scroll
				_NTSI_ItemQueue.tpCount ++;
					return true;// dont njipcheckgooditem
				}
			}
			if (_item.code == "isc"){ //identify
				if (NTTMGR_CheckScrolls() < (17-_NTSI_ItemQueue.idCount) && NTT_GetTome()){					
					this.ShouldSnag = 10; //pick the scroll
					_NTSI_ItemQueue.idCount ++;
					return true;// dont njipcheckgooditem
				}
			}
			if (_item.code == "rvl" || _item.code == "rvs"){
					if ((getBeltColAvail(_item.code)-_NTSI_ItemQueue.rvCount)>0){ //check to see if pot will go in belt
						this.ShouldSnag = 10;	
						_NTSI_ItemQueue.rvCount ++;
						return true;
					}else{
						return true; // dont pick juvies we dont needs
					}
			}
			if (_item.code == "hp5" || _item.code == "hp4"){			
					if ((getBeltColAvail(_item.code)-_NTSI_ItemQueue.hpCount)>0){ //check to see if pot will go in belt
						this.ShouldSnag = 10;	
						_NTSI_ItemQueue.hpCount ++;
						return true;// dont njipcheckgooditem
					}
			}
			if (_item.code == "mp5" || _item.code == "mp4"){			
					if ((getBeltColAvail(_item.code)-_NTSI_ItemQueue.mpCount)>0){ //check to see if pot will go in belt						
						this.ShouldSnag = 10;	
						_NTSI_ItemQueue.mpCount ++;
						return true;// dont njipcheckgooditem
					}			
			}
		}
	if(njipCheckGoodItem(_item, NJIP_CHECK_SIMPLE) != 0)
		this.ShouldSnag = 10;
		return true;
}
function saveItemLoc(item){
	if (item){
		
		
		NTSI_GID_AREA.push([item.gid,me.area]);
	}
}
function NTSI_PickUpItemInt(snagitem)
{
saveItemLoc(snagitem);
	debugPrint("NtSI pickUpItemInt");
	
	for(var i = 0 ; i < 20 ; i++)
	{
		if((i % 5) == 0 && (snagitem.mode == 3 || snagitem.mode == 5))
		{
			if(getDistance(me, snagitem) > 3)
			{
				NTM_MoveTo(snagitem.x, snagitem.y);
			}

			if(NTC_ClearCursor())
			{
				snagitem.interact();
			}
		}
		if(snagitem.code == "isc" || snagitem.code =="tsc"){
			NTC_PingDelay(250);
			return true;		
		}
			
		NTC_Delay(100);

		if(snagitem.mode != 3 && snagitem.mode != 5)
		{
			NTC_PingDelay(250);
			return true;
		}
	}

	return false;
}

function NTSI_IsItemIgnoredInt(classid)
{
debugPrint("NtSI IsItemIgnored");
	if(_NTSI_IgnoredList.length == 0)
	{
		_NTSI_IgnoredList = new Array();

		_NTSI_IgnoredList.push(526); // Arrows quiver
		_NTSI_IgnoredList.push(528); // Bolts quiver
		_NTSI_IgnoredList.push(543); // Keys
		_NTSI_IgnoredList.push(518); // Tome of Town Portal
		_NTSI_IgnoredList.push(519); // Tome of Identify
		//_NTSI_IgnoredList.push(529); // Scroll of Town Portal
		//_NTSI_IgnoredList.push(530); // Scroll of Identify
		_NTSI_IgnoredList.push(514); // Antidote potion
		_NTSI_IgnoredList.push(513); // Stamina potion
		_NTSI_IgnoredList.push(517); // Thawing potion
		_NTSI_IgnoredList.push(80); // Rancid Gas Pot
		_NTSI_IgnoredList.push(81); // Oil Potion
		_NTSI_IgnoredList.push(82); // Choking Gas Pot
		_NTSI_IgnoredList.push(83); // Exploding Pot
		_NTSI_IgnoredList.push(84); // Strangling Gas
		_NTSI_IgnoredList.push(85); // Fulminating Pot
	}

	for(var i = 0 ; i < _NTSI_IgnoredList.length ; i++)
	{
		if(classid == _NTSI_IgnoredList[i])
			return true;
	}

	return false;
}
function Dprint(message){
sendCopyData(null, "OOG", 0,"Dprint"+message );
}
function NTSI_FastSnag(){
if (!FastSnag) return true;

var item = false;
var count = 0;
	
	var tempBob_ItemList = new Array;  //minimize time using shared list
	while (Bob_ItemList.length >= 1) {	
		tempBob_ItemList.push(Bob_ItemList[Bob_ItemList.length-1]);		
		Bob_ItemList.pop();		
	}
	
		//Dprint("fast snag "+Bob_ItemList.toSource());
	while (tempBob_ItemList.length >= 1) {	
	inFastSnag=true;
		item=NTSI_findItemGID(tempBob_ItemList[tempBob_ItemList.length-1]);
		if  (item){
			//Dprint("found "+item.name);
			if(njipCheckGoodItem(item, NJIP_CHECK_SIMPLE) !=0){
				if(NTT_CheckSpace(item.sizex, item.sizey)){
					print ("Fast Snaging "+item.name);
					NTSI_PickUpItemInt(item);
					}
			}		
		}
		tempBob_ItemList.pop();		
	}
	inFastSnag=false;
return true;
}
function NTSI_findItemGID(GID){
	var _itemunit = getUnit(4,-1,-1,GID);
		if (!_itemunit) { //retry to find item
			var _itemfound = false;
			_itemunit = getUnit(4);
			if(_itemunit){
					do{
						if(_itemunit.gid == GID && (_itemunit.mode == 3 || _itemunit.mode == 5)){					
							_itemfound = true;
							_itemunit = copyUnit(_itemunit);
							break;
						}
					} while(_itemunit.getNext());
					if (!_itemfound){
						//Dprint("finding failed "+GID);
						return false;
					}						
			}
		}
		return _itemunit;
}function getBeltColAvail(code){
var BeltSize = NTC_GetBeltSize();
var retVal=0;
for (var col = 0; col < 4; col++){
	var firstPot=getFirstPotCode(col)
	if (code.indexOf(NTConfig_BeltColType[col]) != -1 && (!firstPot || firstPot.indexOf(NTConfig_BeltColType[col]) != -1 ) ){
		retVal=retVal+(BeltSize-getPotsInCol(col));	//how many pots can we pick 	
	}		
}
return retVal;
}