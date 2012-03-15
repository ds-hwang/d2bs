js_strict(true);

String.prototype.trim = function(){ return this.replace(/^\s+|\s+$/g, ''); };