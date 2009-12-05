/*	Glacial Spike Untimed Attack Sequence
 *		Uses Glacial Spike ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Glacial Spike]
 *		Enable - Set if you want to use Glacial Spike at all, default is false.
 *		Enable=false
 */
Attack.untimedAttacks.push({
Name:"Glacial Spike",
Skill:Skill.get(Skill.GlacialSpike),

//Configuration Values
Enabled:Interface.read("Glacial Spike", "Enable", true),

//Check if we should use Glacial Spike against given monster.
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