js_strict(true);
/*	Charged Bolt Untimed Attack Sequence
 *		Uses Charged Bolt ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Charged Bolt]
 *		Enable - Set if you want to use Charged Bolt at all, default is false.
 *		Enable=false
 */
Attack.addAttack({
	Skill:Skills.Get(Skill.ChargedBolt),
	
	//Configuration Values
	//Enabled:Interface.read("Charged Bolt", "Enable", true),
	
	//Check if we should use Charged Bolt against given monster.
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