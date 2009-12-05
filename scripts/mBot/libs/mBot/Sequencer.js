var Sequencer = new function () {
	this.config = {};
	this.sequenceList = [];
	this.completedSequences = [];
	
	this.Init = function () {
		this.config = Interface.readConfig("Sequencer", [{Name:"RandomizeSequences", Default:true}]);
		
		this.GetAllSequences();
		print(this.sequenceList.toSource());
		for (var n in this.sequenceList)
			this.completedSequences.push(false);
	}
	
	this.Go = function () {
		for (var n = 0; n < this.sequenceList.length; n++) {
			var nCurrentSequence = n;
			if (this.config.RandomizeSequences)
				nCurrentSequence = this.GetRandomSequence();
			
			if (!Interface.read(this.sequenceList[nCurrentSequence], "Enabled", false)) {
				this.completedSequences[nCurrentSequence] = true;
				Interface.message(DetailedDebug, this.sequenceList[nCurrentSequence] + " isn't enabled! Skipping.");
				continue;	
			}
			
			if (!mBot.inTown())
				Pather.moveToTown();
				
			Town.tick();
			
			Interface.message(Normal, "Starting sequence '" + this.sequenceList[nCurrentSequence] + "'");
			
			include("sequences/" + this.sequenceList[nCurrentSequence] + ".js");
			
			if (!eval(this.sequenceList[nCurrentSequence] + "()"))
				Interface.message(Normal, "Sequence '" + this.sequenceList[nCurrentSequence] + "' has failed.");
			else
				Interface.message(Normal, "Sequence '" + this.sequenceList[nCurrentSequence] + "' has completed successfully.");
			
			this.completedSequences[nCurrentSequence] = true;
		}
	}
	
	this.GetAllSequences = function () {
		var nDir = dopen("/libs/sequences/");
		var nFiles = nDir.getFiles();
		for (var n in nFiles) {
			if (nFiles[n].indexOf(".js") == -1)
				continue;
			this.sequenceList.push(nFiles[n].substr(0, nFiles[n].indexOf(".js") ));
		}
	}
	
	this.GetRandomSequence = function () {
		var nRand = Math.ceil(Math.random() * this.sequenceList.length) - 1;
		if (this.completedSequences[nRand])
			return this.GetRandomSequence();
		return nRand;
	}
}