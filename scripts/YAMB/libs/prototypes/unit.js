/**
*	@name		unit.js
*
*	@author		alogwe
*
*	@version	1.1
*
*	@desc		Prototype library to extend D2BS 'Unit' Class objects
*				
*				NOTE: 	Fold syntax on sets of {} for easier viewing 
*						If using notepad++ with JavaScript syntax highlighting:
*							Fold All	: Alt + 0
*							Unfold All	: Alt + Shift + 0
*/

/*	Index

Unit.bodyLocationName
Unit.inAmuletPosition
Unit.inArmorPosition
Unit.inBelt
Unit.inBeltPosition
Unit.inBootsPosition
Unit.inGlovesPosition
Unit.inHandRightPosition
Unit.inHandRightSwitchPosition
Unit.inHandLeftPosition
Unit.inHandLeftSwitchPosition
Unit.inHelmetPosition
Unit.inInventory
Unit.inRingLeftPosition
Unit.inRingRightPosition
Unit.inTown
Unit.isAmazon
Unit.isAmazonValkyrie
Unit.isAssassin
Unit.isAssassinShadowMaster
Unit.isAssassinShadowWarrior
Unit.isBarbarian
Unit.isCube
Unit.isDead
Unit.isDruid
Unit.isEnchantable
Unit.isEnchanted
Unit.isEquipped
Unit.isMerc
Unit.isMercItem
Unit.isNecroGolem
Unit.isNecromancer
Unit.isNecroRevive
Unit.isNecroSkeleton
Unit.isNotEquipped
Unit.isNpc
Unit.isPaladin
Unit.isPartyMember
Unit.isPlayer
Unit.isSorceress
Unit.isStash
Unit.parentInParty





Unit.getSkillCharges()


*/


// Class Properties

Unit.prototype.__defineGetter__('bodyLocationName',
		function()
		{
			if(this.isEquipped)
			{
				switch(this.bodylocation)
				{
					case 1:		return 'Helmet';
					case 2:		return 'Amulet';
					case 3:		return 'Armor';
					
					case 4:	
						if(me.weaponswitch == 0) return 'RightHandOne';
						else return 'RightHandTwo';
						
					case 5:
						if(me.weaponswitch == 0) return 'LeftHandOne';
						else return 'LeftHandTwo';
						
					case 6:		return 'RightRing';
					case 7:		return 'LeftRing';
					case 8:		return 'Belt';
					case 9:		return 'Boots';
					case 10:	return 'Gloves';
					
					case 11:
						if(me.weaponswitch == 0) return 'RightHandTwo';
						else return 'RightHandOne';
						
					case 12:
						if(me.weaponswitch == 0) return 'LeftHandTwo';
						else return 'LeftHandOne';
				}
			}
			
			print('ÿc1Error: Unit.getBodyLocationName() the Unit is not in a valid bodylocation');
			return false;
		});
Unit.prototype.__defineGetter__('inAmuletPosition', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 2); });
Unit.prototype.__defineGetter__('inArmorPosition', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 3); });
Unit.prototype.__defineGetter__('inBelt', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 0); });
Unit.prototype.__defineGetter__('inBeltPosition', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 8); });
Unit.prototype.__defineGetter__('inBootsPosition', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 9); });
Unit.prototype.__defineGetter__('inGlovesPosition', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 10); });
Unit.prototype.__defineGetter__('inHandRightPositionOne', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 4); });
Unit.prototype.__defineGetter__('inHandRightPositionTwo', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 11); });
Unit.prototype.__defineGetter__('inHandLeftPositionOne', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 5); });
Unit.prototype.__defineGetter__('inHandLeftPositionTwo', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 12); });
Unit.prototype.__defineGetter__('inHelmetPosition', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 1); });
Unit.prototype.__defineGetter__('inInventory', function(){ return (this.location == ITEMLOC_INVENTORY && this.bodylocation == 0); });
Unit.prototype.__defineGetter__('inRingLeftPosition', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 7); });
Unit.prototype.__defineGetter__('inRingRightPosition', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation == 6); });
Unit.prototype.__defineGetter__('inTown', function(){ return ( this.area == 0 || this.area == 1 || this.area == 40 || this.area == 75 || this.area == 103 || this.area == 109 ); });
Unit.prototype.__defineGetter__('isAmazon', function(){ return this.classid == 0; });
Unit.prototype.__defineGetter__('isAmazonValkyrie', function(){ return this.classid == 357; });
Unit.prototype.__defineGetter__('isAssassin', function(){ return this.classid == 6; });
Unit.prototype.__defineGetter__('isAssassinShadowMaster', function(){ return this.classid == 418; });
Unit.prototype.__defineGetter__('isAssassinShadowWarrior', function(){ return this.classid == 417; });
Unit.prototype.__defineGetter__('isBarbarian', function(){ return this.classid == 4; });
Unit.prototype.__defineGetter__('isCube', function(){ return this.classid == 549; });
Unit.prototype.__defineGetter__('isDead', 
		function()
		{
			if(this.name == me.name)
			{
				if(me.mode == 17) 
					return true;
			}
			else
			{
				if(this.mode == 12)
					return true;
			}
			
			return false;
		});
Unit.prototype.__defineGetter__('isDruid', function(){ return this.classid == 5; });

Unit.prototype.__defineGetter__('isEnchantable', 
		function()
		{ 	
		
			return	(((this.isNecroRevive || this.isNecroSkeleton || this.isNecroGolem || 
						this.isAmazonValkyrie || this.isAssassinShadowWarrior || this.isAssassinShadowMaster || this.isMerc) && (this.parentInParty)) ||
							(this.isPartyMember && this.mode != 17)); 
		});

Unit.prototype.__defineGetter__('isEnchanted', function(){ return this.getState(16); });
Unit.prototype.__defineGetter__('isEquipped', function(){ return (this.location == ITEMLOC_EQUIP && this.bodylocation > 0 && this.bodylocation <= 12); });
Unit.prototype.__defineGetter__('isMerc', function(){ return (this.classid == 271 || this.classid == 338 || this.classid == 359 || this.classid == 560 || this.classid == 561); });
Unit.prototype.__defineGetter__('isMercItem', 
		function()
		{
			var myMerc = me.getMerc();
			
			if(!myMerc)
				return false;
			
			var myMercItems = myMerc.getItems();
			
			for(var i = 0; i < myMercItems.length; i++)
			{
				if(this.gid == myMercItems[i].gid)
					return true;
			}
			
			return false;
		});
Unit.prototype.__defineGetter__('isNecroGolem', function(){ return (this.classid == 289 || this.classid == 290 || this.classid == 291 || this.classid == 292); });
Unit.prototype.__defineGetter__('isNecromancer', function(){ return this.classid == 2; });
Unit.prototype.__defineGetter__('isNecroRevive', function(){ return this.getState(96); });
Unit.prototype.__defineGetter__('isNecroSkeleton', function(){ return this.classid == 363; });
Unit.prototype.__defineGetter__('isNotEquipped', function(){ return this.bodylocation == 0; });
Unit.prototype.__defineGetter__('isNpc', function(){ return this.type == 1; });
Unit.prototype.__defineGetter__('isPaladin', function(){ return this.classid == 3; });
Unit.prototype.__defineGetter__('isPartyMember',
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
		});
	
Unit.prototype.__defineGetter__('isPlayer', function(){ return this.type == 0; });
Unit.prototype.__defineGetter__('isSorceress', function(){ return this.classid == 1; });
Unit.prototype.__defineGetter__('isStash', function(){ return this.classid == 267; });
Unit.prototype.__defineGetter__('parentInParty', 
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
						break;
				}
			}
			return false;
		});
	
	
	
	
	
// Class Methods
	
Unit.prototype.getSkillCharges =
		function()
		{
			//item charges are stored in an object (or object array) at index 204 of Unit.getStat(-2) return array;
			var list = [];
			var stats = this.getStat(-2);

			//if there is no index at [204]
			if(stats.length < 205)
				return undefined;

			if(stats[204])
			{
				if(stats[204].isArray)
					return stats[204];
				if(stats[204].isObject)
				{
					list.push(stats[204]);
				}
			}

			return list;
		};