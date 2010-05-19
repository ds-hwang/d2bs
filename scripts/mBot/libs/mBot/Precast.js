/* Precasts Library
 *	Summary:
 *		Full precasting system, ripped mostly from lord2800's precasting library.
 *	Notes:
 *		After revising mine several times, I still hated it, so I looked back at my old EMB code,
 *		to see what I had done there. Found lord2800's old common libraries, really liked how
 *		he did his precast library, and decided to copy it:P
 *	Functions:
 *		Precast.Init() - Initalizes Precast, so it just calls determinePrecasts
 *		Precast.determinePrecasts() - Determines what precasts we have, which ones are best, fills Precast.precasts array.
 *		Precast.precast() - Actually precasts, depending on if we are in town, has been casted, etc..
 *	Configuration:
 *		[Precast]
 *			ForceRecast - Sets if you want to recast any precasts, regardless of there current state, default is disabled.
 *			ForceRecast=false
 *			(PrecastName)Disable - Sets if you want to use a precast, no spaces with name, default is enabled.
 *			FrozenArmorDisable=true
 *			(PrecastName)Pri - Sets the priority for family related precasts, higher = better, default is based on position in skill tree.
 *			FrozenArmorPri=2
 *	Todo:
 *		@Calculate duration, determine how long precast has until fading, recast if necessary.
 *		@Cache current precast system, have way of determining if we need to re-determine precasts.
 *		@Have a quickcast function, where it would quickly re-cast critcal precasts.
 *	Library Requirements:
 *		mBot.js - For weapon switching.
 *		Interface.js - For debugging and configuration reading.
 *		Skills.js - For casting the skills.
 */
var Precast = new function () {
	this.precasts = [];
	this.config = {};

	/* Precast List
		name - Name of the precast, used for debugging/informative purposes only.
		id - Id of the precast skill.
		town - If precast can be casted in town.
		level - Holds the level of the skill when we determine precasts.
		tab - Holds the tab of which the level is highest when we determine precasts.
		state(optional) - The state it places on the player when casted, used to determine recasting.
		unitid(optional) - The minionCount id to determine the number of precasts we have.
		fam(optional) - The family of precasts, family being there can only be one of theses precasts on at a time.
		pri(optional) - The priority of the familys, determine which one is better, higher = better.
		multi(optional) - Amount of summons we can have, to determine how many we need to cast. Ex Max ravens is 5.
	*/
	this.list = [
		//Barbarian Precasts
		{name:"Battle Command", 	id:155, town:false, level:0, tab:-1, state:51},
		{name:"Battle Orders", 		id:149, town:false, level:0, tab:-1, state:32},
		{name:"Shout", 				id:138, town:false, level:0, tab:-1, state:26},
		//Amazon Precasts
		{name:"Valkyrie", 			id:32,  town:true, level:0, tab:-1, unitid:2},
		//Sorceress Precasts
		{name:"Frozen Armor", 		id:40,  town:true, level:0, tab:-1, state:10, fam:0, pri:0},
		{name:"Shiver Armor",		id:50,  town:true, level:0, tab:-1, state:88, fam:0, pri:1},
		{name:"Chilling Armor",		id:60,  town:true, level:0, tab:-1, state:20, fam:0, pri:2},
		
		{name:"Thunderstorm", 		id:57,  town:false, level:0, tab:-1, state:38},
		{name:"Energy Shield",		id:58,  town:true, level:0, tab:-1, state:30},
		//Necromancer Precasts
		{name:"Bone Armor",			id:68,  town:true, level:0, tab:-1, state:14},
		{name:"Clay Golem",			id:75,  town:true, level:0, tab:-1, unitid:3, fam:1, pri:0},
		{name:"Blood Golem",		id:85,  town:true, level:0, tab:-1, unitid:3, fam:1, pri:1},
		{name:"Fire Golem",			id:94,  town:true, level:0, tab:-1, unitid:3, fam:1, pri:2},
		
		//Paladin Precasts
		{name:"Holy Shield",		id:117, town:true, level:0, tab:-1, state:101},
		//Druid Precasts
		{name:"Raven",				id:221, town:true, level:0, tab:-1, unitid:10, multi:5},
		
		{name:"Poison Creeper",		id:222, town:true, level:0, tab:-1, unitid:14, fam:2, pri:0},
		{name:"Carrion Vine",		id:231, town:true, level:0, tab:-1, unitid:14, fam:2, pri:1},
		{name:"Solar Creeper",		id:241, town:true, level:0, tab:-1, unitid:14, fam:2, pri:2},
		
		{name:"Oak Sage",			id:226, town:true, level:0, tab:-1, unitid:13, fam:3, pri:0},
		{name:"Heart Of Wolverine", id:236, town:true, level:0, tab:-1, unitid:13, fam:3, pri:1},
		{name:"Spirit Of Barbs",	id:246, town:true, level:0, tab:-1, unitid:13, fam:3, pri:2},
		
		{name:"Spirit Wolf",		id:227, town:true, level:0, tab:-1, unitid:11, fam:4, pri:0, multi:5},
		{name:"Dire Wolf",			id:237, town:true, level:0, tab:-1, unitid:12, fam:4, pri:1, multi:3},
		{name:"Grizzly",			id:247, town:true, level:0, tab:-1, unitid:15, fam:4, pri:2},
		
		{name:"Cyclone Armor",		id:235, town:true, level:0, tab:-1, state:151},
		{name:"Hurricane",			id:250, town:false, level:0, tab:-1, state:144, delay:true},
		//Assassin Precasts
		{name:"Burst of Speed",		id:258, town:true, level:0, tab:-1, state:157, fam:5, pri:0},
		{name:"Fade",				id:267, town:true, level:0, tab:-1, state:159, fam:5, pri:1},
		
		{name:"Shadow Warrior",		id:268, town:true, level:0, tab:-1, unitid:16, fam:6, pri:0},
		{name:"Shadow Master", 		id:279, town:true, level:0, tab:-1, unitid:17, fam:6, pri:1}
	];

	this.Init = function () {
		this.determinePrecasts();
		this.config = Interface.readConfig("Precast", [{Name:"ForceRecast", Default:false}]);
	}
	
	/*	this.determinePrecasts()
	 *		Runs through all the available precasts and determines valid ones.
	 */
	this.determinePrecasts = function () {
		var currentTab = weaponSwitch(1);
		var oldTab = currentTab;
		//Check the first tab
		for (var n in this.list) {
			//Read in configuration if someone wants a certain precast disabled
			this.list[n].disable = Interface.read("Precast", this.list[n].name.replace(/\s/gi, "") + "Disable", false);
			//Check if it has a priority, if so, read in configuration
			if (this.list[n].hasOwnProperty("pri"))
				this.list[n].pri = Interface.read("Precast", this.list[n].name.replace(/\s/gi, "") + "Pri", this.list[n].pri);
			//Check if we have the precast!
			if (me.getSkill(this.list[n].id, 1) > 0) {
				this.list[n].level = me.getSkill(this.list[n].id, 1);
				this.list[n].tab = currentTab;
				Interface.message(DetailedDebug, "We've found " + this.list[n].name + " has " + this.list[n].level + " skill points on tab " + this.list[n].tab);
			}
		}
		currentTab = mBot.switchWeapons();
		//Switch tabs and check again!
		for (var n in this.list) {
			//Check if the secondary tab has better levels
			if (me.getSkill(this.list[n].id, 1) > this.list[n].level) {
				this.list[n].level = me.getSkill(this.list[n].id, 1);
				this.list[n].tab = currentTab;
				Interface.message(DetailedDebug, "We've found " + this.list[n].name + " has " + this.list[n].level + " skill points on tab " + this.list[n].tab);
			}
		}
		//Check for skills on the CTA side first
		var ctaTab = (this.list[0].tab == -1) ? 0 : this.list[0].tab;
		//Make sure CTA is always on the secondary tab, never attack/move with it.
		if (ctaTab == currentTab)
			mBot.switchWeapons();
		for (var n in this.list)
			if (this.list[n].level > 0 && this.list[n].tab == ctaTab && !this.list[n].disable)
				this.precasts.push(this.list[n]);
		//Now the other side
		for (var n in this.list)
			if (this.list[n].level > 0 && this.list[n].tab != ctaTab && !this.list[n].disable)
				this.precasts.push(this.list[n]);
				
		//Check the list for duplicate familys
		for (var n in this.precasts) {
			//Make sure the precast has a family
			if (!this.precasts[n].hasOwnProperty("fam"))
				continue;
			//Set the first found family precast
			var highPri = n;
			//Check it versus the other 
			for (var d in this.precasts) {
				if (!this.precasts[d].hasOwnProperty("fam") || this.precasts[n].fam != this.precasts[d].fam || d == n)
					continue;
				//Check if the current highest priority, is the highest!
				if (this.precasts[highPri].pri > this.precasts[d].pri) {
					this.precasts.splice(d, 1);
				} else {
					this.precasts.splice(highPri, 1);
					highPri = d;
				}
			}
			Interface.message(DetailedDebug, "We've found " + this.precasts[highPri].name + " has the highest priority out of the family.");
		}
	}
	
	/* this.precast()
	 *	Runs through all the valid precasts, and casts them if needed.
	 */
	this.precast = function () {
		var oldSwitch = weaponSwitch(1);
		Interface.message(Debug, "We are now running through all given precasts.");
		for (var n in this.precasts) {
		
			//Check if we can even cast this in town.
			if (!this.precasts[n].town && mBot.inTown()) {
				Interface.message(DetailedDebug, "Skipping " + this.precasts[n].name + " since we're in town.");
				continue;
			}
				
			//Determine the maximum amount of times something can be casted
			var maxCount = (this.precasts[n].hasOwnProperty("multi") ? (this.precasts[n].level < this.precasts[n].multi ? this.precasts[n].level : this.precasts[n].multi) : 1);
			
			//Check if we have already casted or summoned.
			if (!this.config.ForceRecast) {
				if (this.precasts[n].hasOwnProperty("state")) {
					//Check if we have already casted
					if (me.getState(this.precasts[n].state)) {
						Interface.message(DetailedDebug, "Skipping " + this.precasts[n].name + " since it's already casted.");
						continue;
					}
				} else if (this.precasts[n].hasOwnProperty("unitid")) {
					//Check if we have the maximum amount of said summon.
					if (me.getMinionCount(this.precasts[n].unitid) == maxCount) {
						Interface.message(DetailedDebug, "Skipping " + this.precasts[n].name + " since we already have them fully summoned.");
						continue;
					}
					//Reduces max count by number of summons already casted.
					maxCount -= me.getMinionCount(this.precasts[n].unitid);
				}
			}
			
			//Switch to proper tab
			mBot.switchWeapons(this.precasts[n].tab);
			
			//Check and wait for complete lockout
			while(mBot.checkMode(Mode.Player.Cast))
				delay(1);
				
			//Obtain the skill class for said preset
			var skill = Skill.get(this.precasts[n].id);
			Interface.message(Debug, "Going to cast " + this.precasts[n].name + " " + maxCount + " time(s).");
			
			for(var n = 0; n < maxCount; n++)
				skill.cast();
		}
		mBot.switchWeapons(oldSwitch);
	}
}