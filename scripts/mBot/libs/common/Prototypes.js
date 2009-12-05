if(!Date.prototype.hasOwnProperty("toShortTimeString")) {
	Date.prototype.toShortTimeString = function () {
		function f(x) { if(x < 10) return "0"+x; return x; }
		var hours = this.getHours();
		hours = hours >= 12 ? hours - 12 : hours;
		var mins = this.getMinutes();
		var secs = this.getSeconds();
		var mer = this.getHours() >= 12 ? "PM" : "AM";
		return f(hours) + ":" + f(mins) + ":" + f(secs) + " " + mer;
	};
}

String.prototype.trim = function () {
    return this.replace(/^\s*/, "").replace(/\s*$/, "");
}