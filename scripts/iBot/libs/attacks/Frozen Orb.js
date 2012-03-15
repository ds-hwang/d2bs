js_strict(true);
/*	Frozen Orb Timed Attack Sequence
 *		Frozen Orbs a monster every 1.8 seconds
 *	Configuration:
 *		[Frozen Orb]
 *		Enable - Set if you want to use Frozen Orb at all, default is false.
 *		Enable=false
 */
Attack.addAttack({
	Skill:Skills.Get(Skill.FrozenOrb),
	
	//Configuration Values
	//Enabled:Interface.read("Frozen Orb", "Enable", true),
	
	//Check if we should use Frozen Orb against given monster.
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