/* Summoner Script
 *	Goes and kills Summoner, has the option to use portal then do Duriel directly after
 */
function Summoner() {
	this.config = Interface.readConfig("Summoner", [{name:"UsePortalToDuriel", default:false}]);
	
	//Move to waypoint then use Arcane waypoint.
	if (!Pather.useWaypoint(Area.Act2.ArcaneSanctuary))
		return false;
		
	//Do your precast thang
	Precast.precast();
	
	//Move to the Summoner Tomb
	if (!Pather.movePreset(2, 357))
		return false;
		
	Attack.attackMonster("The Summoner");
	Pickit.pick();
		
	return true;
}