#pragma once

struct UnitAny;

/**
 * Item Validator
 * Checks a UnitAny to ensure it is a valid item.
 */
bool IsValidItem(UnitAny* item);

/**
 * Get Item Code
 * Returns the items three-letter code.
 */
void GetItemCode(UnitAny* item, char* output, int length);


/** Left Click Inventory Item
 * Left clicks on a inventory item
 */
bool LeftClickInventoryItem(UnitAny* item, bool shift);

/** Right Click Inventory Item
 * Right clicks on a inventory item
 */
bool RightClickInventoryItem(UnitAny* item, bool shift);


/** Left Click Stash Item
 * Left clicks on a stash item
 */
bool LeftClickStashItem(UnitAny* item, bool shift);

/** Right Click Stash Item
 * Right clicks on a stash item
 */
bool RightClickStashItem(UnitAny* item, bool shift);


/** Left Click Cube Item
 * Left clicks on a Cube item
 */
bool LeftClickCubeItem(UnitAny* item, bool shift);

/** Right Click Cube Item
 * Right clicks on a Cube item
 */
bool RightClickCubeItem(UnitAny* item, bool shift);

/** Clicks on Body Item
 * Clicks on an item we are currently wearing
 */
bool ClickBodyItem(UnitAny* item);


/** Body Placement
 * Places item on cursor on given body part.
 */
bool PlaceArmor();
bool PlaceHelm();
bool PlaceGloves();
bool PlaceBoots();
bool PlaceLeftRing();
bool PlaceRightRing();
bool PlaceAmulet();
bool PlaceBelt() ;
bool PlaceLeftPrimary();
bool PlaceRightPrimary();
bool PlaceLeftSecondary();
bool PlaceRightSecondary();

bool PlaceMercHelm();
bool PlaceMercArmor();
bool PlaceMercLeft();
bool PlaceMercRight();