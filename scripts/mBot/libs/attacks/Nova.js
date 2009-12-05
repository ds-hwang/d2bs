/*	Nova Untimed Attack Sequence
 *		Uses Nova ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Nova]
 *		Enable - Set if you want to use Nova at all, default is false.
 *		Enable=false
 */
Attack.untimedAttacks.push({
Name:"Nova",
Skill:Skill.get(Skill.Nova),

//Configuration Values
Enabled:Interface.read("Nova", "Enable", true),

//Check if we should use Nova against given monster.
Predicate:function(mon) {
	if (mon.getStat(Stats.LightningResist) >= 75)
		return false;
	return true;
},

//Do it!
Func:function(mon) {
	if (getDistance(me, mon) > 3)
		Pather.moveTo({unit:mon});
	this.Skill.cast(Hand.Left, mon);
}

});