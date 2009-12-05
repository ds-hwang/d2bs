/*	Chain Lightning Untimed Attack Sequence
 *		Uses Chain Lightning ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Chain Lightning]
 *		Enable - Set if you want to use Chain Lightning at all, default is false.
 *		Enable=false
 */
Attack.untimedAttacks.push({
Name:"Chain Lightning",
Skill:Skill.get(Skill.ChainLightning),

//Configuration Values
Enabled:Interface.read("Chain Lightning", "Enable", true),

//Check if we should use Chain Lightning against given monster.
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