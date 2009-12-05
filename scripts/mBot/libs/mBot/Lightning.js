/*	Lightning Untimed Attack Sequence
 *		Uses Lightning ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Lightning]
 *		Enable - Set if you want to use Lightning at all, default is false.
 *		Enable=false
 */
Attack.untimedAttacks.push({
Name:"Lightning",
Skill:Skill.get(Skill.Lightning),

//Configuration Values
Enabled:Interface.read(Name, "Enable", true),

//Check if we should use Lightning against given monster.
Predicate:function(mon) {
	if (mon.getStat(Stats.LightningResist) >= 75)
		return false;
	return true;
},

//Do it!
Func:function(mon) {
	this.Skill.cast(Hand.Left, mon);
}

});