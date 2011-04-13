/**
*	@name		d2bs.js
*
*	@author		alogwe
*
*	@version	1.0
*
*	@desc		Prototype library to extend JavaScript 'Object' class for getting D2BS types
*				
*				Note: 	Fold syntax on sets of {} for easier viewing 
*						If using notepad++ with JavaScript syntax highlighting:
*							Fold All	: Alt + 0
*							Unfold All	: Alt + Shift + 0
*/


// Class Properties
Object.prototype.__defineGetter__('isD2BSArea', function(){ return ( this instanceof Area ); });
Object.prototype.__defineGetter__('isD2BSBox', function(){ return ( this instanceof Box ); });
Object.prototype.__defineGetter__('isD2BSControl', function(){ return ( this instanceof Control ); });
// Object.prototype.__defineGetter__('isD2BSScript', function(){ return ( this instanceof D2BSScript ); });	//commented out becuase this object typedef doesn't exist in D2BS 1.3.3
Object.prototype.__defineGetter__('isD2BSDBStatement', function(){ return ( this instanceof DBStatement ); });
Object.prototype.__defineGetter__('isD2BSExit', function(){ return ( this instanceof Exit ); });
Object.prototype.__defineGetter__('isD2BSFile', function(){ return ( this instanceof File ); });
Object.prototype.__defineGetter__('isD2BSFileTools', function(){ return ( this instanceof FileTools ); });
Object.prototype.__defineGetter__('isD2BSFolder', function(){ return ( this instanceof Folder ); });
Object.prototype.__defineGetter__('isD2BSFrame', function(){ return ( this instanceof Frame ); });
Object.prototype.__defineGetter__('isD2BSImage', function(){ return ( this instanceof Image ); });
Object.prototype.__defineGetter__('isD2BSLine', function(){ return ( this instanceof Line ); });
Object.prototype.__defineGetter__('isD2BSParty', function(){ return ( this instanceof Party ); });
Object.prototype.__defineGetter__('isD2BSPresetUnit', function(){ return ( this instanceof PresetUnit ); });
Object.prototype.__defineGetter__('isD2BSRoom', function(){ return ( this instanceof Room ); });
Object.prototype.__defineGetter__('isD2BSSandbox', function(){ return ( this instanceof Sandbox ); });
Object.prototype.__defineGetter__('isD2BSSQLite', function(){ return ( this instanceof SQLite ); });
Object.prototype.__defineGetter__('isD2BSText', function(){ return ( this instanceof Text ); });
Object.prototype.__defineGetter__('isD2BSUnit', function(){ return ( this instanceof Unit ); });


// Class Methods


