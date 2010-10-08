#include "Item.h"
#include "D2Structs.h"
#include "D2Ptrs.h"
#include "D2Handlers.h"
#include "Constants.h"
#include "Stubs.h"

bool IsValidItem(UnitAny* item) {
	return item && item->dwType == UNIT_ITEM && item->pItemData && item->pPath;
}

void GetItemCode(UnitAny* item, char* output, int length) {
	if (!IsValidItem(item))
		return;
	ItemText* txt = D2COMMON_GetItemText(item->dwTxtFileNo);
	strcpy_s(output, length, txt->szCode);
	output[3] = 0x00;
}

/**
 * Internal function used to simiplify click functions.
 */
void ClickStorage(UnitAny* item, InventoryLayout* layout, bool shift, bool rightClick) {
	//Determine the screen coordinates of the top-left of the item.
	int x = layout->Left + item->pItemPath->dwPosX * layout->SlotPixelWidth + 10;
	int y = layout->Top + item->pItemPath->dwPosY * layout->SlotPixelHeight + 10;

	if (rightClick)
		D2CLIENT_RightClickItem(x, y, item->pItemData->ItemLocation, D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory);
	else
		D2CLIENT_LeftClickItem(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, x, y, shift, layout, item->pItemData->ItemLocation);
}

bool LeftClickInventoryItem(UnitAny* item, bool shift) {
	if (!IsValidItem(item))
		return false;

	//Do a sanity check to ensure we are clicking an inventory item.
	if (item->dwMode != ITEM_MODE_INV_STASH_CUBE_STORE || item->pItemData->ItemLocation != STORAGE_INVENTORY)
		return false;

	ClickStorage(item, (*p_D2CLIENT_InventoryLayout), shift, false);
	return true;
}

bool RightClickInventoryItem(UnitAny* item, bool shift) {
	if (!IsValidItem(item))
		return false;

	//Do a sanity check to ensure we are clicking an inventory item.
	if (item->dwMode != ITEM_MODE_INV_STASH_CUBE_STORE || item->pItemData->ItemLocation != STORAGE_INVENTORY)
		return false;

	ClickStorage(item, (*p_D2CLIENT_InventoryLayout), shift, true);
	return true;
}

bool LeftClickStashItem(UnitAny* item, bool shift) {
	if (!IsValidItem(item))
		return false;

	//Do a sanity check to ensure we are clicking an stash item.
	if (item->dwMode != ITEM_MODE_INV_STASH_CUBE_STORE || item->pItemData->ItemLocation != STORAGE_STASH)
		return false;

	ClickStorage(item, (*p_D2CLIENT_StashLayout), shift, false);
	return true;
}

bool RightClickStashItem(UnitAny* item, bool shift) {
	if (!IsValidItem(item))
		return false;

	//Do a sanity check to ensure we are clicking an stash item.
	if (item->dwMode != ITEM_MODE_INV_STASH_CUBE_STORE || item->pItemData->ItemLocation != STORAGE_STASH)
		return false;

	ClickStorage(item, (*p_D2CLIENT_StashLayout), shift, true);
	return true;
}

bool LeftClickCubeItem(UnitAny* item, bool shift) {
	if (!IsValidItem(item))
		return false;

	//Do a sanity check to ensure we are clicking an cube item.
	if (item->dwMode != ITEM_MODE_INV_STASH_CUBE_STORE || item->pItemData->ItemLocation != STORAGE_CUBE)
		return false;

	ClickStorage(item, (*p_D2CLIENT_CubeLayout), shift, false);
	return true;
}

bool RightClickCubeItem(UnitAny* item, bool shift) {
	if (!IsValidItem(item))
		return false;

	//Do a sanity check to ensure we are clicking an cube item.
	if (item->dwMode != ITEM_MODE_INV_STASH_CUBE_STORE || item->pItemData->ItemLocation != STORAGE_CUBE)
		return false;

	ClickStorage(item, (*p_D2CLIENT_CubeLayout), shift, true);
	return true;
}

bool ClickBodyItem(UnitAny* item) {
	if (!IsValidItem(item))
		return false;

	//Sanity check to ensure we are wearing given item
	if (item->dwMode != ITEM_MODE_EQUIPPED || item->pItemData->BodyLocation == 0)
		return false;
	
	//Define a function definition, then do some magic to get the function offset.
	typedef void __fastcall clickequip(UnitAny * pPlayer, Inventory * pIventory, INT loc);
	clickequip * click = (clickequip*)*(DWORD*)(D2CLIENT_BodyClickTable + (4 * item->pItemData->BodyLocation));

	//Ensure the magic worked.
	if (!click)
		return false;

	click(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, item->pItemData->BodyLocation);
	return true;
}

/**
 * Internal function, places item from cursor to body location.
 */
bool PlaceBody(unsigned int bodyLoc) {
	if (!D2CLIENT_GetCursorItem())
		return false;
	if (bodyLoc < 1 || bodyLoc > 12)
		return false;

	//Define a function definition, then do some magic to get the function offset.
	typedef void __fastcall clickequip(UnitAny * pPlayer, Inventory * pIventory, INT loc);
	clickequip * click = (clickequip*)*(DWORD*)(D2CLIENT_BodyClickTable + (4 * bodyLoc));

	//Ensure the magic worked.
	if (!click)
		return false;

	click(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, bodyLoc);
	return true;
}

bool PlaceArmor() { return PlaceBody(EQUIP_BODY); };
bool PlaceHelm() { return PlaceBody(EQUIP_HEAD); };
bool PlaceGloves() { return PlaceBody(EQUIP_GLOVES); };
bool PlaceBoots() { return PlaceBody(EQUIP_FEET); };
bool PlaceLeftRing() { return PlaceBody(EQUIP_LEFT_RING); };
bool PlaceRightRing() { return PlaceBody(EQUIP_RIGHT_RING); };
bool PlaceAmulet() { return PlaceBody(EQUIP_AMULET); };
bool PlaceBelt() { return PlaceBody(EQUIP_BELT); };
bool PlaceLeftPrimary() { return PlaceBody(EQUIP_LEFT_PRIMARY); };
bool PlaceRightPrimary() { return PlaceBody(EQUIP_RIGHT_PRIMARY); };
bool PlaceLeftSecondary() { return PlaceBody(EQUIP_LEFT_SECONDARY); };
bool PlaceRightSecondary() { return PlaceBody(EQUIP_RIGHT_SECONDARY); };

bool PlaceMerc(unsigned int bodyLoc) {
	UnitAny* merc = D2CLIENT_GetMercUnit();

	if (!merc)
		return false;

	D2CLIENT_MercItemAction(0x61, bodyLoc);
	return true;
}

bool PlaceMercHelm() { return PlaceMerc(-1); };
bool PlaceMercArmor() { return PlaceMerc(-1); };
bool PlaceMercLeft() { return PlaceMerc(-1); };
bool PlaceMercRight() { return PlaceMerc(-1); };