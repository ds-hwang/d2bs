/*	Frost Nova Untimed Attack Sequence
 *		Uses Frost Nova ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Frost Nova]
 *		Enable - Set if you want to use Frost Nova at all, default is false.
 *		Enable=false
 */
Attack.untimedAttacks.push({
Name:"Frost Nova",
Skill:Skill.get(Skill.FrostNova),

//Configuration Values
Enabled:Interface.read("Frost Nova", "Enable", true),

//Check if we should use Frost Nova against given monster.
Predicate:function(mon) {
	if (mon.getStat(Stats.ColdResist) >= 75)
		return false;
	return true;
},

//Do it!
Func:function(mon) {
	if (getDistance(me, mon) > 2)
		Pather.moveTo({unit:mon});
	this.Skill.cast(Hand.Left, mon);
}

});