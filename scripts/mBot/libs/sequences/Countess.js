/* Countess Sequence
 *	Summary:
 *		Goes to Tower Cellar Level 5 and kills Countess
 *	Configuration:
 *		[Countess]
 *			Enabled - Boolean that is set if you want to run the Countess sequence.
 *			Enabled = true
 */
function Countess() {
		
	//Move through waypoint into Black Marsh
	if (!Pather.useWaypoint(Area.Act1.BlackMarsh))
		return false;
		
	//Precast everything available
	Precast.precast();
	
	//Move to Forgotten Tower
	if (!Pather.moveExit(Area.Act1.ForgottenTower, true))
		return false;
		
	//Move through the tower.
	if (!Pather.moveExit(Area.Act1.TowerCellarLevel1, true))
		return false;
	if (!Pather.moveExit(Area.Act1.TowerCellarLevel2, true))
		return false;
	if (!Pather.moveExit(Area.Act1.TowerCellarLevel3, true))
		return false;
	if (!Pather.moveExit(Area.Act1.TowerCellarLevel4, true))
		return false;
	if (!Pather.moveExit(Area.Act1.TowerCellarLevel5, true))
		return false;
		
	//Move to the chest near Countess
	if (!Pather.movePreset(2, 580))
		return false;
		
	Attack.attackMonster("The Countess");
	Pickit.pick();
	
	return true;
}