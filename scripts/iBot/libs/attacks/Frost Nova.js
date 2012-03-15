js_strict(true);
/*	Frost Nova Untimed Attack Sequence
 *		Uses Frost Nova ethier as the only skill, or to complement a timed attack.
 *	Configuration:
 *		[Frost Nova]
 *		Enable - Set if you want to use Frost Nova at all, default is false.
 *		Enable=false
 */
Attack.addAttack({
	Skill:Skills.Get(Skill.FrostNova),
	
	//Configuration Values
	//Enabled:Interface.read("Frost Nova", "Enable", true),
	
	//Check if we should use Frost Nova against given monster.
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