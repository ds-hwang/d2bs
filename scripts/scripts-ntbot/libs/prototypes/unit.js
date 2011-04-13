/**
*	@name		unit.js
*
*	@author		alogwe
*
*	@version	1.0
*
*	@desc		Prototype library to extend D2BS 'Unit' Class objects
*				
*				Note: 	Fold syntax on sets of {} for easier viewing 
*						If using notepad++ with JavaScript syntax highlighting:
*							Fold All	: Alt + 0
*							Unfold All	: Alt + Shift + 0
*/


// Class Properties
Unit.prototype.__defineGetter__('inAmuletPosition', function(){ return (this.location == 255 && this.bodylocation == 2); });
Unit.prototype.__defineGetter__('inArmorPosition', function(){ return (this.location == 255 && this.bodylocation == 3); });
Unit.prototype.__defineGetter__('inBelt', function(){ return (this.location == 255 && this.bodylocation == 0); });
Unit.prototype.__defineGetter__('inBeltPosition', function(){ return (this.location == 255 && this.bodylocation == 8); });
Unit.prototype.__defineGetter__('inBootsPosition', function(){ return (this.location == 255 && this.bodylocation == 9); });
Unit.prototype.__defineGetter__('inGlovesPosition', function(){ return (this.location == 255 && this.bodylocation == 10); });
Unit.prototype.__defineGetter__('inHandRightPosition', function(){ return (this.location == 255 && this.bodylocation == 4); });
Unit.prototype.__defineGetter__('inHandRightSwitchPosition', function(){ return (this.location == 255 && this.bodylocation == 11); });
Unit.prototype.__defineGetter__('inHandLeftPosition', function(){ return (this.location == 255 && this.bodylocation == 5); });
Unit.prototype.__defineGetter__('inHandLeftSwitchPosition', function(){ return (this.location == 255 && this.bodylocation == 12); });
Unit.prototype.__defineGetter__('inHelmetPosition', function(){ return (this.location == 255 && this.bodylocation == 1); });
Unit.prototype.__defineGetter__('inInventory', function(){ return (this.location == 0 && this.bodylocation == 0); });
Unit.prototype.__defineGetter__('inRingLeftPosition', function(){ return (this.location == 255 && this.bodylocation == 7); });
Unit.prototype.__defineGetter__('inRingRightPosition', function(){ return (this.location == 255 && this.bodylocation == 6); });
Unit.prototype.__defineGetter__('isAmazon', function(){ return this.classid == 0; });
Unit.prototype.__defineGetter__('isAmazonValkyrie', function(){ return this.classid == 357; });
Unit.prototype.__defineGetter__('isAssassin', function(){ return this.classid == 6; });
Unit.prototype.__defineGetter__('isAssassinShadowMaster', function(){ return this.classid == 418; });
Unit.prototype.__defineGetter__('isAssassinShadowWarrior', function(){ return this.classid == 417; });
Unit.prototype.__defineGetter__('isBarbarian', function(){ return this.classid == 4; });
Unit.prototype.__defineGetter__('isCube', function(){ return this.classid == 549; });
Unit.prototype.__defineGetter__('isDruid', function(){ return this.classid == 5; });
{Unit.prototype.__defineGetter__('isEnchantable', 
	function()
	{ 	
	
		return	(((this.isNecroRevive || this.isNecroSkeleton || this.isNecroGolem || 
					this.isAmazonValkyrie || this.isAssassinShadowWarrior || this.isAssassinShadowMaster || this.isMerc) && (this.parentInParty)) ||
						(this.isPartyMember && this.mode != 17)); 
	}
);}
Unit.prototype.__defineGetter__('isEnchanted', function(){ return this.getState(16); });
Unit.prototype.__defineGetter__('isMerc', function(){ return (this.classid == 271 || this.classid == 338 || this.classid == 359 || this.classid == 560 || this.classid == 561); });
Unit.prototype.__defineGetter__('isNecroGolem', function(){ return (this.classid == 289 || this.classid == 290 || this.classid == 291 || this.classid == 292); });
Unit.prototype.__defineGetter__('isNecromancer', function(){ return this.classid == 2; });
Unit.prototype.__defineGetter__('isNecroRevive', function(){ return this.getState(96); });
Unit.prototype.__defineGetter__('isNecroSkeleton', function(){ return this.classid == 363; });
Unit.prototype.__defineGetter__('isNotEquipped', function(){ return this.bodylocation == 0; });
Unit.prototype.__defineGetter__('isNpc', function(){ return this.type == 1; });
Unit.prototype.__defineGetter__('isPaladin', function(){ return this.classid == 3; });
{Unit.prototype.__defineGetter__('isPartyMember',
	function()
	{
		if(!this.isPlayer)
			return false;
		
		var partyPlayerUnit = getParty();
		if(partyPlayerUnit)
		{
			do
			{	
				if(partyPlayerUnit.name == this.name)
				{
					if(partyPlayerUnit.isPartied)
						return true;
				}
			}while(partyPlayerUnit.getNext())
		}
		
		return false;
	}
);}
Unit.prototype.__defineGetter__('isPlayer', function(){ return this.type == 0; });
Unit.prototype.__defineGetter__('isSorceress', function(){ return this.classid == 1; });
Unit.prototype.__defineGetter__('isStash', function(){ return this.classid == 267; });

{Unit.prototype.__defineGetter__('parentInParty', 
	function()
	{ 
		var parent = this.getParent();
		var partiedPlayer;
		
		if(parent)
		{
			switch(typeof(parent))
			{
				case 'string':
					partiedPlayer = getParty().getUser(parent);
					if(partiedPlayer)
						return (parent == partiedPlayer.name);
				case 'object':
					if(typeof(parent.name) == 'string')
						partiedPlayer = getParty().getUser(parent.name);
					if(partiedPlayer)
						return (parent.name == partiedPlayer.name);
				default:
					// NTC_DebugPrint('parent is typeof() '+typeof(parent));	//alogwe debugging;
					break;
			}
		}
		return false;
	}
);}


// Class Methods

