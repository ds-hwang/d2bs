/*	Ice Bolt Untimed Attack Sequence
 *		Uses Ice Bolt ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Ice Bolt]
 *		Enable - Set if you want to use Ice Bolt at all, default is false.
 *		Enable=false
 */
Attack.untimedAttacks.push({
Name:"Ice Bolt",
Skill:Skill.get(Skill.IceBolt),

//Configuration Values
Enabled:Interface.read("Ice Bolt", "Enable", true),

//Check if we should use Ice Bolt against given monster.
Predicate:function(mon) {
	if (mon.getStat(Stats.ColdResist) >= 75)
		return false;
	return true;
},

//Do it!
Func:function(mon) {
	if (getDistance(me, mon) > 5)
		Pather.moveTo({unit:mon, pop:true});
	this.Skill.cast(Hand.Left, mon);
}

});