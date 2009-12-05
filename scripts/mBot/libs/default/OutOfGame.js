/*
	OutOfGame.js
		This is the default game joining OOG code.
*/

if (!isIncluded("mBot/OutOfGame.js")) {
	include("mBot.js");
	mBot.error("default/OutOfGame.js was included without the core OutOfGame!");
	return;
}

mBot.OOG.onMainScreen = function(nControls) {
	
}