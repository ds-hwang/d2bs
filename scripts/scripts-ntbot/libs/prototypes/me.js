/**
*	@name		me.js
*
*	@author		alogwe
*
*	@version	1.1
*
*	@desc		Prototype library for D2BS 'Me' Object
*
*				NOTE: 	Fold syntax on sets of {} for easier viewing 
*						If using notepad++ with JavaScript syntax highlighting:
*							Fold All	: Alt + 0
*							Unfold All	: Alt + Shift + 0
*/

//	Me Properties

me.__defineGetter__('inTown', function(){ return (me.area == 0 || me.area == 1 || me.area == 40 || me.area == 75 || me.area == 103 || me.area == 109 ); });







//	Me Methods

//NOTE: me.switchToWeaponTab accepts 1 or 2 for tab numbers (not 0 or 1)
me.switchToWeaponTab =
	function(x)
	{
		if(!x.isNumber || (x != 1 && x != 2))
			return false;
			
		if((x == 1 && me.weaponswitch == 0) || (x == 2 && me.weaponswitch == 1))
			return true;
		
		if(weaponSwitch())
			return true;
			
		return false;	//shouldn't ever happen;
	};
	
	
	
	
	