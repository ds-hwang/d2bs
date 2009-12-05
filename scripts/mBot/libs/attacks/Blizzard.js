/*	Blizzard Timed Attack Sequence
 *		Blizzards a monster every 1.8 seconds
 *	Configuration:
 *		[Blizzard]
 *		Enable - Set if you want to use blizzard at all, default is false.
 *		Enable=false
 */
Attack.timedAttacks.push({
Name:"Blizzard",
Skill:Skill.get(Skill.Blizzard),
Delay:1800,

//Configuration Values
Enabled:Interface.read("Blizzard", "Enable", true),

//Check if we should use Blizzard against given monster.
Predicate:function(mon) {
	if (mon.getStat(Stats.ColdResist) >= 75)
		return false;
	return true;
},

//Do it!
Func:function(mon) {
	if (getDistance(me, mon) > 5)
		Pather.moveTo({unit:mon, pop:true});
	this.Skill.cast(Hand.Right, mon);
}

});