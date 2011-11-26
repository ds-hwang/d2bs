/**
*	@name		exit.js
*
*	@author		alogwe
*
*	@version	1.1
*
*	@desc		Prototype library to extend D2BS 'Exit' Class objects
*				
*				NOTE: 	Fold syntax on sets of {} for easier viewing 
*						If using notepad++ with JavaScript syntax highlighting:
*							Fold All	: Alt + 0
*							Unfold All	: Alt + Shift + 0
*/

// Notes: 
//	Must be within getUnit() range to use Exit.toUnit() successfully;
//	Will only work for tile exits (because they have an associated Unit);

Exit.prototype.toUnit = function(){
	var u = getUnit(5, this.tileid);
	if(u){
		do{
			if((this.x == u.x) && (this.y == u.y)){
				return u;
			}
		} while(u.getNext)
	}
	return undefined;
};
