var Sequencer = new function () {
	this.config = {};
	this.sequenceList = [];
	
	this.Init = function () {
		this.config = Interface.readConfig("Sequencer", [{Name:"RandomizeSequences", Default:true}]);
		
		this.GetAllSequences();
	}
	
	this.Go = function () {
		Interface.message(Normal, "We have " + this.sequenceList.length + " sequence(s) available.");
		for (var n = 0; this.sequenceList.length != 0; n++) {
			var nCurrentSequence = n;
			if (this.config.RandomizeSequences)
				nCurrentSequence = this.GetRandomSequence();
			
			if (!Interface.read(this.sequenceList[nCurrentSequence], "Enabled", true)) {
				Interface.message(Normal, this.sequenceList[nCurrentSequence] + " isn't enabled! Skipping.");
				this.sequenceList.splice(nCurrentSequence, 1);
				continue;	
			}
			
			if (!mBot.inTown()) {
				if (!Pather.moveToTown())
					quit();
			}
			
			Town.tick();
			
			Interface.message(Normal, "Starting sequence '" + this.sequenceList[nCurrentSequence] + "'");
			
			include("sequences/" + this.sequenceList[nCurrentSequence] + ".js");
			
			if (!eval(this.sequenceList[nCurrentSequence] + "()"))
				Interface.message(Normal, "Sequence '" + this.sequenceList[nCurrentSequence] + "' has failed.");
			else
				Interface.message(Normal, "Sequence '" + this.sequenceList[nCurrentSequence] + "' has completed successfully.");
			
			this.sequenceList.splice(nCurrentSequence, 1);
		}
	}
	
	this.GetAllSequences = function () {
		var nDir = dopen("/libs/sequences/");
		var nFiles = nDir.getFiles();
		this.sequenceList = [];
		for (var n in nFiles) {
			if (nFiles[n].indexOf(".js") == -1)
				continue;
			this.sequenceList.push(nFiles[n].substr(0, nFiles[n].indexOf(".js") ));
		}
	}
	
	this.GetRandomSequence = function () {
		return Math.ceil(Math.random() * this.sequenceList.length) - 1;
	}
}