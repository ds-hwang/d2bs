var GameManager = new function () {
	this.gameCount = 0;
	this.screens = {};
	this.config = {};
	this.delayConfig = {};
	this.Mode = {SinglePlayer:0, BattleNet:1};
	this.leechBot = {GameName:"", GamePass:""};
	this.inChannel = false;
	
	this.Init = function () {
		this.config = Interface.readConfig("GameManager", [{Name:"Mode", Default:0},{Name:"AccountName", Default:""},{Name:"AccountPass", Default:""},{Name:"CharSlot", Default:""},
		{Name:"UseChannel", Default:false}, {Name:"ChannelName", Default:""},{Name:"GamePrefix", Default:""},{Name:"GamePass", Default:""},
		{Name:"GameDiff", Default:0}, {Name:"LeechBot", Default:false}, {Name:"LeechMaster", Default:""}]);
		this.delayConfig = Interface.readConfig("Delays", [{Name:"Login", Default:5000}, {Name:"CharSelect", Default:2000}, {Name:"Lobby", Default: 500}, {Name:"GameCreateMin", Default:5000},
		{Name:"GameCreateMax", Default:10000}, {Name:"ChannelJoin", Default:2000}, {Name:"JoinGameDelay", Default:3000}]);
		addEventListener("chatmsg", this.chatMsg);
	}
	
	this.chatMsg = function(nWho, nMsg) {
		if (nWho.toLowerCase() == this.config.LeechMaster && this.config.LeechBot) {
			var nSplit = nMsg.split("//");
			this.leechBot.GameName = nSplit[0];
			this.leechBot.GamePass = nSplit[1];
		}
	}
	
	this.checkScreen = function(nScreen) {
		var Controls = {};
		for (var nControl in nScreen.Controls)
			if (!(Controls[nControl] = getControl.apply(null, nScreen.Controls[nControl])))
				return false;
		return Controls;
	}
	
	this.Tick = function () {
		for (var nScreen in this.screens) {
			var Controls = {};
			if (this.screens[nScreen].hasOwnProperty("Subscreen")) {
				for (var nSubScreen in this.screens[nScreen].Subscreen)
					if ((Controls = this.checkScreen(this.screens[nScreen].Subscreen[nSubScreen])))
						return this.screens[nScreen].Subscreen[nSubScreen].Event.call(this, Controls);
				continue;
			}
			if ((Controls = this.checkScreen(this.screens[nScreen])))
				return this.screens[nScreen].Event.call(this, Controls);
		}
		return true;
	}
	
	this.delay = function (nMsg, nType) {
		var nTimer = getTickCount();
		if (arguments.length == 3) {
			var nAm = rnd(this.delayConfig[nType + "Min"], this.delayConfig[nType + "Max"]);
		} else {
			var nAm = this.delayConfig[nType];
		}
		var nText = new Text(nMsg + " - " + (nAm - (getTickCount() - nTimer)) + " seconds remaining.", 100, 100);
		while(nAm > (getTickCount() - nTimer)) {
			nText.text = nMsg + " - " + Math.round((nAm - (getTickCount() - nTimer)) / 1000 * 10)/10 + " seconds remaining.";
			delay(100);
		}
		nText.remove();
		return true;
	}
	
	this.chooseMode = function(nControls) {
		for (var n in this.Mode)
			if (this.Mode[n] == this.config.Mode) {
				nControls[n].click();
				delay(50);
				return nControls[n].click();
			}
		return true;
	}
	
	this.login = function(nControls) {
		this.delay("General Delay", "Gen");
		nControls.AccountName.setText(this.config.AccountName);
		nControls.AccountPass.setText(this.config.AccountPass);
		this.delay("Login Delay", "Login");
		nControls.LoginButton.click();
		delay(50);
		nControls.LoginButton.click();
	}
	
	this.UnableToConnect = function(nControls) {
		debugLog("You are unable to connect");
		nControls.Ok.click();
	}
	
	this.OnCDKeyUse = function(nControls) {
		debugLog("Your CD-Key is in use by: " + nControls.ByWho.text);
		nControls.Ok.click();
	}
	
	this.CDKeyInvalid = function(nControls) {
		debugLog("Your CD-Key is invalid");
		nControls.Ok.click();
	}
	
	this.SelectChar = function(nControls) {
		var nSlots = ["", "SlotOne", "SlotTwo", "SlotThree", "SlotFour", "SlotFive", "SlotSix", "SlotSeven", "SlotEight"];
		if (this.config.CharSlot < 1 && this.config.CharSlot > 8)
			return;
		this.delay("Character Selection", "CharSelect");
		nControls[nSlots[this.config.CharSlot]].click();
		delay(50);
		nControls[nSlots[this.config.CharSlot]].click();
	}
	
	this.LobbyScreen = function(nControls) {
		delay(rnd(1000, 1500));
		if (this.config.UseChannel) {
			nControls.EnterChat.click();
			this.inChannel = false;
			this.delay("Lobby Delay", "Lobby");
			return;
		}
		this.ChatScreen(nControls);
	}
	
	this.ChatScreen = function(nControls) {
		delay(rnd(1000, 1500));
		if (this.config.UseChannel && !this.inChannel) {
			say("/join " + this.config.ChannelName);
			this.delay("Channel Join Delay", "ChannelJoin");
			this.inChannel = true;
		}
		if (this.config.LeechBot) {
			nControls.JoinGame.click();
			return;
		}
		nControls.CreateGame.click();
	}
	
	this.createGame = function(nControls) {
		delay(rnd(1000, 1500));
		this.gameCount++;
		nControls.GameName.setText(this.config.GamePrefix + this.gameCount);
		nControls.GamePass.setText(this.config.GamePass);
		var nDiff = ["Normal", "Nightmare", "Hell"];
		nControls[nDiff[this.config.GameDiff]].click();
		this.delay("Creating Game", "GameCreate", true);
		nControls.Create.click();
	}
	this.joinGame = function(nControls) {
	
	}
	
	this.chooseDiff = function(nControls) {
		delay(rnd(250,500));
		var nDiff = ["Normal", "Nightmare", "Hell"];
		nControls[nDiff[this.config.GameDiff]].click();
		nControls[nDiff[this.config.GameDiff]].click();
	}
	
	this.screens.MainScreen = {Name:"Main Screen", Subscreen: [
		{Name:"Unable To Connect", Controls:{Body:[4,162,420,477,100], Ok:[6,335,450,128,35]}, Event:this.UnableToConnect},
		{Name:"CD-Key In Use", Controls:{ByWho:[4,158,310,485,40], Ok:[6,335,450,128,35]}, Event:this.OnCDKeyUse},
		{Name:"CD-Key Invalid", Controls:{Body:[4,162,320,477,100], Ok:[6,335,450,128,35]}, Event:this.CDKeyInvalid},
		{Name:"Main Screen", Controls:{SinglePlayer:[6,264,324,272,35], BattleNet:[6,264,366,272,35]}, Event:this.chooseMode}]};
	this.screens.Login = {Name:"Login", Controls:{AccountName:[1,322,342,162,19], AccountPass:[1,322,396,162,19], LoginButton:[6,264,484,272,35]}, Event:this.login};
	this.screens.CharacterSelect = {Name:"Character Select", Subscreen:[
		{Name:"Select Difficulty", Controls:{Normal:[6,264,297,272,35], Nightmare:[6,264,340,272,35], Hell:[6,264,383,272,35]}, Event:this.chooseDiff},
		{Name:"Character Select", Controls:{SlotOne:[4,237,178,72,93], SlotTwo:[4,509,178,72,93], SlotThree:[4,237,271,72,93], 
		SlotFour:[4,509,271,72,93], SlotFive:[4,237,364,72,93], SlotSix:[4,509,364,72,93], SlotSeven:[4,237,457,72,93], SlotEight:[4,509,457,72,93]}, Event:this.SelectChar},]};
	this.screens.Lobby = {Name:"Lobby", Subscreen: [
		{Name:"Create Game", Controls:{GameName:[1,432,162,158,20], GamePass:[1,432,217,158,20], Normal:[6,430,381,16,16], Nightmare:[6,555,381,16,16], Hell:[6,698,381,16,16], Create:[6,594,433,172,32]}, Event:this.createGame},
		{Name:"Join Game", Controls:{GameName:[4,428,121,150,12], GamePass:[4,602,121,150,12], Join:[6,594,433,172,32]}, Event:this.joinGame},
		{Name:"Lobby", Controls:{EnterChat:[6,27,480,120,20], CreateGame:[6,533,469,120,20], JoinGame:[6,652,469,120,20]}, Event:this.LobbyScreen}
	]};
	this.screens.Chat = {Name:"Chat", Subscreen:[
		{Name:"Create Game", Controls:{GameName:[1,432,162,158,20], GamePass:[1,432,217,158,20], Normal:[6,430,381,16,16], Nightmare:[6,555,381,16,16], Hell:[6,698,381,16,16], Create:[6,594,433,172,32]}, Event:this.createGame},
		{Name:"Join Game", Controls:{GameName:[4,428,121,150,12], GamePass:[4,602,121,150,12], Join:[6,594,433,172,32]}, Event:this.joinGame},
		{Name:"Chat Screen", Controls:{CreateGame:[6,533,469,120,20], JoinGame:[6,652,469,120,20]}, Event:this.ChatScreen}
	]};
		}