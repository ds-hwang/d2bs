include("mBot/Skills.js");
include("common/Constants.js");
include("mBot/Skills.js");
include("common/Constants.js");
include("common/Interface.js");
include("common/Prototypes.js");
include("common/Statistics.js");
include("mBot/Pather.js");
include("mBot/Attacker.js");
include("mBot/Healer.js");
include("mBot/Item.js");
include("mBot/Town.js");
include("mBot/Sequencer.js");
include("mBot/Precast.js");

var mBot = new function () {
	this.Init = function () {
		Interface.Init();
		Attack.Init();
		Pather.Init();
		Healer.Init();
		Town.Init();
		Sequencer.Init();
		Precast.Init();
		
		this.__defineGetter__("gold", function() { return me.getStat(14) + me.getStat(15); });
	}
	
	this.checkMode = function(mode) {
		var unit;
		if (arguments.length == 2)
			unit = copyUnit(arguments[1]);
		else
			unit = me;
			
		if (!unit)
			return false;
			
		if (typeof(mode) == 'object')
		{
			for (var i in mode)
				if (unit.mode == mode[i])
					return true;
			return false;
		} else {
			return (unit.mode == mode)
		}
		return false;
	}
	
	this.inTown = function () {
		for (var levelId in Area.Town)
			if (me.area == Area.Town[levelId])
				return true;
		return false;
	}
	
	this.Print = function(nMsg) {
		print("ÿc5mBot ÿc#::ÿc0 " + nMsg);
		debugLog(nMsg);
	}
	
	this.switchWeapons = function () {
	
		//Check if they want specifics, and we already switched
		if (arguments.length > 0 && weaponSwitch(1) == arguments[0])
			return arguments[0];
			
		var currentTab = weaponSwitch(1);
		for (var n = 0; n < 3; n++) {		
			var nTimer = getTickCount();
			while((getTickCount() - nTimer) < 500) {
				//If we have changed tabs, then return new tab.
				if (currentTab != weaponSwitch(1))
					return weaponSwitch(1);
				delay(5);
			}
			weaponSwitch();
		}
		return weaponSwitch(1);
	}
	
	this.throwError = function(err) {
		this.fileName = err.fileName.substr(err.fileName.indexOf("scripts\\") + 8);
		this.Print("An exception has occured in " + this.fileName + ", line " + err.lineNumber + ", message: " + err.message);
		var nStack = err.stack.replace(err.stack.substr(err.stack.indexOf("@", err.stack.indexOf("@") + 1) + 1, err.stack.indexOf("scripts\\") + 7 - err.stack.indexOf("@", err.stack.indexOf("@") + 1)) + /./g, "");
		Interface.writeLog("Error.log", this.fileName + ", line, " + err.lineNumber + ", message: " + err.message + ", stack trace:\n " + nStack);
	}
	
	this.getPotion = function(nType) {
		var nPotions = {Heal:["hp5", "hp4", "hp3", "hp2", "hp1"], Mana:["mp5", "mp4", "mp3", "mp2", "mp1"], SmallJuv:["rvs"], FullJuv:["rvl"]};
		if (!nType in nPotions)
			return false;
		var nItems;
		var nCount = 0;
		for (var n in nPotions[nType]) {
			nItems = Unit.findItem({code:nPotions[nType][n], node:2});
			if (nItems.length > 0 && arguments.length == 1)
				break;
			nCount += nItems.length;
		}
		if (nItems.length == 0 && arguments.length == 1)
			return false;
		if (arguments.length == 2)
			return nCount;
		return nItems[0];
	}
}
