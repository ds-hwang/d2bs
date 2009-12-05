/*	Charged Bolt Untimed Attack Sequence
 *		Uses Charged Bolt ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Charged Bolt]
 *		Enable - Set if you want to use Charged Bolt at all, default is false.
 *		Enable=false
 */
Attack.untimedAttacks.push({
Name:"Charged Bolt",
Skill:Skill.get(Skill.ChargedBolt),

//Configuration Values
Enabled:Interface.read("Charged Bolt", "Enable", true),

//Check if we should use Charged Bolt against given monster.
Predicate:function(mon) {
	if (mon.getStat(Stats.LightningResist) >= 75)
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