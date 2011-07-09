/**
*	@title			announceGame.js
*
*	@desc			Formats and sends a message to an active mIRC client via sendDDE()
*					Accompanying mIRC script will format the announcement and display in chosen channels
*
*	@requires		Installation of mIRC script which is commented at the bottom of this script
*
*	@installation	Save this script to your /tools/ dir
*					Load this script by adding the following line to your runner's char config file (or anywhere in default.dbj)
*
*					load('tools/announceGame.js');
*
*					To install the mIRC script, copy the commented section at the bottom of this file between the multiline 
*					comment brackets and paste that into a new file under your Remote scripts (ALT + R) section of the program
*					
*					Set your announce channels up under the line			;; ADD / CHANGE ANNOUNCE CHANNELS HERE ;;
*					You can format your announce message under the line		;; FORMAT / CHANGE ANNOUNCE MESSAGE HERE ;;
*/

function main(){
	announceGame();
}

function announceGame(){

	var ClassToName = new Array(7);
	ClassToName[0] = "Amazon";
	ClassToName[1] = "Sorceress";
	ClassToName[2] = "Necromancer";
	ClassToName[3] = "Paladin";
	ClassToName[4] = "Barbarian";
	ClassToName[5] = "Druid";
	ClassToName[6] = "Assassin";
	
	var DiffToName = new Array(3);
	DiffToName[0] = "Normal";
	DiffToName[1] = "Nightmare";
	DiffToName[2] = "Hell";
	
	var gameInfo = ''+ 
		me.realm +' '+ 
		((me.playertype == 0)?'Softcore':'Hardcore') +' '+ 
		((me.gametype == 0)?'Classic':'Expansion') +' '+
		//me.ladder +' '+	//me.ladder is bugged in D2BS 1.4 (always false) 
		me.name +' '+
		me.charlvl +' '+
		ClassToName[me.classid] +' '+
		DiffToName[me.diff] +' '+
		me.gamename +' '+ 
		me.gamepassword;
	
	sendDDE(1, 'mIRC', 'COMMAND', '', '/announceGame '+ gameInfo);
	
};



  
/*	mIRC Script for Remote Scripts section ;; Add this Alias to your Remote scripts in mIRC (Alt+R) and change the %announceChan var.





Alias announceGame {



  ;; ADD / CHANGE ANNOUNCE CHANNELS HERE ;;
  addAnnounceChan #IRCCHANNELHERE
  addAnnounceChan #SOME_OTHER_CHANNEL
  addAnnounceChan #ADD_MORE_LIKE_THIS_ETC



  var %charName = $4

  ;; Workaround for NTBot spamming sendDDE() from within char config (Only allows 1 announcement per charName every 15 seconds)

  if ($var( [ [ %charName ] $+ _announced ] , 0) > 0) { return }
  else { set -u15 [ % $+ [ %charName ] $+ _announced ] true }

  var %gameRealm = $1
  var %gameCore = $2
  var %gameType = $3
  var %charLvl = $5
  var %charClass = $6
  var %gameDiff = $7
  var %gameName = $8
  var %gamePassword = $9

  if ( $count($lower(%gameName), baal) > 0 ) { var %runType = new BAAL }
  else if ( $count($lower(%gameName), chaos) > 0 ) { var %runType = new CHAOS }
  else { var %runType = new }



  ;; FORMAT / CHANGE ANNOUNCE MESSAGE HERE ;;
  var %announceMsg (10 $+ %gameRealm %gameCore %gameType $+ 1) 7 $+ %charName $+ , my level %charLvl %charClass has started a %runType game:4  %gameName 1//4 %gamePassword



  ; Get total number of announceChans
  var %numAnnounceChans = $numtok(%announceChans, 32)

  ; Iterate across announceChans and send announce message to each
  var %count = 1

  while (%count <= %numAnnounceChans) {
    var %currentChan = $gettok(%announceChans, %count, 32)

    if ($me ison %currentChan) {
      msg %currentChan %announceMsg
    }

    inc %count
  }

}


Alias addAnnounceChan {
  set %announceChans $addtok(%announceChans, $1, 32)
}


Alias remAnnounceChan {
  set %announceChans $remtok(%announceChans, $1, 32)
}





*/