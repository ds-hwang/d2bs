js_strict(true);
/*	Ice Bolt Untimed Attack Sequence
 *		Uses Ice Bolt ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Ice Bolt]
 *		Enable - Set if you want to use Ice Bolt at all, default is false.
 *		Enable=false
 */
Attack.addAttack({
	Skill:Skills.Get(Skill.IceBolt),
	
	//Configuration Values
	//Enabled:Interface.read("Ice Bolt", "Enable", true),
	
	//Check if we should use Ice Bolt against given monster.
	Predicate:function(mon) {
		if (mon.getStat(Stats.ColdResist) >= 100)
			return false;
		return true;
	},
	
	//Do it!
	Func:function(mon) {
		Pather.MoveToRange(mon, mon.area, this.Skill.castRange);
		this.Skill.Cast(mon);
	}
});