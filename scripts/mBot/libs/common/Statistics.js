/*
	Statistics.js
		By McGod
		
	The intentions of Statistics is to create a detailed database of
	items found, boss statuses and time took to complete each boss, information
	about the every run like gamename, time spent in game and so forth. Eventually
	I hope to have some sort of information gatherer for this so that we may have a
	global statistics module(optionally of course).
*/

var Status = {Successful:0, Failed:1, Death:2, Skip:3};
var Boss = {Andariel:1, Mephisto:2};
var Type = {Message:0, Warning:1, Error:2};

var Statistics = new function () {
	this.statusRows = ["runSuccess", "runFailure", "runDeath", "runSkip"];
	this.statusMsg = ["successful", "failure", "died", "skipped"];
	this.db = new SQLite("//logs//" + Interface.profileName + "//" + "Statistics.db", true);
	this.currentBoss = 0;
	this.bossTimer = 0;
	this.runId = 0;
	
	this.beginBoss = function(boss) {
		this.currentBoss = boss;
		this.bossTimer = getTickCount ();
	}
	
	this.getCurrentBoss = function () {
		for (var n in Boss) {
			if (Boss[n] == this.currentBoss)
				return n;
		}
		return false;
	}
	
	this.endBoss = function(status) {
		var timeSpent = (getTickCount () - this.bossTimer);
		this.db.execute("UPDATE BossStats SET " + this.statusRows[status] + " = " + this.statusRows[status] + " + 1, timeSpent = timeSpent + " + timeSpent + ", runTotal = runTotal + 1 WHERE id = " + this.currentBoss);
		this.db.execute("UPDATE Runs SET boss" + this.currentBoss + "status = " + status + ", boss" + this.currentBoss + "time = " + timeSpent + " WHERE id = " + this.runId);
		return Math.round(timeSpent / 1000);
	}
	
	this.msg = function(id, msg) {
		print("ÿc5mBot ÿc#::ÿc4 Statistics ÿc#::ÿc0 " + msg);
	}
	
	this.itemFound = function(item, pickitEntry) {
		this.msg(Type.Message, "Keepingÿc" + itemColor[item.quality] + " " + item.name);
		this.db.execute("UPDATE Runs SET itemCount = itemCount + 1 WHERE id = " + this.runId);
		this.db.query("INSERT INTO Items (name, quality, time, boss, desc, pickitdesc) VALUES (?, ?, DATETIME('now'), ?, ?, ?)", item.name, item.quality, this.currentBoss, item.description, pickitEntry.desc);
	}
	
	this.newRun = function() {
		this.db.query("INSERT INTO Runs (gameName, gamePass, date) VALUES (?, ?, DATETIME('now'))", me.gamename, me.gamepassword).go();
		this.runId = this.db.lastRowId;
	}
}