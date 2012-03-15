js_strict(true);
/*	Ice Blast Untimed Attack Sequence
 *		Uses Ice Blast ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Ice Blast]
 *		Enable - Set if you want to use Ice Blast at all, default is false.
 *		Enable=false
 */
Attack.addAttack({
	Skill:Skills.Get(Skill.IceBlast),
	
	//Configuration Values
	//Enabled:Interface.read("Ice Blast", "Enable", true),
	
	//Check if we should use Ice Blast against given monster.
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