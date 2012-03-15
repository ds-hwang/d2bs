js_strict(true);
/*	Lightning Untimed Attack Sequence
 *		Uses Lightning ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Lightning]
 *		Enable - Set if you want to use Lightning at all, default is false.
 *		Enable=false
 */
Attack.addAttack({
	Skill:Skills.Get(Skill.Lightning),
	
	//Configuration Values
	//Enabled:Interface.read("Lightning", "Enable", true),
	
	//Check if we should use Lightning against given monster.
	Predicate:function(mon) {
		if (mon.getStat(Stats.LightningResist) >= 100)
			return false;
		return true;
	},
	
	//Do it!
	Func:function(mon) {
		Pather.MoveToRange(mon, mon.area, this.Skill.castRange);
		this.Skill.Cast(mon);
	}
});