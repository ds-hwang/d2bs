js_strict(true);

Attack.addAttack({
	Skill:Skills.Get(Skill.Cleansing),
	Mode:Attack.Mode.Post,
	
	//Configuration Values
	//Enabled:Interface.read("Cleansing", "Enable", true),
	
	//Check if we are poisoned and need to cleanse
	Predicate:function() {
		if (me.getState(State.Poison))
			return true;
		return false;
	},
	
	//Do it!
	Func:function(mon) {
		this.Skill.setSkill();
	}
});