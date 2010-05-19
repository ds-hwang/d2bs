/* Andariel Sequence
 *	As basic as it gets for andy.
 */
function Andariel() {
	if (!Pather.useWaypoint(Area.Act1.CatacombsLevel2))
		return false;
		
	Precast.precast();
	
	Pather.moveExit(Area.Act1.CatacombsLevel3, true);
	Pather.moveExit(Area.Act1.CatacombsLevel4, true);
	
	delay(500);

	Attack.attackMonster("Andariel");
	Pickit.pick();
	
	return true;
}