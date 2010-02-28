Unit.findItem = function(nStats) {
	var nRet = [];
	if (nStats.hasOwnProperty("ownerid") && nStats.ownerid != me.gid)
		var unit = getUnit(1, false, false, nStats.ownerid);
	else if (nStats.hasOwnProperty("ownerclass") && nStats.ownerclass != me.classid)
		var unit = getUnit(1, nStats.ownerclass);
	else
		var unit = me;
	if (!unit)
		return nRet;
	var nUnit = unit.getItem();
	ItemLoop:do {
		for (var n in nStats) {
			if (n == "ownerid" || n == "ownerclass") {
				continue;
			} else if (n == "scode") {
				if (nUnit["code"].substr(0, 2) != nStats[n])
					continue ItemLoop;
			} else
				if (nUnit[n] != nStats[n])
					continue ItemLoop;
		}	
		nRet.push(copyUnit(nUnit));
	} while(nUnit.getNext());
	if ("scode" in nStats) {
		nRet.sort(function(a, b) {
			return (b.classid - a.classid);
		});
	}
	return nRet;
}

/*
	Unit.buy([nBulkBuy])
		Buys an item, waits for item to be bought, returns true/false.
	Parameters:
		nBulkBuy(Optional) - Pass a true to Unit.buy for it to bulk buy(Fill tome or belt).
*/
Unit.prototype.buy = function() {
	try {
		//Insure this is called with a item unit.
		if (this.type != 4)
			throw new Error("Called with a non-item unit.");
		
		//Check to insure we are in a shop
		if (!getUIFlag(UIFlags.NPCShop))
			throw new Error("Called without being in a npc shop.");
			
		//Check to make sure this is a NPC's unit.
		var nOwner = this.getParent();
		if (nOwner.classid != getInteractedNPC().classid)
			throw new Error("Called with an item not in NPC's Shop.");
			
		//Get our current gold to compare against.
		var nGold = me.getStat(14) + me.getStat(15);
		
		Interface.message(DetailedDebug, "Attempting to buy " + this.name);
		//Try to buy three times.
		for (var n = 0; n < 3; n++) {
			this.shop((arguments.length == 1) ? 6 : 2);
			
			//Wait and check gold
			var nDelay = getTickCount();
			while((getTickCount() - nDelay) < 2000) {
				if (nGold != (me.getStat(14) + me.getStat(15))) {
					Interface.message(DetailedDebug, "Succesfully bought item.");
					return true;
				}
				delay(10);
			}
		}
		throw new Error("Unable to buy " + this.name + " after 3 tries.");
		return false;
	} catch(e) {
		mBot.throwError(e)
		return false;
	}
}

/*
	Unit.sell()
		Sell a unit to an NPC.
*/
Unit.prototype.sell = function () {
	try {
		//Insure this is called with a item unit.
		if (this.type != 4)
			throw new Error("Called with a non-item unit.");
		
		//Check to insure we are in a shop
		if (!getUIFlag(UIFlags.NPCShop))
			throw new Error("Called without being in a npc shop.");
			
		//Check to make sure we own the item
		var nOwner = this.getParent();
		if (nOwner.gid != me.gid)
			throw new Error("Called with not our own item.");
			
		//Get our current gold to compare against.
		var nGold = me.getStat(14) + me.getStat(15);
		
		if (this.node == 3 && this.code != "aqv" && this.code != "cqv")
			throw new Error("Trying to sell item we're wearing!")
		
		if (this.location != 0 && this.location != 1)
			this.toCursor();
			
		
		Interface.message(DetailedDebug, "Attempting to sell " + this.name);
		//Try to buy three times.
		for (var n = 0; n < 3; n++) {
			this.shop(1);
			
			//Wait and check gold
			var nDelay = getTickCount();
			while((getTickCount() - nDelay) < 2000) {
				if (nGold != (me.getStat(14) + me.getStat(15))) {
					Interface.message(DetailedDebug, "Successfully sold " + this.name);
					return true;
				}
				delay(10);
			}
		}
		throw new Error("Unable to sell item after 3 tries.");
		return false;
	} catch(e) {
		mBot.throwError(e)
		return false;
	}
}
	
/*
	Unit.toCursor()
		Places item on cursor
*/
Unit.prototype.toCursor = function () {
	try {
		//Make sure it is an item
		if (this.type != 4)
			throw new Error("Called with a non-item unit.");
			
		//Check to see if it is already on cursor.
		if (me.itemoncursor && getUnit(100).gid == this.gid)
			return true;
		
		//Check if there is an item on cursor.
		if (me.itemoncursor)
			throw new Error("Called when there is an item already on cursor!");
			
		Interface.message(DetailedDebug, "Attempting to pickup " + this.name);
		//Loop three times to try and pick it up.
		for (var n = 0; n < 3; n++) {
			clickItem(0, this);
			
			var nDelay = getTickCount();
			while((getTickCount() - nDelay) < 2000) {
				if (me.itemoncursor) {
					Interface.message(DetailedDebug, "Successfully picked up item.");
					return true;
				}
				delay(10);
			}
		}
		throw new Error("Failed to place item onto cursor in three tries.");
	} catch(e) {
		mBot.throwError(e);
		return false;
	}
}

/*
	Unit.drop()
		Pickups if needed, and drops the given item.
*/
Unit.prototype.drop = function() {
	try {
		//Make sure it is an item
		if (this.type != 4)
			throw new Error("Called with a non-item unit.");
			
		//Check to see if it is already on ground
		if (this.mode == Mode.Item.OnGround)
			return true;
			
		//Try to move onto cursor if not already there.
		if (!this.toCursor())
			return false;//No need to throw an error has toCursor does.
			
		Interface.message(DetailedDebug, "Attempting to drop " + this.name);
		for (var n = 0; n < 3; n++) {
			clickMap(0,0,me.x + 2, me.y + 2);
			
			var nDelay = getTickCount();
			while((getTickCount() - nDelay) < 2000) {
				if (!me.itemoncursor) {
					Interface.message(DetailedDebug, "Successfully dropped item.");
					return true;
				}
				delay(50);
			}
		}
		throw new Error("Unable to drop " + this.name + " after three tries.");
	} catch(e) {
		mBot.throwError(e);
		return false;
	}
}

/*
	Unit.pickFromGround()
		Picks the given item from the ground.
*/
Unit.prototype.pickFromGround = function () {
	try {
		//Make sure it is an item
		if (this.type != 4)
			throw new Error("Called with a non-item unit.");
			
		//Check if the item is picked up.
		if (mBot.checkMode(Mode.Item.Group.Picked, this))
			return true;
			
		//Check if not on ground.
		if (!mBot.checkMode(Mode.Item.Group.Ground, this))
			throw new Error("Given item is on the ground.");
			
		Interface.message(DetailedDebug, "Attempting to pickup " + this.name);
		for (var n = 0; n < 3; n++) {
			this.interact();
			
			var nDelay = getTickCount();
			while((getTickCount() - nDelay) < 2000) {
				if (mBot.checkMode(Mode.Item.Group.Picked, this)) {
					Interface.message(DetailedDebug, "Successfully picked up item.");
					return true;
				}
				delay(10);
			}
		}
		throw new Error("Unable to pickup item after three tries.");
	} catch(e) {
		mBot.throwError(e);
		return false;
	}
}

/*
	Unit.drink()
		Drinks the given potion
	Parameters:
		nMerc - Pass true to give potion to merc.
*/
Unit.prototype.drink = function () {
	try {
		//Make sure it is an item
		if (this.type != 4)
			throw new Error("Called with a non-item unit.");
		
		//Make sure it is a potion.
		if (!this.isPotion())
			throw new Error("Called with a non-potion unit.");
			
		Interface.message(Normal, "Drinking " + this.name + " for " + (arguments.length == 2 ? "Merc" : "Self") + ".");
		if (arguments.length == 1 && arguments[0])
			clickItem(this, 0);
		else
			this.interact(0);
		return true;
	} catch(e) {
		mBot.throwError(e);
		return false;
	}
}

/*
	Unit.getDurability
		Returns the percentage of durability left.
*/
Unit.prototype.getDurability = function () {
	return Math.round((this.getStat(Stats.Durability) / this.getStat(Stats.MaxDurability)) * 100);
}

/*
	Unit.isPotion()
		Returns if the given item is a potion.
*/
Unit.prototype.isPotion = function() {
	for (var n in categories['potions'])
		if (categories['potions'][n] == this.code)
			return true;
	return false;
}

/*
	Unit.useScroll()
		Opens a portal or places ID scroll on cursor.
*/
Unit.prototype.useScroll = function() {
	try {
		//Make sure it is an item
		if(this.type != 4)
			throw new Error("Called with a non-item unit.");
			
		//If it is a TP Scroll, open TP.
		if (this.code == "tsc") {
			clickItem(2, this);
			return true;
		}
		
		//If it isn't the other type of scroll, throw error
		if (this.code != "isc")
			throw new Error("Called with a non-scroll unit.");
		
		//Try to place id scroll on cursor.
		Interface.message(DetailedDebug, "Attempting to use " + this.name);
		for (var n = 0; n < 3; n++) {
			clickItem(2, this);
			
			var nDelay = getTickCount();
			while((getTickCount() - nDelay) < 2000) {
				if (getCursorType() == 6) {
					Interface.message(DetailedDebug, "Successfully used scroll");
					return true;
				}
				delay(50);
			}
		}
		throw new Error("Unable to place scroll on cursor after three tries.");
	} catch(e) {
		mBot.throwError(e);
		return false;
	}
}

Unit.prototype.identify = function () {
	try {
		if (getCursorType() != 6)
			throw new Error("There is no scroll on the cursor.");
		
		if (this.getFlag(pickitFlag.identified))
			return true;
			
		Interface.message(DetailedDebug, "Attempting to identify " + this.name);
		for (var n = 0; n < 3; n++) {
			clickItem(0, this);
			
			var nDelay = getTickCount();
			while((getTickCount() - nDelay) < 2000) {
				if (this.getFlag(pickitFlag.identified)) {
					Interface.message(DetailedDebug, "Succesfully identified item.");
					return true;
				}
				delay(50);
			}
		}
		throw new Error("Unable to identify item after three tries.");
	} catch(e) {
		mBot.throwError(e);
		return false;
	}
}

Unit.prototype.beltCount = function () {
	switch(this.code) {
		case "lbl"://Sash
		case "vbl"://Light Belt
			return 8;
		break;
		case "mbl"://Belt
		case "tbl"://Heavy Belt
			return 12;
		break;
		default:
			return 16;
		break;
	}
	return 0;
}

Unit.__defineGetter__("loc", function(){
	switch(this.node) {
		case 1://Storage
			switch(this.location) {
				case 0://Inventory
					return itemLocation.Inventory;
				break;
				case 3://Cube
					return itemLocation.Cube;
				break;
				case 4://Stash
					return itemLocation.Stash;
				break;
			}
		break;
		case 2://Belt
			return itemLocation.Belt;
		break;
		case 3://Equip
			return itemLocation.Equip;
		break;
	}
	return 255;
});