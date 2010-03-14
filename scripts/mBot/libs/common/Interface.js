const DetailedDebug = 0;
const Debug = 1;
const Normal = 2;
const Warning = 3;

var Interface = new function () {
	this.profileName = "";
	this.mBotConfig = {};
	this.config = {};
	this.cache;
	
	this.Init = function () {
		this.profileName = "McGod";
		while(this.profileName.length == 0)
			delay(100);
		this.cache = new Ini("profile/" + this.profileName + "/mBot.ini", true);
		this.mBotConfig = this.cache.Read();
		this.config = this.readConfig("Interface", [{Name:"MessageType", Default:2}]);
	}
	this.read = function(nSection, nName, nDefault) {
		if (!this.mBotConfig.hasOwnProperty(nSection))
			var ret = nDefault;
		else
			var ret = this.mBotConfig[nSection][nName];
			
		if (typeof(nDefault) == "number")
			return parseInt(ret);
		if (typeof(nDefault) == "boolean")
			return parseBoolean(ret);
		return ret;
	}
	
	this.readConfig = function (nSection, nConfig) {
		var nRet = {};
		for (var n = 0; n < nConfig.length; n++) {
			if (!this.mBotConfig.hasOwnProperty(nSection))
				var ret = String(nConfig[n].Default);
			else
				var ret = this.mBotConfig[nSection][nConfig[n].Name];
			if (typeof(nConfig[n].Default) == "number")
				nRet[nConfig[n].Name] = parseInt(ret);
			if (typeof(nConfig[n].Default) == "boolean")
				nRet[nConfig[n].Name] = parseBoolean(ret);
		}
		return nRet;
	}

	this.message = function(nType, nMsg) {
		if (this.config.hasOwnProperty("MessageType") && nType < this.config.MessageType)
			return false;
			
		var TypeNames = ["DDebug", "Debug", "", "Warning", "Error"];
		
		if (TypeNames[nType].length == 0)
			mBot.Print(nMsg)
		else
			mBot.Print("[" + TypeNames[nType] + "]: " + nMsg);
		this.writeLog("Events.log", "[" + TypeNames[nType] + "]: " + nMsg);
		return true;
	}
	
	this.writeLog = function(nFile, nMessage) {
		var nDate = new Date();
		var nFil = File.open("//logs//" + this.profileName + "//" + nFile, 2);
		nFil.write("[" + nDate.toShortTimeString() + "]: " + nMessage + "\n");
		nFil.close();
	}
}

function parseBoolean(nStr) {
	switch (String(nStr).toLowerCase()) {
		case "y":
		case "yes":
		case "1":
		case "true":
			return true;
		break;
	}
	return false;
}