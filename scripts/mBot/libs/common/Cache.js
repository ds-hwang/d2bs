var Ini = function(path, cache) {
	var iniPath = path,
		useCache = cache,
		cacheName = path.substr(path.lastIndexOf("/") + 1, path.lastIndexOf(".") - path.lastIndexOf("/") - 1) + ".cache",
		cachePath = "logs/" + cacheName;
		
	this.Read = function () {
		if (useCache && this.IsCacheUpToDate()) {
			var cacheFile = File.open(cachePath, 0);
			if (cacheFile) {
				cacheFile.readLine();
				var cacheData = cacheFile.readLine();
				cacheFile.close();
				Interface.message(Debug, cacheName + " found and loaded.");
				return JSON.parse(cacheData);
			}
		}
		Interface.message(Debug, "Cache not enabled or out-of-date, reading file.");
		var iniFile = File.open(path, 0), currentSection = "", lines = iniFile.readAllLines(), data = {};
		for (var lineNum = 1, line = lines[0]; (lineNum - 1) < lines.length; lineNum++, line = lines[lineNum - 1]) {
			//Strip any comments out of the code.
			if (line.indexOf(";") != -1)
				line = line.substr(0, line.indexOf(";"));
				
			//If line is empty/comment, skip.
			if (line.length == 0)
				continue;
				
			//Check for a new section.
			var section = /^\[(.+?)\]/.exec(line);
			if (section) {
				currentSection = section[1];
				data[currentSection] = {};
				continue;
			}
			
			//Check for key/value pair.
			var stat = /(.+?)=(.+)/.exec(line);
			if (!stat || stat.length != 3) {
				Interface.message(Warning, "Unable to process line " + lineNum + " of " + iniPath);
				continue;
			}
			if (data[currentSection].hasOwnProperty(stat[1])) {
				Interface.message(Warning, "Duplicate key value on " + lineNum + ". Line skipped.");
				continue;
			}
			data[currentSection][stat[1]] = stat[2];
		}
		FileTools.writeText(cachePath, iniFile.modified + "\n" + JSON.stringify(data));
		iniFile.close();
		return data;
	}
	
	this.IsCacheUpToDate = function () {
		if (!FileTools.exists(iniPath) || !FileTools.exists(cachePath))
			return false;
			
		var iniFile = File.open(iniPath, 0);
		var cacheFile = File.open(cachePath, 0);

		//Check if cache is up to date.
		if (iniFile.modified == cacheFile.readLine()) {
			iniFile.close();
			cacheFile.close();
			return true;
		}
			
		//Not up-to-date, close and return false.
		iniFile.close();
		cacheFile.close();
		return false;	
	}
}