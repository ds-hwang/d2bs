js_strict(true);

Attack.addAttack({
	Skill:Skills.Get(Skill.BlessedHammer),
	Conc:Skills.Get(Skill.Concentration),
	
	//Configuration Values
	//Enabled:Interface.read("Blessed Hammer", "Enable", true),
	
	//Check if we the monster has a magic immunity.
	Predicate:function(mon) {
		//if (mon.getStat(Stats.MagicResist) >= 75)
		//	return false;
		return true;
	},
	
	//Attack Dependant Variables
	LastGID:0, LastHP:0, Attempts:0,
	
	//Do it!
	Func:function(mon) {
		//Check if it is a new monster, if so, reset vars
		if (mon.gid != this.LastGID) {
			this.Attempts = 0;
			this.LastHP = 0;
			this.LastGID = mon.gid;
			Pather.MoveToPoint(mon);
		}
		if (this.LastHP == mon.hp)
			this.Attempts++;
		if (this.Attempts >= 5) {
			Pather.MoveToPoint(mon);
			this.Attempts = 0;
		}
		this.LastHP = mon.hp;
		this.Conc.setSkill();
		this.Skill.Cast(mon);
	}
});
/*
Attack.spamAttacks.push({
	Name:"Blessed Hammer",
	Skill:Skills.Get(Skill.BlessedHammer),
	Conc:Skills.Get(Skill.Concentration),

	//Configuration Values
	Enabled:Interface.read("Blessed Hammer", "SpamEnable", true),
	
	Func:function() {
		this.Conc.setSkill();
		this.Skill.cast(Hand.Left);
	}
});
//*/