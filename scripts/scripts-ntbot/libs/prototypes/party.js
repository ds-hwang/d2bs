/**
*	@name		party.js
*
*	@author		alogwe
*
*	@version	1.0
*
*	@desc		Prototype library to extend D2BS 'Party' Class objects
*
*				Note: 	Fold syntax on sets of {} for easier viewing 
*						If using notepad++ with JavaScript syntax highlighting:
*							Fold All	: Alt + 0
*							Unfold All	: Alt + Shift + 0
*/


// Class Properties
Party.prototype.__defineGetter__('isPartied', function(){ return this.partyflag == 1; });


// Class Methods
{Party.prototype.getUser = 
	function(username)
	{
		// Validate arguments
		if(arguments.length == 0 || !(username.isString))
			return false;		
		// Loop through party to find username
		do
		{
			if(this.name == username && (this.isPartied || username == me.name))
				return this;
		}while(this.getNext());
		// Did not find username
		return 0;
	}
}