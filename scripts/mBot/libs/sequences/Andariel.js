function Andariel() {
	//Move thru waypoint
	if (!Pather.useWaypoint(Area.Act1.CatacombsLevel2))
		return false;
		
	Precast.precast();
	
	Pather.moveExit(Area.Act1.CatacombsLevel3, true);
	Pather.moveExit(Area.Act1.CatacombsLevel4, true);
	

	Attack.attackMonster("Andariel");
	
	return true;
}