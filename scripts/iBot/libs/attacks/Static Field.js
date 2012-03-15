js_strict(true);
/*	Static Field Pre-Attack Sequence
 *		Statics a monster to a given life percentage.
 *	Configuration:
 *		[Static Field]
 *		Enable - Set if you want to use static field at all, default is false.
 *		Enable=false
 *		SuperUniquesOnly - Set if you want to static only super-uniques, default is true.
 *		SuperUniquesOnly=true
 *		LifePercent - Life Percent you want to push
 */
Attack.addAttack({
	Skill:Skills.Get(Skill.StaticField),
	Mode:Attack.Mode.Pre,
	
	//Configuration values.
	//Enabled:Interface.Config("Static Field", "Enable", false),
	SuperUniquesOnly:Interface.Config('StaticField','SuperUniquesOnly',false),
	MaxStatics:Interface.Config('StaticField','MaxStatics',4),
	
	//Check if we want to use Static against said monster.
	Predicate:function(mon) {
		if (mon.uniqueid == -1 && this.SuperUniquesOnly)
			return false;
		return true;
	},
	
	//Actually static's said monster
	Func:function(mon) {
		//Move within range of monster.
		var staticDistance = 1.0 + 0.668 * this.Skill.level;
		
		//Check within Range
		var maxLife = [0,43,65];
		for(let i=0;i<this.MaxStatics && mon.hp > maxLife[me.diff];i++) {
			Pather.MoveToRange(mon, mon.area, staticDistance);
			this.Skill.Cast(mon);
		}
	}
});