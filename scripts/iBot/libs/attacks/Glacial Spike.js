js_strict(true);
/*	Glacial Spike Untimed Attack Sequence
 *		Uses Glacial Spike ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Glacial Spike]
 *		Enable - Set if you want to use Glacial Spike at all, default is false.
 *		Enable=false
 */
Attack.addAttack({
	Skill:Skills.Get(Skill.GlacialSpike),
	
	//Configuration Values
	//Enabled:Interface.read("Glacial Spike", "Enable", true),
	
	//Check if we should use Glacial Spike against given monster.
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