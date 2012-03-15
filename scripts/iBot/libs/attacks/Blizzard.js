js_strict(true);
/*	Blizzard Timed Attack Sequence
 *		Blizzards a monster every 1.8 seconds
 *	Configuration:
 *		[Blizzard]
 *		Enable - Set if you want to use blizzard at all, default is false.
 *		Enable=false
 */
Attack.addAttack({
	Skill:Skills.Get(Skill.Blizzard),
	
	//Configuration Values
	//Enabled:Interface.read("Blizzard", "Enable", true),
	
	//Check if we should use Blizzard against given monster.
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