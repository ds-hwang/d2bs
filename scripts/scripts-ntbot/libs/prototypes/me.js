/**
*	@name		me.js
*
*	@author		alogwe
*
*	@version	1.0
*
*	@desc		Prototype library for D2BS 'Me' Object
*
*				Note: 	Fold syntax on sets of {} for easier viewing 
*						If using notepad++ with JavaScript syntax highlighting:
*							Fold All	: Alt + 0
*							Unfold All	: Alt + Shift + 0
*/

me.__defineGetter__('inTown', function(){ return (me.area == 0 || me.area == 1 || me.area == 40 || me.area == 75 || me.area == 103 || me.area == 109 ); });

