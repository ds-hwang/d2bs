var Town = new function () {
	this.config = {};
	this.invRef = [];
	
	this.Init = function () {
		this.config = Interface.readConfig("Town", [
			{Name:"MinHealPercent", Default:80}, {Name:"MinManaPercent", Default:70},
			{Name:"MinTPs", Default:10}, {Name:"MinDurability", Default:70}, {Name:"PotionRows", Default:"hmmj"}, {Name:"HealRows", Default:1},
			{Name:"ManaRows", Default:2}, {Name:"UseMerc", Default:false}]);
		this.config.MinManaPots = 8;
		this.invRef = eval(Interface.read("Internal", "InvRef", Storage.Inventory.toSource()));
	}
	
	this.tick = function () {
		this.doHeal();
		this.doIdentify();
		this.doStash();
		this.doMerc();
		this.doPotions();
		this.doTPs();
		//this.doRepair();
		me.cancel(0);
		me.cancel(1);
	}
	
	/*
		this.useNPC(nType)
			Moves to and interacts with an NPC that can do given job.
		Parameters:
			nType - Type of NPC you need, "Scrolls" - NPC that has scrolls.
	*/
	this.useNPC = function(nType) {
		try {
			var npcId = Area[me.act].Jobs[nType];
			var npcName = Area[me.act].NPCNames[npcId];
			Interface.message(DetailedDebug, "Entering Town.useNPC with job: " + nType + ", npc id: " + npcId + ", npc name: " + npcName);
			
			//Check if we are currently interacted with someone.
			if (getInteractedNPC()) {
				var npcUnit = getInteractedNPC();
				Interface.message(Debug, "We are currently interacted with " + npcUnit.name);
				if (npcUnit.classid != npcId) {
					Interface.message(Debug, "We need to goto " + npcName + ", so we are cancelling out.");
					me.cancel(0);
					me.cancel(1);
				}
			}
			
			//Check if we are already near the wanted npc.
			if (!getUIFlag(UIFlags.NPCShop) && !getUIFlag(UIFlags.NPCMenu)) {
				Interface.message(Normal, "Moving to " + npcName + " for " + nType);
				//Check if we have the unit's NPC.
				var nUnit = getUnit(1, npcId);
				if (!nUnit) {
					//Not close enough to get unit's NPC, moving to their preset area.
					Interface.message(Debug, "Couldn't find " + npcName + "'s unit! Moving to preset first.")
					if (!Pather.movePreset(1, npcId))
						throw new Error("Unable to find unit and preset for " + nType);
				}
				Interface.message(Debug, "Trying to find and move to real unit.");
				
				//Get NPC's unit.
				nUnit = getUnit(1, npcId);
				if (!nUnit)
					throw new Error("Unable to find " + npcName + " after moving to their preset.");
					
				//Move to the unit
				Interface.message(DetailedDebug, getDistance(me, nUnit) + " yards away from " + npcName);
				if (getDistance(me, nUnit) > 5)
					if (!Pather.moveTo({unit:nUnit, pop:true}))
						throw new Error("Unable to move to " + npcName + "'s unit.");
					
				//Try to interact with NPC three times
				for (var n = 0; n < 3; n++) {
					Interface.message(Debug, "Attempting to interact with " + npcName + " attempt: " + (n + 1));
					nUnit.interact();
					
					var nTimer = getTickCount();
					while((getTickCount() - nTimer) < 1000) {
						if (getUIFlag(UIFlags.NPCMenu))
							break;
					}
					if (getUIFlag(UIFlags.NPCMenu))
						break;
					me.cancel(0);
					me.cancel(1);
				}
				
				//Make sure we are interacted
				if (!getUIFlag(UIFlags.NPCMenu))
					throw new Error("Unable to interact with " + npcName + " after three tries");
			} else {
				Interface.message(Debug, "Attempting to get our currently interacted NPC's unit.");
				//Grab the current NPC's unit.
				var nUnit = getInteractedNPC();
				if (!nUnit)
					throw new Error("Unable to get the interacted NPC's unit, odd.");
			}
			
			//Since we are in the NPC Menu, we have healed.
			if (nType == "Heal")
				return nUnit;
				
			//If we are here to ress merc, it's the time!
			if (nType == "Merc") {
				Interface.message(Normal, "Attempting to ressurect merc.");
				
				//Make sure we have enough gold!
				if (mBot.gold < me.mercrevivecost)
					throw new Error("We do not have enough gold to ressurect merc.");
					
				//Try three times to ressurect merc.
				for (var n = 0; n < 3; n++) {
					Interface.message(Debug, "Attempting to ressurect merc, attempt: " + (n + 1));
					nUnit.useMenu(NPCMenu.RessurectMerc);
					
					var nTimer = getTickCount();
					while((getTickCount() - nTimer) < 2000) {
						if (me.getMerc(1))
							return true;
						delay(5);
					}
					this.cancel();
				}
				throw new Error("Unable to ressurect merc after three tries!");
			}
			
			//Identify items time.
			if (nType == "Cain") {
				//Just use menu item, no way in D2BS to confirm.
				Interface.message(Normal, "Attempting to identify items via Cain.");
				nUnit.useMenu(NPCMenu.IdentifyItems);
				return nUnit;
			}
			
			//Check if we are already in shop.
			if (!getUIFlag(UIFlags.NPCShop)) {
				Interface.message(Normal, "Going into " + npcName + "'s shop.");
				for (var n = 0; n < 3; n++) {
					Interface.message(Debug, "Attempting to open " + npcName + "'s shop, attempt: " + n + 1);
					nUnit.useMenu(Area[me.act].Interact[nType]);
					
					var nTimer = getTickCount();
					while((getTickCount() - nTimer) < 2000) {
						if (getUIFlag(UIFlags.NPCShop))
							break;
						delay(5);
					}
					if (getUIFlag(UIFlags.NPCShop))
						break;
				}
				if (!getUIFlag(UIFlags.NPCShop))
					throw new Error("Unable to open " + npcName + "'s shop after three tries.");
			}
			
			//Waiting for population
			Interface.message(Debug, "Waiting for shop to populate.");
			var nTimer = getTickCount();
			while((getTickCount() - nTimer) < 5000) {
				if (nUnit.itemcount > 10) {
					Interface.message(Normal, "Successfully interacted and opened shop with " + npcName);
					return nUnit;
				}
				delay(5);
			}
			throw new Error("Timed out waiting for " + npcName + "'s shop to populate.");
		} catch(e) {
			mBot.throwError(e);
			return false;
		}
	}
	
	//								Town Mercenary Functions
	/* Town.needMerc()
			Checks if we have a merc, and if we are configured to ressurect the merc
	*/
	this.needMerc = function () {
		//Do we even want a merc?
		if (!this.config.UseMerc) {
			Interface.message(Debug, "Set not to ressurect the mercenary.");
			return false;
		}
			
		//Is our merc alive?
		if (me.getMerc(1)) {
			Interface.message(Debug, "Merc is alive, no need to ressurect.");
			return false;
		}
			
		//Merc must be dead and we want him!
		return true;
	}
	
	/* Town.doMerc()
			Goes and ressurects the merc if needed.
	*/
	this.doMerc = function() {
		//Check if we even need the merc.
		if (!this.needMerc())
			return true;
			
		//Go grab the merc
		if (!this.useNPC("Merc"))
			return false;
			
		Interface.message(Normal, "Mercenary has been revived.");
		return true;
	}
	
	//								Town Heal Functions
	/* Town.needHeal()
			Checks if we need to heal
	*/
	this.needHeal = function () {
		//Check if the health is above the threshold
		if ((me.hp / me.hpmax * 100) < this.config.MinHealPercent) {
			Interface.message(Debug, "We need to heal, our life is at " + Math.round(me.hp / me.hpmax * 100) + "% below " + this.config.MinHealPercent);
			return true;
		}
		
		//Check if the mana is above the threshold
		if ((me.mp / me.mpmax * 100) < this.config.MinManaPercent) {
			Interface.message(Debug, "We need to heal, our mana is at " + Math.round(me.mp / me.mpmax * 100) + "% below " + this.config.MinManaPercent);
			return true;
		}
		
		Interface.message(Debug, "Both are heal and mana are above the given thresholds.");
		return false;
	}
	
	/* Town.doHeal()
			Goes to an NPC to heal if needed.
	*/
	this.doHeal = function () {
		//Do we actually need to heal?
		if (!this.needHeal())
			return true;
		
		//Move to the healer
		if (!this.useNPC("Heal"))
			return false;
			
		Interface.message(Normal, "We have now fully healed.");
		return true;
	}
	
	//								Town Potion Functions
	/* Town.needPotions()
		Checks if the belt is empty and needs refilling
	*/
	this.needPotions = function () {
		//Check if we need health potions
		this.config.MinHealPots = 8;
		if (mBot.getPotion("Heal", true) < this.config.MinHealPots) {
			Interface.message(Debug, "We need to buy Heal potions.");
			return true;
		}
		
		//Check if we need mana potions
		this.config.MinManaPots = 8;
		if (mBot.getPotion("Mana", true) < this.config.MinManaPots) {
			Interface.message(Debug, "We need to buy Mana potions.");
			return true;
		}
		Interface.message(Debug, "Our belt is fully stocked with potions.");
		return false;
	}
	
	/* Town.doPotions()
		Goes to NPC and buys the amount of potions wanted
	*/
	this.doPotions = function () {
		try {
			//Check if we actually need potions
			if (!this.needPotions())
				return true;
				
			//Move to the npc and get the unit.
			var nUnit = this.useNPC("Pots");
			
			//Make sure we are actually at NPC.
			if (!nUnit)
				return false;
				
			//Determine how many potions we need.
			var nHeal = this.config.MinHealPots - mBot.getPotion("Heal", true);
			var nMana = this.config.MinManaPots - mBot.getPotion("Mana", true);
			Interface.message(Debug, "We need " + nHeal + " healing potions and " + nMana + " mana potions.");
			
			//Check if we need heal pots
			if (nHeal > 0) {
				Interface.message(Normal, "We need to buy " + nHeal + " healing potions from " + nUnit.name);
				//Find the potions in the NPC's shop
				var nHealPots = Unit.findItem({scode:"hp", ownerid:nUnit.gid});
				
				//Make sure the NPC has the wanted potions.
				if (nHealPots.length == 0)
					throw new Error("Unable to find healing potions in NPC's shop.");
					
				Interface.message(Debug, "There are " + nHealPots.length + " potions in the shop.");
					
				//Buy the potions.
				for (var n = 0; n < nHeal; n++) {
					nHealPots[0].buy();
					delay(200);
				}
			}
			
			//Check if we need mana pots
			if (nMana > 0) {
				Interface.message(Normal, "We need to buy " + nMana + " mana potions from " + nUnit.name);
				//Find the potions in the NPC's Shop.
				var nManaPots = Unit.findItem({scode:"mp", ownerid:nUnit.gid});
				
				//Make sure they actually have the potions.
				if (nManaPots.length == 0)
					throw new Error("Unable to find mana potions in NPC's shop.");
					
				Interface.message(Debug, "There are " + nManaPots.length + " potions in the shop.");
				
				//Buy the potions
				for (var n = 0; n < nMana; n++) {
					nManaPots[0].buy();
					delay(200);
				}
			}
			Interface.message(Normal, "Potions are now fully stocked.");
			return true;
			
		} catch(e) {
			mBot.throwError(e);
			return false;
		}
	}
	
	//								Town TP Functions
	this.needTPs = function () {
	
		//Check for the tome
		var nTome = Unit.findItem({code:"tbk", location:0, mode:0, ownerid:me.gid})
		if (nTome.length == 0) {
			Interface.message(Debug, "We do not have a tome, need to buy one.");
			return true;
		}
		
		//Check the TP amount
		if (nTome[0].getStat(70) < this.config.MinTPs) {
			Interface.message(Debug, "We do not have enough TPs in our tome.");
			return true;
		}
		
		Interface.message(Debug, "Our tome is well stocked with TPs.");
		return false;
	}
	
	/* Town.doTPs()
		Gos to an NPC and buys tp scrolls.
	*/
	this.doTPs = function() {
		//Check if we even need TPs
		if (!this.needTPs())
			return true;
			
		//Move to the NPC with scrolls
		var nUnit = this.useNPC("Scrolls");
	
		//Make sure we are at NPC
		if (!nUnit)
			return false;
			
		var nTome = Unit.findItem({code:"tbk", location:0, mode:0, ownerid:me.gid});
		if (nTome.length == 0) {
			Interface.message(Normal, "We need to buy a tome for TPs.");
			var nNPCsTome = Unit.findItem({code:"tbk", ownerid:nUnit.gid});
			if (nNPCsTome.length == 0)
				throw new Error("The NPC doesn't have a tome.");
			nNPCsTome[0].buy();
			delay(500);
			nTome = Unit.findItem({code:"tbk", location:0, mode:0, ownerid:me.gid});
			if (nTome.length == 0)
				throw new Error("Unable to find the tome we bought, maybe the inventories full?");
		}
		var nNPCsTPs = Unit.findItem({code:"tsc", ownerid:nUnit.gid});
		if (nNPCsTPs.length == 0)
			throw new Error("Unable to find scrolls in the NPC's shop.");
		
		print(nNPCsTPs.toSource());
		print(nNPCsTPs[0].loc);
		Interface.message(Normal, "We need to buy " + (20 - nTome[0].getStat(70)) + " scrolls from NPC.");
		nNPCsTPs[0].buy(true);
		Interface.message(Normal, "Tome is now fully stocked.");
		return true;
	}

	//								Town Repair Functions
	/* Town.needRepair()
			Checks all items to see if they are in need of repair
	*/
	this.needRepair = function () {
		var nItems = Unit.findItem({location:255, node:3, ownerid:me.gid});
		for (var n in nItems) {
			if (nItems[n].getDurability() < this.config.MinDurability && !nItems[n].getFlag(0x00400000)) {
				Interface.message(Debug, nItems[n].name + " needs to repaired.");
				return true;
			}
		}
		Interface.message(Debug, "All items appear to be in good repair.");
		return false;
	}
	
	/* Town.doRepair()
		Goes to an NPC and repairs all items.
	*/
	this.doRepair = function () {
		if (!this.needRepair())
			return true;
			
		if (!this.useNPC("Repair"))
			return false;
		
		me.repair();
		Interface.message(Normal, "Successfully repaired all items.");
		return true;
	}
	
	this.needIdentify = function () {
		var items = Storage.Inventory.Compare(this.invRef);
		for (var n in items)
			if (!items[n].getFlag(pickitFlag.identified))
				return true;
		return false;
	}
	
	this.doIdentify = function () {
		try {
			//Check if we even have itamz.
			if (!this.needIdentify())
				return true;

			//Move to the NPC with scrolls
			var nUnit = this.useNPC("Scrolls");
			
			if (!nUnit)
				return false;
				
			var nIDs = Unit.findItem({code:"isc", ownerid:nUnit.gid});
			
			if (nIDs.length == 0)
				throw new Error("Unable to find ID Scrolls in NPC's Shop.");
				
			this.newItems = Storage.Inventory.Compare(this.invRef);
			var items = "Identifying:";
			for (var n in this.newItems)
				items += " ÿc" + itemColor[this.newItems[n].quality] + " " + this.newItems[n].name;
			Interface.message(Normal, items);
			for (var n in this.newItems) {
				if (!this.newItems[n].getFlag(pickitFlag.identified)) {
					nIDs[0].buy();
					
					var scroll = Unit.findItem({code:"isc", ownerid:me.gid, location:0});
					if (scroll.length == 0)
						throw new Error("Unable to find ID Scroll in Inventory.");
						
					if (!scroll[0].useScroll())
						continue;

					if (!this.newItems[n].identify())
						continue;
				}
			}
			return true;
		} catch(e) {
			mBot.throwError(e);
			return false;
		}	
	}
	
	this.needStash = function () {
		return Storage.Inventory.Compare(this.invRef).length > 0;
	}
	
	this.doStash = function () {
		if (getUIFlag(UIFlags.NPCMenu)) {
			me.cancel(0);
			me.cancel(1);
		}
		
		this.newItems = Storage.Inventory.Compare(this.invRef);
		for (var n in this.newItems) {
			if (!!Pickit.checkItem(this.newItems[n])) {
				Interface.message(Normal, "Keepingÿc" + itemColor[this.newItems[n].quality] + " " + this.newItems[n].name);
				Storage.Stash.MoveTo(this.newItems[n]);
			} else {
				if (getInteractedNPC())
					this.newItems[n].sell();
				else
					this.newItems[n].drop();
			}
		}
	}
}