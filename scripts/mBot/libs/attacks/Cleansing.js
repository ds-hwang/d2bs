Attack.postAttacks.push({
Name:"Cleansing",
Skill:Skill.get(Skill.Cleansing),

//Configuration Values
Enabled:Interface.read("Cleansing", "Enable", true),

//Check if we are poisoned and need to cleanse
Predicate:function() {
	if (me.getState(State.Poison))
		return true;
	return false;
},

//Do it!
Func:function(mon) {
	if (getDistance(me, mon) > 5)
		Pather.moveTo({unit:mon, pop:true});
	this.Skill.cast(Hand.Left, mon);
}

});