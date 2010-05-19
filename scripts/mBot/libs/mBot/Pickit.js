var Pickit = new function () {
	this.pickit = [];
	this.config = {};
	
	this.Init = function () {
		this.readPickit();
	}
	
	this.readPickit = function () {
		//Open Pickit!
		var file = File.open("profile//" + Interface.profileName + "//Pickit.ini", 0);
		
		//Insure we actually have a pickit file!
		if (!file.readable) {
			Interface.message(Warning, "Unable to find pickit file, *BOT WILL NOT PICK ANYTHING*");
			return false;
		}
		
		//Make sure the global pickit array is empty.
		this.pickit = [];
		
		this.currentEntry = {};
		
		//Loop pickit!
		var lines = file.readAllLines();
		file.close();
		for (var lineNum = 1, line = lines[0]; (lineNum - 1) < lines.length; lineNum++, line = lines[lineNum - 1]) {
			
			//Clean out any comments in the line.
			if (line.indexOf("//") != -1)
				line = line.substr(0, line.indexOf("//"));
			
			//Check if the line is empty
			if (line.length == 0)
				continue;
			
			//Check for a new pickit entry.
			var desc = /\[(.+?)\]/.exec(line);
			if (desc) {
				//Check if we have an entry that needs to be stored.
				if (this.currentEntry.hasOwnProperty("Name"))
					this.pickit.push(this.currentEntry);
				//Set the current entry to the new entry.
				this.currentEntry = {Name:desc[1], Stats:[], Priority:0, Line:lineNum};
				continue;
			}
			
			//Make sure we're inside a pickit entry.
			if (!this.currentEntry.hasOwnProperty("Name"))
				continue;
				
			//Check for stats.
			var stat = /(.+?)(=|==|>=|<=|!=|<>|<|>)(.+)/.exec(line);
			if (!stat || stat.length != 4) {
				Interface.message(Warning, "Invalid pickit syntax on line " + lineNum + ", skipping stat entry.");
				continue;
			}
			//Clean up the entries.
			var oper = stat[2];
			var value = stat[3].trim().toLowerCase();
			var stat = stat[1].trim().toLowerCase();
			
			//Verify the stat is valid.
			switch(stat) {
				case "cat":
				case "category":
					//Check if the value given is a valid category.
					if (!categories.hasOwnProperty(value)) {
						Interface.message(Warning, "Category given at line " + lineNum + " is not a valid category, permanently skipping.");
						this.currentEntry = {};
						continue;
					}
				break;
				case "priority":
					this.currentEntry.Priority = parseInt(value);
					continue;
				break;
				case "name":
					if (!itemCodes.hasOwnProperty(value)) {
						Interface.message(Warning, "Name given at line " + lineNum + " is not a valid name, permanently skipping.");
						this.currentEntry = {};
						continue;
					}
				break;
				case "code":
					if (!itemCodes.IsValid(value)) {
						Interface.message(Warning, "Item Code given at line " + lineNum + " is not a valid code, permanently skipping.");
						this.currentEntry = {};
						continue;
					}
				break;
				case "ilvl":
				case "levelreq":
				break;
				case "quality":
					if (!pickitQuality.hasOwnProperty(value)) {
						Interface.message(Warning, "Quality given at line " + lineNum + " is not a valid code, permanently skipping.");
						this.currentEntry = {};
						continue;
					}
				break;
				default:
					if (!pickitFlag.hasOwnProperty(stat) && !pickitStats.hasOwnProperty(stat)) {
						Interface.message(Warning, "Stat given at line " + lineNum + " is not valid, permanently skipping.");
						this.currentEntry = {};
						continue;
					}
			}

			this.currentEntry.Stats.push({Stat:stat, Oper:oper, Value:value, Line:lineNum});
		}
		//Push last entry if it is there.
		if (this.currentEntry.hasOwnProperty("Name"))
			this.pickit.push(this.currentEntry);
		
		//Tell the user if we don't have any entries.
		if (this.pickit.length == 0) {
			Interface.message(Warning, "There is no pickit entries in Pickit.ini, *BOT WILL NOT PICK ANYTHING*");
			return false;
		}
		
		Interface.message(Normal, "Pickit loaded with " + this.pickit.length + " entries.");
		return true;
	}
	
	this.checkItem = function(item) {
		if (!item)
			return false;
		entry:for(var entry in this.pickit) {
			stat:for(var stat in this.pickit[entry].Stats) {
				var statEntry = this.pickit[entry].Stats[stat];
				switch(statEntry.Stat) {
					case "cat":
					case "category":
						//Loop all the item codes in the given category.
						for (var cat in categories[statEntry.Value])
							//Compare item code versus given item's code.
							if (categories[statEntry.Value][cat] == item.code)
								continue stat;
					break;
					case "name":
						//Convert name to code and compare versus the item code.
						if (itemCodes[statEntry.Value] == item.code)
							continue stat;
					break;
					case "code":
						//Compare given code versus the item's code.
						if (statEntry.Value == item.code)
							continue stat;
					break;
					case "ilvl":
						//Do a operator comparison for the item level.
						if (this.compareStat(item.ilvl, statEntry.Oper, statEntry.Value))
							continue stat;
					break;
					case "levelreq":
						//Do a operator comparison for the level requirements.
						if (this.compareStat(item.levelreq, statEntry.Oper, statEntry.Value))
							continue stat;
					break;
					case "quality":
						//Convert quality string to int, do operator comparison.
						if (this.compareStat(item.quality, statEntry.Oper, pickitQuality[statEntry.Value]))
							continue stat;
					break;
					default:
						//Check if the stat is a item flag
						if (pickitFlag.hasOwnProperty(statEntry.Stat))
							if (this.compareStat(item.getFlag(pickitFlag[statEntry.Stat]), statEntry.Oper, statEntry.Value))
								continue stat;
					
						//Check if the item is unidentified, if so, we can't do a stat check since we can't see them.
						if (!item.getFlag(pickitFlag.identified))
							continue stat;
							
						if (pickitStats.hasOwnProperty(statEntry.Stat))
							if (this.compareStat(item.getStat(pickitStats[statEntry.Stat]), statEntry.Oper, statEntry.Value))
								continue stat;
				}
				continue entry;
			}
			return {entry:this.pickit[entry], item:copyUnit(item)};
		}
		return false;
	}
	
	this.compareStat = function(nStat1, nOpt, nStat2) {
	if (nStat1 > 0)
		switch(nOpt) {
			case "=":
			case "==":
				if (nStat1 == nStat2)
					return true;
			break;
			case ">":
				if (nStat1 > nStat2)
					return true;
			break;
			case "<":
				if (nStat1 < nStat2)
					return true;
			break;
			case ">=":
				if (nStat1 >= nStat2)
					return true;
			break;
			case "!=":
			case "<>":
				if (nStat1 != nStat2)
					return true;
			break;
		}
		return false;
	}

	this.detailedInfo = function(item) {
		var stats = item.getStat(-1);
		print(stats.toSource());
	}
	
	this.pick = function () {
		var pickList = [];
		//Loop all the items on the ground.
		var item = getUnit(4);
		if (!item)
			return true;
		do {
			//Check the item to see if it is valid.
			var pick = this.checkItem(item);
			//If valid item, push to list.
			if (!!pick)
				pickList.push(pick);
		} while(item.getNext());
		Interface.message(Normal, "Found " + pickList.length + " items to pickup.");
		//Sort the items based on priority.
		pickList.sort(function(a, b) { return b.entry.Priority - a.entry.Priority; });
		
		//Pick em up!
		for (var n in pickList) {
			Storage.Reload();
			if (!Storage.Inventory.CanFit(pickList[n].item)) {
				Interface.message(Normal, "Cannot fit " + pickList[n].item.name + " into Inventory!");
				continue;
			}
			
			Pather.itemMove(pickList[n].item);
				
			if (pickList[n].item.pickFromGround()) {
				Interface.message(Normal, "Picked upÿc" + itemColor[pickList[n].item.quality] + " " + pickList[n].item.name);
			}
		}
		return true;
	}
}