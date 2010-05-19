var Attack = new function () {
	this.config = {};
	this.preAttacks = [];
	this.timedAttacks = [];
	this.untimedAttacks = [];
	this.postAttacks = [];
	this.spamAttacks = [];

	
	this.Init = function () {
		//Load all attacks from file.
		var nDir = dopen("/libs/attacks/");
		var nFiles = nDir.getFiles();
		for (var n in nFiles) {
			if (nFiles[n].indexOf(".js") == -1)
				continue;
			if (me.getSkill(getSkillByName(nFiles[n].substr(0, nFiles[n].indexOf(".js"))), 1) == 0)
				continue;
			Interface.message(DetailedDebug, "Including attack sequence '" + nFiles[n] + "'.");
			include("attacks/" + nFiles[n]);
		}
		
		Interface.message(Normal, "We have " + this.preAttacks.length + " pre-attack(s), " + this.timedAttacks.length + " timed attack(s), " + this.untimedAttacks.length + " untimed attack(s), " + this.postAttacks.length + " post attack(s).");
		Interface.message(DetailedDebug, "Sorting attack sequences based on skill level.");
		this.sortFunc = function(a, b) {
			return (b.Skill.level - a.Skill.level);
		}
		this.preAttacks.sort(this.sortFunc);
		this.timedAttacks.sort(this.sortFunc);
		this.untimedAttacks.sort(this.sortFunc);
		this.postAttacks.sort(this.sortFunc);
	}
	
	this.attackAll = function() {
		var nUnits = [];
		var nUnit = getUnit(1);
		var nCallback = (arguments.length == 1) ? arguments[0] : false;
		
		do {
			if (this.isValidMonster(nUnit)) {
				if (!!nCallback && !nCallback(nUnit))
					continue;
				nUnits.push(copyUnit(nUnit));
			}
		} while(nUnit.getNext());
		Interface.message(Debug, "Found " + nUnits.length + " to kill.");
		if (nUnits.length == 0)
			return false;
		nUnits.sort(this.sort);
		var nString = "";
		for (var n in nUnits)
			if (this.isValidMonster(nUnits[n]))
				this.attack(nUnits[n]);
		return true;
	}
	
	this.attackMonster = function(mon) {
		var nUnit = getUnit(1, mon);
		if (!nUnit)
			return false;
		return this.attack(nUnit);
	}
	
	this.attackRange = function(nRange) {
		return this.attackAll(function(nUnit) { return (getDistance(me, nUnit) < nRange);});
	}
	
	this.sort = function(nUnit1, nUnit2) {
		if (nUnit1.spectype & 0x01 || nUnit1.spectype & 0x02 || nUnit1.spectype & 0x04)
			return -1;
		if (nUnit2.spectype & 0x01 || nUnit2.spectype & 0x02 || nUnit2.spectype & 0x04)
			return 1;
		var nResIDs = [58,59,60,61,62,101,102,103,104,105,229,278,279,280,281,282,645,646,647,667,668,669,670];
		for (var n in nResIDs) {
			if (nResIDs[n] == nUnit1.classid)
				return -1;
			if (nResIDs[n] == nUnit2.classid)
				return 1;
		}
		if (getDistance(me, nUnit1) < getDistance(me, nUnit2))
			return -1;
		return 1;
	}
	
	this.isValidMonster = function(Mon) {
	
		if (!Mon) {
			Interface.message(DetailedDebug, Mon.name + " is not a valid unit.");
			return false;
		}
			
		if ((Mon.hp == 0) || (Mon.mode == 12) || (Mon.mode == 0)) {
			Interface.message(DetailedDebug, Mon.name + " is dead.");
			return false;
		}
			
		if (Mon.getParent ()) {//Check if unit has a owner, if so, it's going to be a summon.
			Interface.message(DetailedDebug, Mon.name + " is a summon.");
			return false;
		}
		
		if ((Mon.area == 1) || (Mon.area == 70) || (Mon.area == 75) || (Mon.area == 103) || (Mon.area == 109)) {//Is Monster in Town
			Interface.message(DetailedDebug, Mon.name + " is in town.");
			return false;
		}
			
		if ((((Mon.classid >= 110) && (Mon.classid <= 113)) || (Mon.classid == 608))  && (Mon.mode == 8)) {// Ignore flying scavengers
			Interface.message(DetailedDebug, Mon.name + " is in the air, forgot missiles at home.");
			return false;
		}
			
		if ((Mon.classid == 68) && (Mon.mode == 14)) {// Ignore burrowing maggots
			Interface.message(DetailedDebug, Mon.name + " is a burrowing maggot.");
			return false;
		}
		
		if (((Mon.classid == 258) || (Mon.classid == 261)) && (Mon.mode == 14)) {// Ignore submerged WaterWatchers		
			Interface.message(DetailedDebug, Mon.name + " is underwater, forgot harpoon.");
			return false;
		}
		
		if ((Mon.name == "Baal") && me.area == Area.Act5.ThroneOfDestruction)
			return false;

		var BadIDs = new Array(326, 327, 328, 329, 330, 410, 411, 412, 413, 414, 415, 416, 366, 406,
								351, 352, 353, 266, 408, 516, 517, 518, 519, 522, 523, 543, 543, 545);
		
		for(var n = 0; n < BadIDs.length; n++) {
			if (Mon.classid == BadIDs[n]) {
				Interface.message(DetailedDebug, Mon.name + " is a bad monster.");
				return false;
			}
		}
		
		if ((Mon.name == "an evil force") || (Mon.name == "dummy")) { //Check name to ensure valid
			Interface.message(DetailedDebug, Mon.name + " is a, well look at the name.");
			return false;
		}
		
		//nothing else to check
		return true;
	}
	
	this.compareMonster = function(mon, list) {
		var nRet = [];
		for (var n in list)
			if (list[n].Predicate(mon))
				nRet.push(list[n]);
		if (arguments.length == 3)
			return (nRet.length > 0) ? nRet[0] : false;
		return nRet;
	}
	
	this.spam = function () {
		for (var n in this.spamAttacks)
			this.spamAttacks[n].Func();
	}
	
	this.attack = function(mon) {
		try {
			//Make sure we have a valid monster to play with.
			if (!this.isValidMonster(mon))
				return false;
				
			//Determine what we will use before starting.
			this.timedAttack = this.compareMonster(mon, this.timedAttacks, true);
			this.untimedAttack = this.compareMonster(mon, this.untimedAttacks, true);
			this.preAttack = this.compareMonster(mon, this.preAttacks);
			
			//Interface.message(Debug, "Attacking " + mon.name + " with " + this.timedAttack.Name + " and " + this.untimedAttack.Name);
			
			//Check if we even have any possible attack to use.
			if (!this.timedAttack && !this.untimedAttack)
				throw new Error("No available attacks to use against " + mon.name);
				
			//Run all pre-attack functions.
			for (var n in this.preAttack)
				this.preAttack[n].Func(mon);
			
			//Let the attacking begin.
			
			//Start out with a timed attack to get the counter going.
			if (this.timedAttack) {
				Interface.message(DetailedDebug, "Doing first timed attack");
				this.timer = getTickCount();
				this.timedAttack.Func(mon);
			}
			
			//Loop until monster is dead.
			while(this.isValidMonster(mon)) {
				//Check if we have a timed attack and if it is time to use it.
				if (this.timedAttack && ((getTickCount() - this.timer) > this.timedAttack.Delay)) {
					//Reset timer and use timed attack.
					this.timer = getTickCount();
					this.timedAttack.Func(mon);
				}
				//Use untimed attack
				if (this.untimedAttack)
					this.untimedAttack.Func(mon);
				delay(10);
			}
			
			this.postAttack = this.compareMonster(mon, this.postAttacks);
			//Last but not least, post-attack functions.
			for (var n in this.postAttack)
				this.postAttack[n].Func(mon);
			
			return true;
			
		} catch (e) {
			mBot.throwError(e);
			return false;
		}
	}
}