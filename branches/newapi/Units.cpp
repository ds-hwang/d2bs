#include "Structs.h"
#include "Functions.h"
#include "Helpers.h"
#include "D2Utilities.h"

#include "Units.h"

#include "SyncLock.h"

#include "Script.h"

#include "js32.h"

#include "debug_new.h"

/*Player::IsPartied ()
 *	Checks if the player is partied with us.
 */
BOOL Player::IsPartied () {
	if (!IsValid())//Make sure the player is valid
		return -1;

	//Get the player roster for yourself
	RosterUnit* pMe = FindPlayerRoster(p_D2CLIENT_PlayerUnit->dwUnitId);

	//Check if we are both in the same party and we both arent not partied.
	if (pMe->wPartyId == pRoster->wPartyId && (!(pMe->wPartyId == 0xFFFF)))
		return true;
	return false;
}

/*Player::IsHostile()
 *	Checks if we are hostile with the player/
 */
BOOL Player::IsHostile () {
	if (!IsValid())//Make sure we have valid units
		return -1;

	//Check the pvp flag to see if we're hostile
	if (TestPvpFlag(p_D2CLIENT_PlayerUnit->dwUnitId, pUnit->dwUnitId, 8))
		return true;
	return false;
}

/*Player::IsInParty ()
 *	Checks if the player is in a party, might not be the same party as you.
 */
BOOL Player::IsInParty () {
	if (!IsValid())//Make sure we have valid units
		return -1;

	//Check if they are in a party
	if (pRoster->dwPartyFlags == 0x01)
		return true;
	return false;
}
/*Player::IsInvited ()
 *	Check if this player has been invited to a party.
 */
BOOL Player::IsInvited () {
	if (!IsValid())//Make sure we have valid units
		return -1;

	//Check if he has been invited
	if (pRoster->dwPartyFlags == 0x02 || pRoster->dwPartyFlags == 0x04)
		return true;
	return false;
}