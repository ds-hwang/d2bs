/*	Frozen Orb Timed Attack Sequence
 *		Frozen Orbs a monster every 1.8 seconds
 *	Configuration:
 *		[Frozen Orb]
 *		Enable - Set if you want to use Frozen Orb at all, default is false.
 *		Enable=false
 */
Attack.timedAttacks.push({
Name:"Frozen Orb",
Skill:Skill.get(Skill.FrozenOrb),
Delay:1000,

//Configuration Values
Enabled:Interface.read("Frozen Orb", "Enable", true),

//Check if we should use Frozen Orb against given monster.
Predicate:function(mon) {
	if (mon.getStat(Stats.ColdResist) >= 75)
		return false;
	return true;
},

//Do it!
Func:function(mon) {
	if (getDistance(me, mon) > 5)
		Pather.moveTo({unit:mon, pop:true});
	this.Skill.cast(Hand.Right, mon);
}

});