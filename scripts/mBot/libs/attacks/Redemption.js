/* Redemption Post Attack Sequence
 *	Regain some health from dead monsters.
 */
Attack.postAttacks.push({
Name:"Redemption", 
Skill:Skill.get(Skill.Redemption),

//Configuration values.
Enabled:Interface.read("Redemption", "Enable", true),
Timeout:Interface.read("Redemption", "Timeout", 2500),
BodyCount:Interface.read("Redemption", "BodyCount", 4),
ChampionOverride:Interface.read("Redemption", "ChampionOverride", false),

//Predicate function to determine if the monster is a body we can redeem
FindRedeemableBodies:function(mon) {
	return	((mon.mode == 0 || mon.mode == 12) && //Check if the monster is dead
			!me.getState(State.Redeemed) && //Check if the monster hasn't been redeemed yet
			getDistance(me, mon) < 10.6) //Make sure the monster is within Redemption range
},

//Check if we need to use Redemption at all
Predicate:function(mon) {
	var nLifePercent = Math.round((me.getStat(Stats.Life) / me.getStat(Stats.MaxLife)) * 100);

	//If we are at full health, then no use to redempt.
	if (nLifePercent == 100)
		return false;
	
	//Find all non-redeemed bodies
	var bodies = mBot.findUnits(1, this.FindRedeemableBodies);
	
	//Check if there is any bodies to redempt from
	if (bodies.length == 0)
		return false;
		
	print("Found " + bodies.length + " bodies");
		
	//Check if the monster we killed was special, if so redempt.
	if (this.ChampionOverride && (mon.spectype & 1 || mon.spectype & 2 || mon.spectype & 4))
		return true;
		
	print(bodies.length  + " > " + this.BodyCount);
		
	//If there is enough bodies on the floor, clean up!
	if (bodies.length > this.BodyCount);
		return true;
		
	print("DO NOT REDEEM");

	return false;
},

//Redempt!
Func:function(mon) {
	//Set Redemption on the right hand.
	this.Skill.setSkill(Hand.Right);
	
	//Timeout check
	var nDelay = getTickCount();
	while((getTickCount() - nDelay) < this.Timeout) {
		//If there are no dead units that have not been Redempted then return!
		if (mBot.findUnits(1, this.FindRedeemableBodies).length == 0)
			return true;
		delay(100);
	}
	return true;
}
});