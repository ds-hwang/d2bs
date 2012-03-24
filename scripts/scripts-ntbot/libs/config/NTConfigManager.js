/// <reference path="/../../d2bsAPI.js" />
/// <reference path="/../common/HookedControls.js" />
var ConfigObjects = []
var ShowConfigHooks =[];
//var ConfigVals =[];
//var ChkHooks =[];
//var SliderHooks =[];
//var SliderConfigVals = [];
var useTeleport = me.useTeleport;
var NTConfig_ColumnsHooks = []		// needed for inventory layout hooks


function ConfigSetting(category, hook, globalVar, Comment) {
	this.val = false;
    this.setting = ""
    this.Hooks = []
    this.Category = category
    if (globalVar)
    	hook.hookedVal = globalVar;
    if (hook) this.addHook(hook)
    this.globalVar = globalVar;
	this.Comment = Comment
    ConfigObjects.push(this)
    this.valueChanged = false;
   
    this.watch("val", function (id, oldval, newval) {
        if (typeof (this.valueChanged) == "function")
            this.valueChanged()
        return newval;
    });
   
}
ConfigSetting.prototype.addHook = function (hook){
    hook.visible=false
    this.Hooks.push(hook)
}
this.ConfigSetting.prototype.name =""

function ShowConfig() {
	
    this.textColor =13;
	var bossList = getBossList();	//populate boss list with valid scripts from 'libs/bots/' folder;
	this.NTConfig_Script = new LinkedListBoxes(25, 120, 100, 200, bossList, NTConfig_Script, 4)
	this.NTConfig_Script.valueChanged = function (val) { NTConfig_Script = val }
	this.setNTConfig_Script = new ConfigSetting("Runs", this.NTConfig_Script, "NTConfig_Script")
	 	   
	var j = 120;

 	this.NTConfig_PublicMode = new CheckBox(300, j, "Public Mode", NTConfig_PublicMode, true);
	this.pub = new ConfigSetting("Runs", this.NTConfig_PublicMode,"NTConfig_PublicMode"); 
    
    this.chkSimonEnableSnagging = new CheckBox(300, j + 15, "Simon: Enable Snagging", SimonEnableSnagging, true);
    this.setchkSimonEnableSnagging = new ConfigSetting("Runs", this.chkSimonEnableSnagging, "SimonEnableSnagging");
    
    this.SimonAutoReveal = new CheckBox(300, j + 30, "Simon: Auto Reveal", SimonAutoReveal, true);
    this.setSimonAutoReveal = new ConfigSetting("Runs", this.SimonAutoReveal,"SimonAutoReveal")

    this.SimonStopLifeWatch = new CheckBox(300, j + 45, "Simon: Disable Life Watch", SimonStopLifeWatch, true);
    this.setSimonStopLifeWatch = new ConfigSetting("Runs", this.SimonStopLifeWatch,"SimonStopLifeWatch")
    
	this.SimonEnableCommands = new CheckBox(300,j + 60,"Simon: Enable Commands",SimonEnableCommands,true)
	this.setSimonEnableCommands = new ConfigSetting("Runs", this.SimonEnableCommands, "SimonEnableCommands")

	this.NTConfig_ClearPitLevel1 = new CheckBox(300, j + 75, "Pit: Clear lvl 1 ", NTConfig_ClearPitLevel1, true)
	this.setNTConfig_ClearPitLevel1 = new ConfigSetting("Runs", this.NTConfig_ClearPitLevel1,"NTConfig_ClearPitLevel1","// 	Set to true to clear level 1 of Pit")

	this.NTConfig_FireEyeExtension = new CheckBox(300, j + 90, "Summoner: Fire Eye Ext ", NTConfig_FireEyeExtension, true)
    this.setNTConfig_FireEyeExtension = new ConfigSetting("Runs",this.NTConfig_FireEyeExtension,"NTConfig_FireEyeExtension","		//	Set to true if you want to kill Fire Eye at arcane sanctuary portal when running Summoner script")


    this.NTConfig_KillFangskin = new CheckBox(300, j + 105, "ClawViper: Kill Fangskin ", NTConfig_KillFangskin, true)
    this.setNTConfig_KillFangskin = new ConfigSetting("Runs", this.NTConfig_KillFangskin,"NTConfig_KillFangskin","			//	Set to true if you want to kill Fangskin while running Claw Viper Temple script")

    this.NTConfig_CouncilExtension = new CheckBox(300, j + 120, "Mephisto: Kill Council ", NTConfig_CouncilExtension, true)
    this.setNTConfig_CouncilExtension = new ConfigSetting("Runs", this.NTConfig_CouncilExtension,"NTConfig_CouncilExtension"," 		//	Set to true if you want to kill council when running Mephisto script")

    this.NTConfig_Moattrick = new CheckBox(300, j + 135, "Mephisto: Moattrick ", NTConfig_Moattrick, true)
    this.setNTConfig_Moattrick = new ConfigSetting("Runs", this.NTConfig_Moattrick, "NTConfig_Moattrick", "				//	Set to true if you want to use the moat trick at Mephisto")

    this.NTConfig_NihlathakExtension = new CheckBox(300, j + 150, "Pindleskin: NihlathakExtension ", NTConfig_NihlathakExtension, true);
    this.setNTConfig_NihlathakExtension = new ConfigSetting("Runs", this.NTConfig_NihlathakExtension,"NTConfig_NihlathakExtension","		//	Set true to continue to Nihl ")

    this.NTConfig_PindleskinExtension = new CheckBox(300, j + 165, "Nihlathak: PindleskinExtension ", NTConfig_PindleskinExtension, true)
    this.setNTConfig_PindleskinExtension = new ConfigSetting("Runs", this.NTConfig_PindleskinExtension,"NTConfig_PindleskinExtension","	//	Set true if you want to continue to Pindle")

    this.NTConfig_ShenkExtension = new CheckBox(300, j + 180, "Eldritch: Shenk Ext ", NTConfig_ShenkExtension, true);
    this.setNTConfig_ShenkExtension = new ConfigSetting("Runs", this.NTConfig_ShenkExtension,"NTConfig_ShenkExtension")

    this.NTConfig_DacFarrenExtension = new CheckBox(300, j + 195, "Eldritch: DacFarren Ext ", NTConfig_DacFarrenExtension, true);
    this.setNTConfig_DacFarrenExtension = new ConfigSetting("Runs", this.NTConfig_DacFarrenExtension, "NTConfig_DacFarrenExtension")

	this.NTConfig_WSK2Extension = new CheckBox(300, j + 210, "WSK2 Ext ", NTConfig_WSK2Extension, true);
    this.setNTConfig_WSK2Extension = new ConfigSetting("Runs", this.NTConfig_WSK2Extension,"NTConfig_WSK2Extension")

    this.NTConfig_WSK3Extension = new CheckBox(300, j + 225, "WSK3 Ext ", NTConfig_WSK3Extension, true);
    this.setNTConfig_WSK3Extension = new ConfigSetting("Runs", this.NTConfig_WSK3Extension,"NTConfig_WSK3Extension")

    this.NTConfig_KillBaal = new CheckBox(300, j + 240, "Baal: Kill Baal ", NTConfig_KillBaal, true)
    this.setNTConfig_KillBaal = new ConfigSetting("Runs", this.NTConfig_KillBaal,"NTConfig_KillBaal")

    this.NTConfig_RuinedExtension = new CheckBox(300, j + 255, "Ruined Ext ", NTConfig_RuinedExtension, true);
    this.setNTConfig_RuinedExtension = new ConfigSetting("Runs", this.NTConfig_RuinedExtension,"NTConfig_RuinedExtension")
   
    this.NTConfig_DisusedExtension = new CheckBox(300, j + 270, "Disused Ext ", NTConfig_DisusedExtension, true);
    this.setNTConfig_DisusedExtension = new ConfigSetting("Runs", this.NTConfig_DisusedExtension,"NTConfig_DisusedExtension")

    this.DiabloTPMsg = new TextBox(500, 100, 200, 15, DiabloTPMsg, 3, 0);this.DiabloTPMsg.editable = true;
    this.setDiabloTPMsg = new ConfigSetting("Runs", this.DiabloTPMsg, "DiabloTPMsg", "				//	Set your message to announce when you make a town portal in Chaos Sanctuary when running Diablo script")
    this.disDiabloTPMsg = new Text("DiabloTPMsg", 500, 100, 3,6,0)
    this.setDiabloTPMsg.addHook(this.disDiabloTPMsg)

    this.KillingDiabloMsg = new TextBox(500, 130, 200, 15, KillingDiabloMsg, 3, 0); this.KillingDiabloMsg.editable = true;
    this.setKillingDiabloMsg = new ConfigSetting("Runs", this.KillingDiabloMsg,"KillingDiabloMsg", "	//	Set your message to announce just before killing Diablo")
    this.disKillingDiabloMsg = new Text("Im Killing Diablo Msg", 500, 130, 3, 6, 0)
    this.setKillingDiabloMsg.addHook(this.disKillingDiabloMsg)

    this.DiabloNewGameMsg = new TextBox(500, 160, 200, 15, DiabloNewGameMsg, 3, 0); this.DiabloNewGameMsg.editable = true;
    this.setDiabloNewGameMsg = new ConfigSetting("Runs", this.DiabloNewGameMsg,"DiabloNewGameMsg", "		//	Set your message to announce as Diablo script is finishing execution")
    this.disDiabloNewGameMsg = new Text("Diablo Died msg", 500, 160, 3, 6, 0)
    this.setDiabloNewGameMsg.addHook(this.disDiabloNewGameMsg)

    this.BaalTPMsg = new TextBox(500, 190, 200, 15, BaalTPMsg, 3, 0); this.BaalTPMsg.editable = true;
    this.setBaalTPMsg = new ConfigSetting("Runs", this.BaalTPMsg,"BaalTPMsg")
    this.disBaalTPMsg = new Text("TP open at Throne msg", 500, 190, 3, 6, 0)
    this.setBaalTPMsg.addHook(this.disBaalTPMsg)

    this.BaalTPSafeMsg = new TextBox(500, 220, 200, 15, BaalTPSafeMsg, 3, 0); this.BaalTPSafeMsg.editable = true;
    this.setBaalTPSafeMsg = new ConfigSetting("Runs", this.BaalTPSafeMsg, "BaalTPSafeMsg")
    this.disBaalTPSafeMsg = new Text("Safe TP at Throne msg", 500, 220, 3, 6, 0)
    this.setBaalTPSafeMsg.addHook(this.disBaalTPSafeMsg)

    this.KillBaalMsg = new TextBox(500, 250, 200, 15, KillBaalMsg, 3, 0); this.KillBaalMsg.editable = true;
    this.setKillBaalMsg = new ConfigSetting("Runs", this.KillBaalMsg, "KillBaalMsg")
    this.disKillBaalMsg = new Text("Lets Kill Baal msg", 500, 250, 3, 6, 0)
    this.setKillBaalMsg.addHook(this.disKillBaalMsg)

    this.BaalNextGameMsg = new TextBox(500, 280, 200, 15, BaalNextGameMsg, 3, 0); this.BaalNextGameMsg.editable = true;
    this.setBaalNextGameMsg = new ConfigSetting("Runs", this.BaalNextGameMsg, "BaalNextGameMsg")
    this.disBaalNextGameMsg = new Text("Baal Died Msg", 500, 280, 3, 6, 0)
    this.setBaalNextGameMsg.addHook(this.disBaalNextGameMsg)

    this.NTConfig_BOMsg = new TextBox(500, 310, 200, 15, NTConfig_BOMsg, 3, 0); this.NTConfig_BOMsg.editable = true;
    this.setNTConfig_BOMsg = new ConfigSetting("Runs", this.NTConfig_BOMsg, "NTConfig_BOMsg")
    this.disNTConfig_BOMsg = new Text("Casting Bo Msg", 500, 310, 3, 6, 0)
    this.setNTConfig_BOMsg.addHook(this.disNTConfig_BOMsg)

    this.SimonNextLevel = new TextBox(620, 100, 50, 15, SimonNextLevel+" ", 3, 0); this.SimonNextLevel.editable = true;this.SimonNextLevel.setKeyVal = true;
    this.setSimonNextLevel = new ConfigSetting("Misc", this.SimonNextLevel, "SimonNextLevel")
    this.disSimonNextLevel = new Text("Simon Next Lvl Key", 620, 100, 3, 6, 0)
    this.setSimonNextLevel.addHook(this.disSimonNextLevel)

    this.SimonPOI = new TextBox(620, 130, 50, 15, SimonPOI + " ", 3, 0); this.SimonPOI.editable = true; this.SimonPOI.setKeyVal = true;
    this.setSimonPOI = new ConfigSetting("Misc", this.SimonPOI, "SimonPOI")
    this.disSimonPOI = new Text("Simon POI Key", 620, 130, 3, 6, 0)
    this.setSimonPOI.addHook(this.disSimonPOI)

    this.SimonPrevousLevel = new TextBox(620, 160, 50, 15, SimonPrevousLevel + " ", 3, 0); this.SimonPrevousLevel.editable = true; this.SimonPrevousLevel.setKeyVal = true;
    this.setSimonPrevousLevel = new ConfigSetting("Misc", this.SimonPrevousLevel, "SimonPrevousLevel")
    this.disSimonPrevousLevel = new Text("Simon Previous lvl Key", 620, 160, 3, 6, 0)
    this.setSimonPrevousLevel.addHook(this.disSimonPrevousLevel)

    this.SimonEscToTown = new TextBox(620, 190, 50, 15, SimonEscToTown + " ", 3, 0); this.SimonEscToTown.editable = true; this.SimonEscToTown.setKeyVal = true;
    this.setSimonEscToTown = new ConfigSetting("Misc", this.SimonEscToTown, "SimonEscToTown")
    this.disSimonEscToTown = new Text("Simon Escape to Town Key", 620, 190, 3, 6, 0)
    this.setSimonEscToTown.addHook(this.disSimonEscToTown)

    this.NT_PauseKey = new TextBox(620, 230, 50, 15, NT_PauseKey + " ", 3, 0); this.NT_PauseKey.editable = true; this.NT_PauseKey.setKeyVal = true;
    this.setNT_PauseKey = new ConfigSetting("Misc", this.NT_PauseKey, "NT_PauseKey")
    this.disNT_PauseKey = new Text("Pause Key", 620, 230, 3, 6, 0)
    this.setNT_PauseKey.addHook(this.disNT_PauseKey)

    j = 120;
    this.NTConfig_GambleIt = new CheckBox(25, j + 15, "Gamble ", NTConfig_GambleIt, true);
    this.Misc = new ConfigSetting("Misc", this.NTConfig_GambleIt,"NTConfig_GambleIt")

    this.NTConfig_UseMerc = new CheckBox(25, 30+j, "Use Merc ", NTConfig_UseMerc, true);
    this.NTConfig_UseMerc = new ConfigSetting("Misc" ,this.NTConfig_UseMerc,"NTConfig_UseMerc","; // Set to true if you use a mercenary, will revive merc at a reviver npc." )

    this.NT_PickUtility = new CheckBox(25, 45+j, "Pick Utility ", NT_PickUtility, true)			
	this.setNT_PickUtility = new ConfigSetting("Misc",this.NT_PickUtility,"NT_PickUtility","; // tries to fill belt and tombs with pickit")

    this.FastSnag = new CheckBox(25, 60+j, "Fast Snag ", FastSnag, true);
    this.setFastSnag= new ConfigSetting("Misc",this.FastSnag,"FastSnag","; // picks after each attack. based on itemdrop event")

    this.NTConfig_SkipHealCurses = new CheckBox(25, j + 75, "Skip Heal for Curses ", NTConfig_SkipHealCurses, true)
    this.setNTConfig_SkipHealCurses = new ConfigSetting("Misc", this.NTConfig_SkipHealCurses, "NTConfig_SkipHealCurses")
		    
    this.NTConfig_OpenChest = new CheckBox(25, j+90, "Open Chest ", NTConfig_OpenChest, true);
    this.setNTConfig_OpenChest = new ConfigSetting("Misc",this.NTConfig_OpenChest,"NTConfig_OpenChest")

    this.NTConfig_ClearPosition = new CheckBox(25, j+105, "Clear Position ", NTConfig_ClearPosition, true);
    this.setNTConfig_ClearPosition = new ConfigSetting("Misc",this.NTConfig_ClearPosition,"NTConfig_ClearPosition")

	this.NT_logItems = new CheckBox(25, j+120, "Log Items ", NT_logItems, true);
	this.setNT_logItems = new ConfigSetting("Misc", this.NT_logItems, "NT_logItems", ";  // enable logging")

	this.NT_IdleOnClone = new CheckBox(25, j + 135, "Idle On Clone", NT_IdleOnClone, true)
	this.setNT_IdleOnClone = new ConfigSetting("Misc", this.NT_IdleOnClone, "NT_IdleOnClone")

	this.NT_IdleOnSOJ = new CheckBox(25, j + 150, "Idle On Soj Count", NT_IdleOnSOJ, true)
	this.setNT_IdleOnSOJ = new ConfigSetting("Misc", this.NT_IdleOnSOJ, "NT_IdleOnSOJ")

	//this.NTConfig_AttackSkill5 = new DropDownBox(200, 300, myGetSkillByID(NTConfig_AttackSkill[5]), 13, 1, this.mySkills, "Secondary Untimed");
	this.NT_HostileAction = new DropDownBox(25, j+180, NT_HostileAction + "    " , 13, 1, ["0 Nothing", "1 Quit    ", "2 Kill   "], "Hostal Action");
	this.NT_HostileAction.valueChanged = function (val) {  	NT_HostileAction = val.substring(0,1)  	}
	this.setNT_HostileAction = new ConfigSetting("Misc", this.NT_HostileAction, "NT_HostileAction")

	this.SimonFollowMode = new CheckBox(25, j + 265, "Simon Fight with Lead", SimonFollowMode, true)
	this.setSimonFollowMode = new ConfigSetting("Misc", this.SimonFollowMode, "SimonFollowMode","       // Fight while folowing ")

	this.SimonEnableSnagging = new CheckBox(25, j + 205, "Simon Enable Snagging", SimonEnableSnagging, true)
	this.setSimonEnableSnagging = new ConfigSetting("Misc", this.SimonEnableSnagging, "SimonEnableSnagging")

	this.SimonAutoReveal = new CheckBox(25, j + 220, "Simon Auto Reveal", SimonAutoReveal, true)
	this.setSimonAutoReveal = new ConfigSetting("Misc", this.SimonAutoReveal, "SimonAutoReveal")

	this.SimonStopLifeWatch = new CheckBox(25, j + 235, "Simon Stop Life Watch", SimonStopLifeWatch, true)
	this.setSimonStopLifeWatch = new ConfigSetting("Misc", this.SimonStopLifeWatch, "SimonStopLifeWatch")

	this.SimonEnableCommands = new CheckBox(25, j + 250, "Simon Listen to Commands", SimonEnableCommands, true)
	this.setSimonEnableCommands = new ConfigSetting("Misc", this.SimonEnableCommands, "SimonEnableCommands")

	// no idea why this dosent work
	this.useTeleport = new CheckBox(25, j + 280, "me.useTeleport", me.useTeleport, true)
	this.setuseTeleport = new ConfigSetting("Misc", this.useTeleport,"useTeleport","					//	Set to true to allow character to use teleport when moving, set to false to disable teleport when moving")

	this.bobDebug = new CheckBox(300, j + 280, "debug to OOG", bobDebug, true)
	this.setbobDebug = new ConfigSetting("Misc", this.bobDebug,"bobDebug"," 		//Set to true to send extra debug information to OOG (good to try if you are having problems)")

	//this.NTConfig_Colums= new ConfigSetting("Misc",this.NTConfig_Colums )		
	//NTConfig_Columns[0] = [1, 1, 1, 1, 0, 0, 0, 0, 0, 0];		// just manually resaving this based on  NTConfig_ColumnsHooks instead of hooking functions to val

	if (NTCreatedConfig) 
		NTConfig_Columns = NTT_CheckInventory(true)
		for (var c = 0; c < 10; c++) {
				for (var r = 0; r < 4; r++) {
				if (NTCreatedConfig) 
					this.hook = new CheckBox(30 + (c * 13), 430 + r * 13, "", NTConfig_Columns[r][c] ==1 ? 0 :1 )
				else
					this.hook = new CheckBox(30 + (c * 13), 430 + r * 13, "", NTConfig_Columns[r][c])
				this.setNT_logItems.addHook(this.hook)
				NTConfig_ColumnsHooks.push(this.hook)
			}	
		}
	
		
	
	this.configDiscript = new Text("Inventory. Black = Wont Stash / Sell", 30, 425, 18, 6, 0)
	this.setNT_logItems.addHook(this.configDiscript)

	var nipfiles= dopen('settings/').getFiles();
	this.NTConfig_NIPFilePath = new LinkedListBoxes(200,140,175,200,nipfiles,NTConfig_NIPFilePath,2)
	this.setNTConfig_NIPFilePath = new ConfigSetting("Misc", this.NTConfig_NIPFilePath, "NTConfig_NIPFilePath")
	this.nipDis = new Text("Included Nip Files",200 + 175 +50,140,21,6,0); this.setNTConfig_NIPFilePath.addHook(this.nipDis)
	//lifeWatch
	j=30
	this.NTConfig_LifeThresh = new SliderWText (300, 100+j,"Drink Hp pot at",100,NTConfig_LifeThresh,this.textColor,4)
	this.setNTConfig_LifeThresh = new ConfigSetting("LifeWatch", this.NTConfig_LifeThresh,"NTConfig_LifeThresh")

	this.NTConfig_LifeRejuvThresh = new SliderWText(300, 130 + j, "Drink rejuve for HP at", 100, NTConfig_LifeRejuvThresh, this.textColor, 4);
	this.setNTConfig_LifeRejuvThresh = new ConfigSetting("LifeWatch", this.NTConfig_LifeRejuvThresh,"NTConfig_LifeRejuvThresh")

	this.NTConfig_ManaThresh = new SliderWText(300, 160 + j, "Drink Mana pot at", 100, NTConfig_ManaThresh, this.textColor, 4)
	this.setNTConfig_ManaThresh = new ConfigSetting("LifeWatch", this.NTConfig_ManaThresh,"NTConfig_ManaThresh")

	this.NTConfig_ManaRejuvThresh = new SliderWText(300, 190 + j, "Drink rejuve for mana at", 100, NTConfig_ManaRejuvThresh, this.textColor, 4)
	this.setNTConfig_ManaRejuvThresh=new ConfigSetting("LifeWatch",this.NTConfig_ManaRejuvThresh,"NTConfig_ManaRejuvThresh")

	this.NTConfig_LifeChicken = new SliderWText(300, 220 + j, "Chicken when Life is", 100, NTConfig_LifeChicken, this.textColor, 4)
	this.setNTConfig_LifeChicken = new ConfigSetting("LifeWatch", this.NTConfig_LifeChicken,"NTConfig_LifeChicken")

	this.NTConfig_ManaChicken = new SliderWText(300, 250 + j, "Chiken when Mana is", 100, NTConfig_ManaChicken, this.textColor, 4)
	this.setNTConfig_ManaChicken = new ConfigSetting("LifeWatch", this.NTConfig_ManaChicken,"NTConfig_ManaChicken")

	this.NTConfig_MercLifeThresh = new SliderWText(300, 280 + j, "Merc Hp Pot at", 100, NTConfig_MercLifeThresh, this.textColor, 4);
	this.setNTConfig_MercLifeThresh = new ConfigSetting("LifeWatch", this.NTConfig_MercLifeThresh,"NTConfig_MercLifeThresh")

	this.NTConfig_MercRejuvThresh = new SliderWText(300, 310 + j, "Merc Rejuve at", 100, NTConfig_MercRejuvThresh, this.textColor, 4)
	this.setNTConfig_MercRejuvThresh=new ConfigSetting("LifeWatch",this.NTConfig_MercRejuvThresh,"NTConfig_MercRejuvThresh")


	this.NTConfig_MercChicken = new SliderWText(300, 340 + j, "Chicken when Merc is", 100, NTConfig_MercChicken, this.textColor, 4)
	this.setNTConfig_MercChicken = new ConfigSetting("LifeWatch", this.NTConfig_MercChicken,"NTConfig_MercChicken")

	this.NTConfig_SkipHealLife = new SliderWText(50, 130, "heal HP at npc at", 100, NTConfig_SkipHealLife, this.textColor, 4)
	this.setNTConfig_SkipHealLife = new ConfigSetting("LifeWatch", this.NTConfig_SkipHealLife, "NTConfig_SkipHealLife", "; // If you have more than this percent of life, you won't go to a healer")
	
	this.NTConfig_SkipHealMana =  new SliderWText(50, 160, "heal MP at npc at", 100, NTConfig_SkipHealMana, this.textColor, 4)
	this.setNTConfig_SkipHealMana = new ConfigSetting("LifeWatch", this.NTConfig_SkipHealMana, "NTConfig_SkipHealMana", "; // If you have more than this percent of mana, you won't go to a healer")

	this.NTConfig_CheckSafe = new CheckBox(50, 190, "Safe Check ", NTConfig_CheckSafe, true);
	this.setNTConfig_CheckSafe = new ConfigSetting("LifeWatch", this.NTConfig_CheckSafe, "NTConfig_CheckSafe")

	this.mySkills =["Nothing"];
	
	var usableSkills =[5,6,7,8,10,11,12,14,15,16,17,19,20,21,22,24,25,26,27,28,30,31,32,34,35,36,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,62,64,66,67,68,70,71,72,73,74,75,76,77,78,80,81,82,83,84,85,86,87,88,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,130,131,132,133,134,135,137,138,139,140 ,142,143,144,146,147,149,150,151,152,154,155,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250, 251,253,254,255,256,257,258,259,260,261,262,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281]
	for (var a = usableSkills.length-1; a >0; a =a -1)
		if (me.getSkill(usableSkills[a],1) >0)
			this.mySkills.push(getSkillById(usableSkills[a]))
	if (me.classid == 1 || me.classid ==5 || me.classid == 6){ //sorc 
		this.NTConfig_AttackSkill0 = new DropDownBox(50, 150, myGetSkillByID(NTConfig_AttackSkill[0]), 13, 1, this.mySkills, "Cast First");
		this.NTConfig_AttackSkill0.valueChanged = function (val) { NTConfig_AttackSkill[0] = myGetSkillByName(val) }
		this.setNTConfig_AttackSkill0 = new ConfigSetting("Skills", this.NTConfig_AttackSkill0,"NTConfig_AttackSkill[0]","   // Cast your first spell once. Set to 0 if you won't ")

		this.NTConfig_AttackSkill1 = new DropDownBox(200, 150, myGetSkillByID(NTConfig_AttackSkill[1]), 13, 1, this.mySkills, "Primary Boss");
		this.NTConfig_AttackSkill1.valueChanged = function (val) { NTConfig_AttackSkill[1] = myGetSkillByName(val) }
		this.setNTConfig_AttackSkill1 = new ConfigSetting("Skills",this.NTConfig_AttackSkill1,"NTConfig_AttackSkill[1]","   // Cast your primary spell to boss. ")

		this.NTConfig_AttackSkill2 = new DropDownBox(350, 150, myGetSkillByID(NTConfig_AttackSkill[2]), 13, 1, this.mySkills, "Primary For Others");
		this.NTConfig_AttackSkill2.valueChanged = function (val) { NTConfig_AttackSkill[2] = myGetSkillByName(val) }
		this.setNTConfig_AttackSkill2 = new ConfigSetting("Skills", this.NTConfig_AttackSkill2,"NTConfig_AttackSkill[2]","   // Cast your primary spell to others. ")

		this.NTConfig_AttackSkill3 = new DropDownBox(500, 150,myGetSkillByID(NTConfig_AttackSkill[3]),13,1,this.mySkills,"Primary Untimed");
		this.NTConfig_AttackSkill3.valueChanged = function (val) { NTConfig_AttackSkill[3] = myGetSkillByName(val) }
		this.setNTConfig_AttackSkill3 = new ConfigSetting("Skills", this.NTConfig_AttackSkill3,"NTConfig_AttackSkill[3]","   // Cast your primary untimed spell if primary spell is timed spell. Set to -1 if you won't" )

		this.NTConfig_AttackSkill4 = new DropDownBox(50, 300, myGetSkillByID(NTConfig_AttackSkill[4]), 13, 1, this.mySkills, "Secondary");
		this.NTConfig_AttackSkill4.valueChanged = function (val) { NTConfig_AttackSkill[4] = myGetSkillByName(val) }
		this.setNTConfig_AttackSkill4 = new ConfigSetting("Skills", this.NTConfig_AttackSkill4,"NTConfig_AttackSkill[4]","   // Cast your secondary spell if monster is immune to your primary spell. Set to 0 if you won't ")

		this.NTConfig_AttackSkill5 = new DropDownBox(200, 300,myGetSkillByID(NTConfig_AttackSkill[5]),13,1,this.mySkills,"Secondary Untimed");
		this.NTConfig_AttackSkill5.valueChanged = function (val) { NTConfig_AttackSkill[5] = myGetSkillByName(val) }
		this.setNTConfig_AttackSkill5 = new ConfigSetting("Skills", this.NTConfig_AttackSkill5,"NTConfig_AttackSkill[5]","   // Cast your secondary untimed spell if secondary spell is timed spell. Set to 0 if you won't")

		if (me.classid == 1) {
			this.NTConfig_CastStatic = new SliderWText(350, 300, "Cast Static ", 100, NTConfig_CastStatic, this.textColor, 4);
			this.setNTConfig_CastStatic = new ConfigSetting("Skills",this.NTConfig_CastStatic,"NTConfig_CastStatic")
		}
		if (me.classid == 6) { //sin
			this.NTConfig_UseTraps = new CheckBox(350, 300, "Use Traps", NTConfig_UseTraps,true);
			this.setNTConfig_UseTraps = new ConfigSetting("Skills", this.NTConfig_UseTraps,"NTConfig_UseTraps")

			this.NTConfig_CastShadowMaster = new CheckBox(350, 315, "Safe Check ", NTConfig_CastShadowMaster,true);
			this.setNTConfig_CastShadowMaster = new ConfigSetting("Skills", this.NTConfig_CastShadowMaster,"NTConfig_CastShadowMaster")

			this.NTConfig_CastShadowWarrior = new CheckBox(350, 330, "Use Shadow Master ", NTConfig_CastShadowWarrior,true);
			this.setNTConfig_CastShadowWarrior = new ConfigSetting("Skills", this.NTConfig_CastShadowWarrior,"NTConfig_CastShadowWarrior")

			this.NTConfig_CastBrustOfSpeed = new CheckBox(350, 345, "Use Burst Of Speed ", NTConfig_CastBrustOfSpeed,true);
			this.setNTConfig_CastBrustOfSpeed = new ConfigSetting("Skills", this.NTConfig_CastBrustOfSpeed, "NTConfig_CastBrustOfSpeed")
			
			this.NTConfig_CastFade = new CheckBox(350, 360, "Cast Fade ", NTConfig_CastFade,true); 
			this.setNTConfig_CastFade = new ConfigSetting("Skills", this.NTConfig_CastFade, "NTConfig_CastFade")
			
			this.NTConfig_CastBladeShield = new CheckBox(350, 375, "Cast Blade Sheild ", NTConfig_CastBladeShield,true);
			this.setNTConfig_CastBladeShield = new ConfigSetting("Skills", this.NTConfig_CastBladeShield, "NTConfig_CastBladeShield")
			
			this.NTConfig_CastCloakOfShadows = new CheckBox(350, 390, "Cast Cloak of Shadows ", NTConfig_CastCloakOfShadows,true); 
			this.setNTConfig_CastCloakOfShadows = new ConfigSetting("Skills", this.NTConfig_CastCloakOfShadows, "NTConfig_CastCloakOfShadows")
			
		}
	
	}
	if (me.classid == 3 || me.classid == 2){ // Pally n necro shared
		this.NTConfig_AttackBoss = new DropDownBox(100, 140,myGetSkillByID(NTConfig_AttackBoss),13,1,this.mySkills,"Boss Skill")
		this.setNTConfig_AttackBoss = new ConfigSetting("Skills", this.NTConfig_AttackBoss, "NTConfig_AttackBoss")
		
		this.NTConfig_AttackOthers = new DropDownBox(300, 140,myGetSkillByID(NTConfig_AttackOthers),13,1,this.mySkills,"Primary For Others");
		this.setNTConfig_AttackOthers = new ConfigSetting("Skills", this.NTConfig_AttackOthers, "NTConfig_AttackOthers")
		
	}
	
	if (me.classid == 2){ // Necro
		this.XP_Curse = new DropDownBox(500, 180,myGetSkillByID(XP_Curse),13,1,this.mySkills,"Curse after Army is Made");
		this.setXP_Curse = new ConfigSetting("Skills", this.XP_Curse, "XP_Curse")
		
		this.XP_BuildArmyCurse = new DropDownBox(500, 220,myGetSkillByID(XP_BuildArmyCurse),13,1,this.mySkills,"Curse to Build Army");
		this.setXP_BuildArmyCurse = new ConfigSetting("Skills", this.XP_BuildArmyCurse, "XP_BuildArmyCurse")
		
		this.XP_Golm = new SliderWText (500, 245,"0=clay 1=blood, 2=fire, 3=iron",3,XP_Golm,this.textColor,4)
		this.setXP_Golm = new ConfigSetting("Skills", this.XP_Golm, "XP_Golm")
		
		this.XP_BuildArmyThresh = new SliderWText (500, 275,"Build Army Threshold",20,XP_BuildArmyThresh,this.textColor,4) 
		this.setXP_BuildArmyThresh = new ConfigSetting("Skills", this.XP_BuildArmyThresh, "XP_BuildArmyThresh")
		
		this.XP_useSkel = new CheckBox(500, 295,"Make Skeletons ",XP_useSkel,true)	
		this.setXP_useSkel = new ConfigSetting("Skills", this.XP_useSkel, "XP_useSkel")
		
		this.XP_useSkelMage = new CheckBox(500, 310,"Make Mages ",XP_useSkelMage,true)	
		this.setXP_useSkelMage = new ConfigSetting("Skills", this.XP_useSkelMage, "XP_useSkelMage")
		
		this.XP_useRevive = new CheckBox(500, 325,"Make Revives ",XP_useRevive,true)	; 	
		this.setXP_useRevive = new ConfigSetting("Skills", this.XP_useRevive, "XP_useRevive")
		
		this.XP_CorpseExplosion = new CheckBox(500, 340,"Use Corpse Explosion",XP_CorpseExplosion,true)
		this.setXP_CorpseExplosion = new ConfigSetting("Skills", this.XP_CorpseExplosion, "XP_CorpseExplosion")		
	}	
	if (me.classid == 3){ // Pally
		this.NTConfig_AttackFirst = new DropDownBox(500, 190,myGetSkillByID(NTConfig_AttackFirst),13,1,this.mySkills,"Cast First");
		this.setNTConfig_AttackFirst = new ConfigSetting("Skills", this.NTConfig_AttackFirst, "NTConfig_AttackFirst")
		
		this.NTConfig_AttackSecondary = new DropDownBox(500, 220,myGetSkillByID(NTConfig_AttackSecondary),13,1,this.mySkills,"Secondary For Immunes");
		this.setNTConfig_AttackSecondary = new ConfigSetting("Skills", this.NTConfig_AttackSecondary, "NTConfig_AttackSecondary")
		
		this.NTConfig_PutAura = new DropDownBox(500, 260,myGetSkillByID(NTConfig_PutAura),13,1,this.mySkills,"Attack Aura");
		this.setNTConfig_PutAura = new ConfigSetting("Skills", this.NTConfig_PutAura, "NTConfig_PutAura")
		
		this.NTConfig_UseRedemption = new CheckBox(500, 275,"Use Redemption ",NTConfig_UseRedemption,true)	;
		this.setNTConfig_UseRedemption = new ConfigSetting("Skills", this.NTConfig_UseRedemption, "NTConfig_UseRedemption")			
	}
	
	//this.test = new DropDownBox(600, 100,"Test",13,1,this.mySkills);
	this.NTConfig_HelpPreClear = new CheckBox(25, 315, "Help Leader Clear Thone before SafeMsg", NTConfig_HelpPreClear, true)
	this.setNTConfig_HelpPreClear = new ConfigSetting("Leech", this.NTConfig_HelpPreClear, "NTConfig_HelpPreClear","			//	Set to true if you want to help clear the throne before it's announced 'Safe'")

	this.NTConfig_HelpWithWaves = new CheckBox(25, 330, "Help Clear Baal's Minios", NTConfig_HelpWithWaves, true)
	this.seNTConfig_HelpWithWaves = new ConfigSetting("Leech", this.NTConfig_HelpWithWaves, "NTConfig_HelpWithWaves", "			//	Set to true to fight in throne when each wave drops")

	this.NTConfig_HelpWithBaal = new CheckBox(25, 345, "Help kill Baal", NTConfig_HelpWithBaal, true)
	this.setNTConfig_HelpWithBaal = new ConfigSetting("Leech", this.NTConfig_HelpWithBaal, "NTConfig_HelpWithBaal", "			//	Set to true if you want to help kill Baal")

	this.NTConfig_GetBaalItems = new CheckBox(25, 360, "Get Baal Items", NTConfig_GetBaalItems, true)
	this.setNTConfig_GetBaalItems = new ConfigSetting("Leech", this.NTConfig_GetBaalItems, "NTConfig_GetBaalItems", "			//	Set to true if you want to pick items after Baal dies")

	this.NTConfig_ClearLeechPosition = new CheckBox(25, 375, "clear safe position in baal chamber", NTConfig_ClearLeechPosition, true)
	this.setNTConfig_ClearLeechPosition = new ConfigSetting("Leech", this.NTConfig_ClearLeechPosition, "NTConfig_ClearLeechPosition","		//	Set to true if you want to fight while leeching in Worldstone Chamber (tentacles could appear etc)")

	var fullLeaders = []
	for (var l = 0; l < NT_Leader.length; l++) { fullLeaders.push(NT_Leader[l]) }
	for (var q = 0; q < QuitWithLeaderList.length; q++) { fullLeaders.push(QuitWithLeaderList[q]) }
	var a = getParty();
	if (a) {
		do {
			if (a.name != "") fullLeaders.push (a.name)
		} while (a.getNext())
	}
	NT_Leader=removeBlanksFromArray(NT_Leader)
	fullLeaders=removeBlanksFromArray(fullLeaders)
	QuitWithLeaderList=removeBlanksFromArray(QuitWithLeaderList)
	this.NT_Leader = new LinkedListBoxes(50, 140, 200, 75, fullLeaders, NT_Leader, 2)
	this.setNT_Leader = new ConfigSetting("Leech", this.NT_Leader, "NT_Leader")
	this.nipDis = new Text("Leader List", 50 + 200 + 50, 140, 21, 6, 0); this.setNT_Leader.addHook(this.nipDis)

	this.QuitWithLeaderList = new LinkedListBoxes(50, 230, 200, 75, fullLeaders, QuitWithLeaderList, 2)
	this.setQuitWithLeaderListr = new ConfigSetting("Leech", this.QuitWithLeaderList, "QuitWithLeaderList")
	this.nipDis = new Text("Quit With Lead List", 50 + 200 + 50, 230, 21, 6, 0); this.setNT_Leader.addHook(this.nipDis)
	

	var cmdOffset = (me.screensize == 2) ?500 : 30;
		
	
	
	this.cmdExit = new TextBox(450,cmdOffset,130,40," Exit");	this.cmdExit.visible=false; this.cmdExit.font =3;
	this.cmdExit.parent = this; ShowConfigHooks.push(this.cmdExit) ;this.cmdExit.ShowOutline = false;
	this.cmdExit.clickFunction = function(){		
		this.parent.setVisible(false)		
	}
	
	this.cmdSave = new TextBox(200,cmdOffset,130,40," Save");	this.cmdSave.visible=false; this.cmdSave.font =3;
	this.cmdSave.parent = this; ShowConfigHooks.push(this.cmdSave) ;this.cmdSave.ShowOutline = false;
//Save vars to config ////////////////////////////
	this.cmdSave.clickFunction = function () {
		this.parent.saveVars()
		SaveConfig();
		this.parent.setVisible(false)
	}


		this.tabs = []
		var newconfig = true
		for (var c = 0; c < ConfigObjects.length; c++) {
			newconfig = true
			for (var t = 0; t < this.tabs.length; t++) {
				if (this.tabs[t] == ConfigObjects[c].Category)
					newconfig = false
			}
			if (newconfig) this.tabs.push(ConfigObjects[c].Category)
		}
		this.MainTabControl = new TabControl(20, 80, 780, 500, 0, 28, this.tabs)
		this.MainTabControl.tabChanged = function (oldText, newText) {
			for (var c = 0; c < ConfigObjects.length; c++) {
				if (ConfigObjects[c].Category == oldText)				
					for (var t = 0; t < ConfigObjects[c].Hooks.length; t++)
						ConfigObjects[c].Hooks[t].visible = false;
				if (ConfigObjects[c].Category == newText)				
					for (var t = 0; t < ConfigObjects[c].Hooks.length; t++)
						ConfigObjects[c].Hooks[t].visible = true;				
		
			 }
		};
								 
		  this.MainTabControl.box.opacity =2
		  this.MainTabControl.box.color = 0
		  this.MainTabControl.box.click = function () {return true; };


}


ShowConfig.prototype.setVisible = function (vis) {
	this.MainTabControl.visible = vis;
	for (var c = 0; c < ConfigObjects.length; c++) {
		if (ConfigObjects[c].Category == this.MainTabControl.selectedText)
			for (var t = 0; t < ConfigObjects[c].Hooks.length; t++)
				ConfigObjects[c].Hooks[t].visible = vis;
		

	}

	for (var j = 0 ; j < ShowConfigHooks.length; j ++){	
		ShowConfigHooks[j].visible=vis;
	}	
	
	


}
ShowConfig.prototype.saveVars = function () {
	// everything is linked to global and saved on change
}
function SaveConfig(){
print("Saving Config")

	var configVersion = 2.2;
	var filehandle = File.open('libs/config/NTConfig_'+NTC_CharClassToNameList[me.classid]+"_"+me.name+".dbl", 2);
	var linecount = 0;
	var templine ="";
	var content = '//Universal Config v'+ configVersion;	
	var firstline =filehandle.readLine();  
		if (firstline != content){
		//print(firstline +"!="+content)
			filehandle.close();
			filehandle = File.open("libs/config/NTConfig_Any.dbl", 2);
			firstline =filehandle.readLine();
		}
		var counter = 0; // double array save easyiest just to do it here
		for (var c = 0; c < 10; c++) { 
			for (var r = 0; r < 4; r++) {
				NTConfig_Columns[r][c] = NTConfig_ColumnsHooks[counter].checked ? 1 : 0
				counter++;
				
			}
		}
		
	content= content+"\n"	
	while(!filehandle.eof) {
		templine = filehandle.readLine();
		if (templine.indexOf("NTConfig_Columns[0] =") > -1) templine = "\t" + "NTConfig_Columns[0] = " + NTConfig_Columns[0].toSource()
		if (templine.indexOf("NTConfig_Columns[1] =") > -1) templine = "\t" + "NTConfig_Columns[1] = " + NTConfig_Columns[1].toSource()
		if (templine.indexOf("NTConfig_Columns[2] =") > -1) templine = "\t" + "NTConfig_Columns[2] = " + NTConfig_Columns[2].toSource()
		if (templine.indexOf("NTConfig_Columns[3] =") > -1) templine = "\t" + "NTConfig_Columns[3] = " + NTConfig_Columns[3].toSource()
		
		for (var j = 0; j < ConfigObjects.length; j++) {
			if (templine.indexOf(ConfigObjects[j].globalVar + " = ") > -1) {
				if (typeof (this[ConfigObjects[j].globalVar]) == 'object') {
					templine = "\t" + ConfigObjects[j].globalVar + " = " + this[ConfigObjects[j].globalVar].toSource() + (ConfigObjects[j].Comment ? ConfigObjects[j].Comment : ";")
				}else if (typeof (this[ConfigObjects[j].globalVar]) == 'string') {
					templine = "\t" + ConfigObjects[j].globalVar + ' = "' + this[ConfigObjects[j].globalVar] +'"' + (ConfigObjects[j].Comment ? ConfigObjects[j].Comment : ";")

				} else {
					if (ConfigObjects[j].globalVar.indexOf("[") > -1) {	   // huge hack to eval array vars
						var s = ConfigObjects[j].globalVar
						if(typeof(this[s.substring(0, s.indexOf("["))][s.substring(s.indexOf("[") + 1, s.length - 1)]) == 'object'){
							   templine = "\t" + ConfigObjects[j].globalVar + " = " + this[s.substring(0, s.indexOf("["))][s.substring(s.indexOf("[") + 1, s.length - 1)].toSource() + (ConfigObjects[j].Comment ? ConfigObjects[j].Comment : ";")
						}else {
							   templine = "\t" + ConfigObjects[j].globalVar + " = " + this[s.substring(0, s.indexOf("["))][s.substring(s.indexOf("[") + 1, s.length - 1)] + (ConfigObjects[j].Comment ? ConfigObjects[j].Comment : ";")
						}
							 } else {
						templine = "\t" + ConfigObjects[j].globalVar + " = " + this[ConfigObjects[j].globalVar] + (ConfigObjects[j].Comment ? ConfigObjects[j].Comment : ";")
					}
				}

			}

		}
        if (templine.indexOf("useTeleport = ") > -1) templine = "\t" + "me.useTeleport = " + useTeleport + "					//	Set boobs to true to allow character to use teleport when moving, set to false to disable teleport when moving;"
		content += (templine) +"\n" ;
//		 this.globalVar = globalVar;
//	this.Comment = Comment
	}
	filehandle.close();
	
	 FileTools.writeText('libs/config/NTConfig_'+NTC_CharClassToNameList[me.classid]+"_"+me.name+".dbl", content); 
  
   return true;

}


function myGetSkillByID(id){
if (id == -1 || id == undefined)
	return "Nothing"
return getSkillById(id)
}

function myGetSkillByName(name){
if (!name)
	return -1;
if (name == "Nothing")
	return -1;
return getSkillByName(name)
}


//alogwe: added these  functions to read valid script files from folder for onscreen config;
function getBossList()
{
	var list = getScriptsInFolder('libs/bots/');	//populate boss list with valid scripts from 'libs/bots/' folder;
	list.toScriptAliases();							//convert bossList elements from filenames to script aliases (for putting in char config);
	//print(list.toSource());						//debugging;
	
	return list;
}


function getScriptsInFolder(folderPath)
{
	if(arguments.length == 0 || typeof(folderPath) != 'string' || !(FileTools.exists(folderPath)))
		return false;
		
	var filetypes = ['dbl','DBL'];	//Array of valid filetypes to filter the read-in file list;
	var fileArray = [];				//Array to hold function output (file list);
	
	
	function validNTBotScripts(element, index, array)
	{
		var searchString = '';    //Create RegExp search string to test the array elements with;	
		for(var i = 0; i < filetypes.length; i++)
		{
			searchString = '^NT.+\\.' + filetypes[i];
			if((typeof(element) == 'string') && RegExp(searchString, 'g').test(element) && element != 'NTLoader.dbl')
				return true;
		}
		return false;
	}
	
	
	//get all filenames in folderPath;
	fileArray = dopen(folderPath).getFiles();
	
	//filter array to only keep filenames with valid file extensions (set above in 'filetypes' array);
	fileArray = fileArray.filter(validNTBotScripts);
	
	return fileArray;
}


//alogwe: this will remove file extension regardless of filetype
Array.prototype.toScriptAliases =
	function()
	{
		for(var i = 0; i < this.length; i++)
		{
			if(typeof(this[i]) == 'string' && this[i].indexOf('NT') == 0)
				this[i] = this[i].substring(2, this[i].lastIndexOf('.'));
		}
	};
	function removeBlanksFromArray(array) {
		for (var j = 0; j < array.length; j++) {
		  if (array[j]=="") array.remove(j)
		 }
	return array
	}


	

