js_strict(true);
/*	Chain Lightning Untimed Attack Sequence
 *		Uses Chain Lightning ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Chain Lightning]
 *		Enable - Set if you want to use Chain Lightning at all, default is false.
 *		Enable=false
 */
Attack.addAttack({
	Skill:Skills.Get(Skill.ChainLightning),
	
	//Configuration Values
	//Enabled:Interface.read("Chain Lightning", "Enable", true),
	
	//Check if we should use Chain Lightning against given monster.
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