include('prototypes.dbl');

include('mbot/interface.dbl');

var Ini = new function() {
	this.Read = function(filePath) {
		if (!FileTools.exists(filePath)) {
			print('Ini-file not found ('+filePath+')');
			//Interface.Display(Message.Debug, 'Ini-file not found ('+filePath+')');
			return {};
		}
		var content = FileTools.readText(filePath),
			section = '',
			data = {},
			i = 1;
		for each(let line in content.split('\n')) {
			if (line.indexOf(';') != -1)
				line = line.substr(0, line.indexOf(';'));
			line = line.trim();
			if (line.length < 1) continue;
			var sec = /^\[(.+?)\]/.exec(line);
			if (sec) {
				section = sec[1].trim();
				data[section] = {};
				continue;
			}
			var stat = /(.+?)=(.+)/.exec(line);
			if (!stat || stat.length != 3) {
				print('Unable to parse line '+i+' in '+filePath);
				//Interface.Display(Message.Warning, 'Unable to parse line '+i+' in '+filePath);
				continue;
			}
			data[section][stat[1].trim()] = stat[2].trim();
			i++;
		}
		return data;
	};
	this.Write = function(data) {
		var str = '';
		for(let section in data) {
			str += '['+section+']';
			for(let stat in section)
				str += stat+'='+section[stat];
		}
		File.writeText(filePath, str);
	};
};