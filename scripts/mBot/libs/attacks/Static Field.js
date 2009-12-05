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
Attack.preAttacks.push({
Name:"Static Field", 
Skill:Skill.get(Skill.StaticField),

//Configuration values.
Enabled:Interface.read("Static Field", "Enable", false),
SuperUniquesOnly:Interface.read("Static Field", "SuperUniquesOnly", true),
MaxStatics:Interface.read("Static Field", "MaxStatics", 4),

//Check if we want to use Static against said monster.
Predicate:function(mon) {
	if (mon.uniqueid == -1 && this.SuperUniquesOnly)
		return false;
	return true;
},

//Actually static's said monster
Func:function(mon) {
	//Move within range of monster.
	var staticDistance = 1.0 + 0.668 * Skill.level;
	if (getDistance(me, mon) > staticDistance)
		Pather.moveTo({unit:mon, range:staticDistance, distance:staticDistance + 1});
	
	//Check within Range
	var maxLife = [0,43,65];
	for (var n = 0; n < MaxStatics || mon.life <= maxLife[me.diff]; n++)
		this.Skill.cast(Hand.Right, mon);
}
});