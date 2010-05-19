/* Mephisto Sequence
 *	Summary:
 *		Goes to Durance of Hate Level 3, moats Meph if wanted, kills Mephisto, opens uber chest if wanted, kills council if wanted, then goes through act 4 portal if wanted.
 *	Configuration:
 *		[Mephisto]
 *			Enabled - Boolean that is set if you want to run the Mephisto sequence.
 *			Enabled = true
 *			UseMoatTrick - Set if you want the bot to trap Mephisto in the moat before killing him.
 *			UseMoatTrick = false
 *			OpenSpecialChest - Set if you want to go open the special chest at the top of the map.
 *			OpenSpecialChest = true
 *			KillCouncil - Set if you want to kill the council members in the level.
 *			KillCouncil = false
 *			UseRedPortal - Set if you want to leave Durance through the Red Portal
 *			UseRedPortal = false
 */
function Mephisto() {
	//Read in configuration values for Mephisto Sequence
	this.config = Interface.readConfig("Mephisto", [
			{Name:"UseMoatTrick", Default:false}, {Name:"OpenSpecialChest", Default:true},
			{Name:"KillCouncil", Default:false}, {Name:"UseRedPortal", Default:false}]);
			
	//Move through waypoint into Durance of Hate Level 2
	if (!Pather.useWaypoint(Area.Act3.DuranceOfHateLevel2))
		return false;
		
	//Precast everything available
	Precast.precast();
	
	//Move to Durance of Hate Level 3
	if (!Pather.moveExit(Area.Act3.DuranceOfHateLevel3, true))
		return false;
	
	//Move close enough to get unit lock.
	if (!Pather.moveTo({x:17584, y:8078}))
		return false;
	
	//Check if we are to trap Mephisto in the moat.
	if (this.config.UseMoatTrick) {
		Pather.moveTo({x:17585, y:8067});
		Pather.moveTo({x:17560, y:8067});
		delay(500);
		Pather.moveTo({x:17563, y:8072});
		delay(500);
		Pather.moveTo({x:17585, y:8083});
		delay(500);
		Pather.moveTo({x:17611, y:8087});
		Interface.message(Debug, "Mephisto is now in the moat.");
	}
	
	//Pick Meph!
	Attack.attackMonster("Mephisto");
	Pickit.pick();
	
	//Open special chest if wanted
	if (this.config.OpenSpecialChest) {
		//Move to chest area
		Pather.moveTo({x:17512, y:8065});
		
		//Obtain proper uber chest
		var chest = mBot.findUnits(2, function(chest) { return chest.x == 17513 && chest.y == 8063; });
			
		if (chest.length > 0) {
		
			//Open chest, and wait for goodies
			chest[0].interact();
			delay(500);
			
			//Get the goods
			Pickit.pick();
		}
	}
	
	//Kill council if wanted
	if (this.config.KillCouncil) {
		var councilLocations = [{x:17603, y: 8127}, {x:17617, y: 8074}, {x:17601, y: 8015}];
		
		for (var n in councilLocations) {
			//Move to one of the three locations(left, center, right).
			if (!Pather.moveTo(councilLocations[n]))
				continue;
			
			//Simply attack only council members.
			if (!Attack.attackAll(function(mon) { return mon.classid >= 345 && mon.classid <= 347; }))
				continue;
			
			//Pick dat shit
			Pickit.pick();
		}
	}
	
	//Move through red portal if wanted
	if (this.config.UseRedPortal) {
		if (!Pather.moveTo({x:17601, y:8070}))
			return false;
		
		//Find the big red portal
		var portal = getUnit(2, 342);
		
		if (!portal)
			throw new Error("Unable to find Red Portal to Act 4!");
			
		if (!Pather.moveThru(portal))
			return false;
	}
	
	return true;
}