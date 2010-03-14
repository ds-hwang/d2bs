function Baal() {

	this.config = Interface.readConfig("Baal", [
		{Name:"PublicMode", Default:1}, {Name:"KillBaal", Default:true}, {Name:"OpenTPMessage", Default:"TP's Up"}, 
		{Name:"SafeTPMessage", Default:"TP's Safe"}]);
		
	this.filterRoom = function(nUnit) {
		if (nUnit.x >= 15072 && nUnit.y >= 5002 && nUnit.x <= 15118 && nUnit.y <= 5073)
			return true;
		return false;
	}
		
	this.filterCircle = function(nUnit) {
		if (this.filterRoom(nUnit) && getDistance(me, nUnit) < 15)
			return true;
		return false;
	}
	if (this.config.PublicMode > 0)
		Interface.message(Normal, "Running partying script.");
	
	//Move thru waypoint
	if (!Pather.useWaypoint(Area.Act5.TheWorldStoneKeepLevel2))
		return false;
		
	//Precast here.
	Precast.precast();
	
	//Move to next Level
	if (!Pather.moveExit(Area.Act5.TheWorldStoneKeepLevel3, true))
		return false;
		
	//Move to the throne
	if (!Pather.moveExit(Area.Act5.ThroneOfDestruction, true))
		return false;
	
	//Move to the center of throne
	if (!Pather.moveTo({x:15092, y:5008}))
		return false;

	//Make sure Baal is there!
	if (!getUnit(1, "Baal")) {
		Interface.message(Warning, "Baal is not in the throne, skipping.");
		return false;
	}
		
	if (this.config.PublicMode > 0) {
		//Move to corner of Throne
		if (!Pather.moveTo({x:15118, y:5002}))
			return false;
		
		//Cast a TP
		Pather.moveToTown(1);
		
		//Say message about TP Up
		if (this.config.PublicMode == 2)
			say(this.config.OpenTPMessage);

		//Clear any monsters close to TP
		Attack.attackAll(this.filterCircle);
		
		//Say portal is safe
		if (this.config.PublicMode == 2)
			say(this.config.SafeTPMessage);
	}
	
	//Clear the throne room

	while(getUnit(1, "Baal")) {
		if (!Attack.attackAll(this.filterRoom)) {
			Pather.moveTo({x:15098, y:5033});
			Precast.precast();
			Attack.spam();
		}
		Pickit.pick();
		delay(500);
	}
	var superUniques = ["Colenzo the Annihilator", "Achmel the Cursed", "Bartuc the Bloody", "Ventar the Unholy", "Lister the Tormentor"];

	delay(1000);
	return true;
}