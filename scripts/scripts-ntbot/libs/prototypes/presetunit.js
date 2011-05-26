/**
*	@name		presetunit.js
*
*	@author		alogwe
*
*	@version	1.1
*
*	@desc		Prototype library to extend D2BS 'PresetUnit' Class objects
*				
*				NOTE: 	Fold syntax on sets of {} for easier viewing 
*						If using notepad++ with JavaScript syntax highlighting:
*							Fold All	: Alt + 0
*							Unfold All	: Alt + Shift + 0
*/

PresetUnit.prototype.toUnit = function(){ return getUnit(this.type, this.id); };