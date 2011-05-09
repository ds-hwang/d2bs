/**
*	@name		javascript.js
*
*	@author		alogwe
*
*	@version	1.0
*
*	@desc		Prototype library to extend JavaScript
*				
*				Note: 	Fold syntax on sets of {} for easier viewing 
*						If using notepad++ with JavaScript syntax highlighting:
*							Fold All	: Alt + 0
*							Unfold All	: Alt + Shift + 0
*/


// Object Class Properties
Object.prototype.__defineGetter__('isBoolean', function(){ return (this instanceof Boolean); });
Object.prototype.__defineGetter__('isNumber', function(){ return (this instanceof Number); });
Object.prototype.__defineGetter__('isString', function(){ return (this instanceof String); });
Object.prototype.__defineGetter__('isArray', function(){ return (this instanceof Array); });
Object.prototype.__defineGetter__('isObject', function(){ return (this instanceof Object); });
Object.prototype.__defineGetter__('isFunction', function(){ return (this instanceof Function); });
Object.prototype.__defineGetter__('isRegExp', function(){ return (this instanceof RegExp); });
Object.prototype.__defineGetter__('isDate', function(){ return (this instanceof Date); });
Object.prototype.__defineGetter__('isError', function(){ return (this instanceof Error); });
Object.prototype.__defineGetter__('isEvalError', function(){ return (this instanceof EvalError); });
Object.prototype.__defineGetter__('isRangeError', function(){ return (this instanceof RangeError); });
Object.prototype.__defineGetter__('isReferenceError', function(){ return (this instanceof ReferenceError); });
Object.prototype.__defineGetter__('isSyntaxError', function(){ return (this instanceof SyntaxError); });
Object.prototype.__defineGetter__('isTypeError', function(){ return (this instanceof TypeError); });
Object.prototype.__defineGetter__('isURIError', function(){ return (this instanceof URIError); });

// Object Class Methods
Object.prototype.toArray = function(){ return Array.prototype.slice.call(this); };

if(!Object.keys) 
	Object.keys = function(o)
	{
		if(o !== Object(o))
			throw new TypeError('Object.keys called on non-object');
		var ret = [], p;
		for(p in o) 
			if(Object.prototype.hasOwnProperty.call(o,p)) 
				ret.push(p);
		return ret;
	}

	
// Array Class Properties
// Array Class Methods

// **NOTE** Array.removeDuplicates is not a mutator method;
Array.prototype.removeDuplicates = 
	function()
	{
		var result = [];
		
		for(var i=0; i<this.length; i++)    
		{
			if(result.indexOf(this[i]) == -1)
				result.push(this[i]);
		}

		return result;
	};

Array.prototype.remove = 
	function(from, to) 
	{
		var rest = this.slice((to || from) + 1 || this.length);
		this.length = from < 0 ? this.length + from : from;
		return this.push.apply(this, rest);
	};


// String Class Properties
String.prototype.__defineGetter__('isEmpty', function(){ return this.length == 0; });
// String Class Methods
String.prototype.toRandomCase = 
	function()
	{
		var result = '';
		
		for(var i=0; i<this.length; i++)
		{
			var x = Math.round(Math.random()); //0 or 1
			if(x)
				result += this.charAt(i).toUpperCase();
			else
				result += this.charAt(i).toLowerCase();
		}
		
		return result;
	};
