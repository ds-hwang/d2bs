const DetailedDebug = 0;
const Debug = 1;
const Normal = 2;
const Warning = 3;

var Interface = new function () {
	this.profileName = "";
	this.config = {};
	
	this.Init = function () {
		this.profileName = "McGod";
		while(this.profileName.length == 0)
			delay(100);
		this.config = this.readConfig("Interface", [{Name:"MessageType", Default:2}]);
	}
		this.read = function(nSection, nName, nDefault) {
		var ret = iniread("profile/" + this.profileName + "/mBot.ini", nSection, nName, nDefault);
		if (typeof(nDefault) == "number")
			return parseInt(ret);
		if (typeof(nDefault) == "boolean")
			return parseBoolean(ret);
		return ret;
	}
	
	this.readConfig = function (nSection, nConfig) {
		var nRet = {};
		for (var n = 0; n < nConfig.length; n++) {
			var ret = iniread("profile/" + this.profileName + "/mBot.ini", nSection, nConfig[n].Name, nConfig[n].Default);
			nRet[nConfig[n].Name] = ret;
			if (typeof(nConfig[n].Default) == "number")
				nRet[nConfig[n].Name] = parseInt(ret);
			if (typeof(nConfig[n].Default) == "boolean")
				nRet[nConfig[n].Name] = parseBoolean(ret);
		}
		return nRet;
	}

	this.message = function(nType, nMsg) {
		if (nType < this.config.MessageType)
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
	
	this.iniread = function(file, section, name, defaultValue) {
		/*var nFile = File.open(file, 0);
		if (!nFile)
			return defaultValue;
		var nSection = false;
		while(!nFile.eof) {
			var line = nFile.readLine();
			var nSection = /\[(.+?)\]/.exec(line);
			if (nSection) {
				if (nSection[1].toLower() == section.toLower())
					nSection = true;
				else
					nSection = false;
			}
			if (line.indexOf("=") != -1 && nFile
		}*/
		return defaultValue;
	}
}

function parseBoolean(nStr) {
	switch (nStr.toLowerCase()) {
		case "y":
		case "yes":
		case "1":
		case "true":
			return true;
		break;
	}
	return false;
}