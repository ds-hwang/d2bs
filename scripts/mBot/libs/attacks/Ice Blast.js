/*	Ice Blast Untimed Attack Sequence
 *		Uses Ice Blast ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Ice Blast]
 *		Enable - Set if you want to use Ice Blast at all, default is false.
 *		Enable=false
 */
Attack.untimedAttacks.push({
Name:"Ice Blast",
Skill:Skill.get(Skill.IceBlast),

//Configuration Values
Enabled:Interface.read("Ice Blast", "Enable", true),

//Check if we should use Ice Blast against given monster.
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