
function getArea(id){
/// <summary>returns an area object</summary>
/// <param name="id" type="Number">Optional area id, blank = current area.</param>
    return new Area();
}

function Area (){
	/// <summary>Area Object.</summary>
}
Area.prototype.x =function(){
/// <summary>area.x * 5 = game coords</summary>
}
Area.prototype.exits =function(){
	/// <summary>Array of exits</summary>
	 var ret = [] // mock fill array
    var p = new exits
    for (var j = 0; j < 10000; j++) {
        ret.push(p)
    }
    return ret
 }

Area.prototype.name =function(){}
Area.prototype.xsize =function(){
/// <summary>area.xsize * room.xsize = area width</summary>
}
Area.prototype.y =function(){
/// <summary>area.y * 5 = game coords</summary>
}
Area.prototype.ysize =function(){
/// <summary>area.ysize * room.ysize = area height</summary>
}
Area.prototype.id =function(){}

function Control (){
/// <summary>Out of game control object.</summary>
}
Control.prototype.text = function(){}
Control.prototype.x = function(){}
Control.prototype.y = function(){}
Control.prototype.xsize = function(){}
Control.prototype.ysize = function(){}
Control.prototype.state = function(){}
Control.prototype.password = function(){}
Control.prototype.type = function(){}
Control.prototype.cursorpos = function(){}
Control.prototype.selectstart = function(){}
Control.prototype.selectend = function(){}
Control.prototype.disabled = function(){}
Control.prototype.getNext = function(){
return new Control()
}
Control.prototype.click = function(){}
Control.prototype.setText = function(){}
Control.prototype.getText = function(){
/// <summary>returns an array of text for multi lined controls</summary>
}
function getControl(type, x, y, xsize, ysize ){
/// <summary>returns a control object, all prams optinal </summary>
return new Control()
}
function print(text){
/// <summary>prints text to console.</summary>
}
function delay(milliseconds){}
function load(path){
/// <summary>loads a script on its own thread</summary>
/// <param name="path" type="String">path is relitive to scriptPath/ ex. "tools/lifewatch.js </param>
}
function include(path){
/// <summary>includes a file on the current thread</summary>
/// <param name="path" type="String">path is relitive to scriptpath/lib/ folder ex. "common/NTAreas.dbl</param>
}
function stop(OnlyCurrentScript){
/// <summary>stops running scripts, if true is passed only current script is stopped</summary>
/// <param name="OnlyCurrentScript" type="String">true to stop current script</param>
}
function beep(type){
/// <summary>plays a system sound</summary>
/// <param name="type" type="Number">0= Simple, 1 = Asterick, 2 = Exclamation, 3=Hand, 4=Question,5 = Default</param>
}
function blockMinimize(setTrueOrFalse){}
function getTickCount(){
/// <summary>returns system ticks. as number</summary>
}
function getThreadPriority(){
/// <summary>returns the current thread priority</summary>
}
function isIncluded(path){
/// <summary>returns true / false if path is already included.</summary>
/// <param name="path" type="String">path is relitive to scriptpath/lib/ folder ex. "common/NTAreas.dbl</param>
}
function version(){
/// <summary>returns d2bs version string</summary>
}
function debugLog(logline1,logline2){
/// <summary>writes prams to the log file, unlimited pram lines</summary>
}
function sendCopyData(windowClassName, windowName, nModeId, data){
/// <summary>sendmessage copy data to specified window</summary>
/// <param name="windowClassName" type="String">null for window title</param>
/// <param name="windowName" type="String">window title receiving message</param>
/// <param name="nModeId" type="String">copydata mode / id</param>
/// <param name="data" type="String">string of data to send to window</param>
}
function sendDDE(mode, pszDDEServer, pszTopic, pszItem, pszData){
/// <param name="mode" type="String">0 - XTYP_REQUEST, 1 - XTYP_POKE, 2 - XTYP_EXECUTE</param>
}
function keystate(keyCode){}
function addEventListener(eventName,myFunction){
/// <summary>hooks your function to an event</summary>
/// <param name="eventName" type="String">ChatMsg,CopyData,GameEvent,GameMsg,GoldDrop,ItemAction, KeyDown,KeyUp,MeLife,MeMana,MouseClick,PlayerAssign,ScriptMsg,WhisperMsg</param>
/// <param name="myFunction" type="String">my function to get called, without ()</param>
}
function removeEventListener(eventName,myFunction){
/// <summary>unhooks your function to an event</summary>
/// <param name="eventName" type="String">ChatMsg,CopyData,GameEvent,GameMsg,GoldDrop,ItemAction, KeyDown,KeyUp,MeLife,MeMana,MouseClick,PlayerAssign,ScriptMsg,WhisperMsg</param>
/// <param name="myFunction" type="String">my function to get called, without ()</param>
}
function clearEvent(eventName){
/// <summary>unhooks events by name</summary>
/// <param name="eventName" type="String">ChatMsg,CopyData,GameEvent,GameMsg,GoldDrop,ItemAction, KeyDown,KeyUp,MeLife,MeMana,MouseClick,PlayerAssign,ScriptMsg,WhisperMsg</param>
}
function clearAllEvents(){}
function js_strict(disableWarnings){}
function scriptBroadcast(message){}
function showConsole(){}
function hideConsole(){}

function dir(){
/// <summary>dir object</summary>
}
dir.prototype.name =function (){}
dir.prototype.create =function (name){}
dir.prototype.remove =function (name){}
dir.prototype.getFiles =function (filter){
/// <param name="filter" type="String">blank = *.* </param>
}
dir.prototype.getFolders =function (){}

function exits(){
/// <summary>exit object</summary>
}
exits.prototype.x = function(){}
exits.prototype.y = function(){}
exits.prototype.target = function(){
/// <summary>exits next area</summary>
}
exits.prototype.type = function(){
/// <summary>1 = walkable, 2 = tile</summary>
}
exits.prototype.tileid = function(){}
exits.prototype.level = function(){
/// <summary>current area</summary>
}

function dopen(path){
    return new dir()
}

function file(){
/// <summary>file object</summary>
}
file.prototype.readable =function(){}
file.prototype.writeable =function(){}
file.prototype.seekable =function(){}
file.prototype.mode =function(){}
file.prototype.binaryMode =function(){}
file.prototype.length =function(){}
file.prototype.path =function(){}
file.prototype.eof =function(){}
file.prototype.accessed =function(){}
file.prototype.modified =function(){}
file.prototype.autoflush =function(){}

file.prototype.close =function(){}
file.prototype.reopen =function(){}
file.prototype.read =function(){}
file.prototype.readLine =function(){}
file.prototype.readAllLines =function(){}
file.prototype.readAll =function(){}
file.prototype.write =function(){}
file.prototype.seek =function(bytes,isLines,fromStart){}
file.prototype.flush =function(){}
file.prototype.reset =function(){}
file.prototype.end =function(){}

function FileTools(){}
FileTools.prototype.remove =function(path){}
FileTools.prototype.rename =function(oldname,newname){}
FileTools.prototype.copy = function(orignal,copy){}
FileTools.prototype.exists = function(path){}
FileTools.prototype.readText = function(path){}
FileTools.prototype.writeText = function(path,object){}
FileTools.prototype.appendText = function(path,string){}


function File(){}
File.prototype.open = function(path,mode,binary,autoflush,lockFile){
/// <summary>Opens and returns a file object needs first 2 prams</summary>
/// <param name="path" type="String">path relitive to /scriptPath</param>
/// <param name="mode" type="String">0 = Read, 1 = Write, 2 = Append</param>
return new file()
}

function copyUnit(unit){
return new Unit()
}
function clickMap(clickType, shiftKey, unitObject_OR_x, y){
/// <summary>calls d2 click function</summary>
/// <param name="clickType" type="Number">0 left down, 1 left hold, 2 left up, 3 right down, 4 right hold, 5 right up</param>
/// <param name="shiftKey" type="Number">0 no shift, 1 shift pressed.</param>
}
function acceptTrade(optionalCheck){
/// <param name="optionalCheck" type="Number">1: bool(Accepted Trade already), 2 return recent trade flag, 3: bool(red check)</param>
}
function getPath(id_Or_IdArray,x1,y1,x2,y2,useTele,Radius,useReduction){
    var ret = [] // mock fill array
    var p = new _path
    for (var j = 0; j < 1000; j++) {
        ret.push(p)
    }
    return ret
 }

function path (){}
path.prototype.x = function (){}
path.prototype.y = function (){}

function getCollision (area,x,y){}
function clickItem (clickType, item_OR_x, y, location){
/// <summary>clicks item or inventory spot</summary>
/// <param name="location" type="Number">0 - Inventory, 2 - Trade Window, 3 - Cube 4 - Stash 5 - Belt  OR 1 -merc Helmet, 3 -Merc Armor 4 -Merc Weapon</param>
/// <param name="clickType" type="Number">0 - Left, 1 - Right, 2 - Shift-left (put to belt), 3 - Mercenary Item</param>
}
function getLocaleString(id){}
function rand(minValue,maxValue){}
function getDistance(x1_orunitObject,y1_orunitObject,x2_orunitObject,y2){}
function gold(amount,mode){
/// <summary>moves gold</summary>
/// <param name="amount" type="Number">amount of gold to move</param>
/// <param name="mode" type="Number">1 = drop, 2 = inventory to trade, 3 = inventory to stash, 4 = stash to inventory</param>
}
function checkCollision(unit1,unit2,bitMask){
/// <summary>checks collision path between 2 units, bitmask is binary bit 0 and bit 1 =3</summary>
/// <param name="bitMask" type="Number">bit 0 : block walk, bit 1 : block light + block Line Of Sight bit 2 : block ranged bit 3 : block Player's walk,bit 4 : some doors, entry/exits, bit 5 : rocks trees, bit 7 : Players </param>
}
function getMercHP(){}
function getCursorType(shopCursor){
/// <summary>pass true for shop cursor nothing for regular cursor type</summary>
/// <param name="shopCursor" type="Number">true for shop cursor type blank for regular cursor</param>
}
function getSkillByName(Skill_Name){}
function getSkillByid(Skill_Id){}
function getTextWidthHeight(text,font){
/// <param name="font" type="Number">todo fill in with font discript</param>
}
function getTradeInfo(mode){ // core problem detected

}
function getUIFlag(id){
/// <param name="id" type="Number">1 inv, 2 stat, 4 skill, 5 chat,8 npc, 9 esc, 10 automap, 12 shop, 15 quest, 20 wp, 25 stash, 26 cube</param>
}
function getWaypoint(levelID){}
function quitGame(){
/// <summary>exit d2 completly</summary>
}
function Game(){
/// <summary>exit d2 game to bnet</summary>
}
function playSound(soundID){}
function say(text){}
function clickParty(partyObject,mode){
/// <param name="mode" type="Number">0 = Loot toggle, 1 = Hostile toggle, 2 = Party with Player, 3 = Leave Party</param>
}
function getBaseStat(table,classid,stat){

}
function weaponSwitch(querry_switchedTab){
/// <param name="querry_switchedTab" type="Number">1 to return active tab nothing to switch</param>
}
function transmute(){}
function getPlayerFlag(unit1Gid,unit2Gid,flag){}
function getMouseCoords(relitive_gameCoords,object_or_array){
/// <param name="relitive_gameCoords" type="Number">1 to return game coords 0 for screen coords</param>
/// <param name="object_or_array" type="Number">1 for object, 0 for array</param>
    if (object_or_array) 
        return new _path();
    else
        return [1][1];
}
function submitItem(){}
function getIsTalkingNPC(){}
function getInteractedNPC(){
	return new unitObject()
}
function login(profileName){}
function selectChar(charname){}
function createGame(gameName,gamePass,difficulty){
/// <param name="difficulty" type="Number">0:Normal, 1: Nightmare, 2:Hell, 3: hardest availabile</param>
}
function joinGame(gameName,gamePass){}
function addProfile(profileName, mode, gateway, userName, password, charName,spDifficulty){
/// <param name="mode" type="String">single, battle.net, other multiplayer</param>
/// <param name="spDifficulty" type="String">0 - normal, 1 - nightmare, 2 - hell</param>
}
function getOOGLocation(){}
function createCharacter(name, type, hc, ladder) {}// unfinished

function partyObject(){}
partyObject.prototype.x = function(){}
partyObject.prototype.y = function(){}
partyObject.prototype.area = function(){}
partyObject.prototype.gid = function(){}
partyObject.prototype.life = function(){}
partyObject.prototype.partyflag = function(){}
partyObject.prototype.partyid = function(){}
partyObject.prototype.name = function(){}
partyObject.prototype.classid = function(){}
partyObject.prototype.level = function(){}
partyObject.prototype.getNext = function(){
    return new partyObject()
}
function getParty(){
    return new partyObject()
}

function presetUnitObject() { }
presetUnitObject.prototype.type =function(){}
presetUnitObject.prototype.roomx =function(){}
presetUnitObject.prototype.roomy =function(){}
presetUnitObject.prototype.x =function(){}
presetUnitObject.prototype.y =function(){}
presetUnitObject.prototype.id =function(){}
presetUnitObject.prototype.level =function(){}

function roomObject (){}
roomObject.prototype.number = function(){}
roomObject.prototype.x = function(){}
roomObject.prototype.y = function(){}
roomObject.prototype.xsize = function(){}
roomObject.prototype.ysize = function(){}
roomObject.prototype.subnumber = function(){}
roomObject.prototype.area = function(){}
roomObject.prototype.level = function(){}
roomObject.prototype.correcttomb = function(){}
roomObject.prototype.getNext = function(){
    return new roomObject()
}
roomObject.prototype.reveal = function(){}
roomObject.prototype.getPresetUnits = function(){
    return getPresetUnits()
}
roomObject.prototype.getCollision = function(){}
roomObject.prototype.getNearby = function(){
	var ret = [] // mock fill array
    var p = new roomObject
    for (var j = 0; j < 1000; j++) {
        ret.push(p)
    }
    return ret
 }

roomObject.prototype.getStat = function(stat){}
roomObject.prototype.getFirst = function(){
  /// <summary>Gets first room in area</summary>
}
roomObject.prototype.unitInRoom = function(unitObject){}

function getRoom(area,x,y){
 /// <summary>finds rooom (calling with 0 prams = me.room.</summary>
 return new roomObject()
}
function sandbox(){
 /// <summary>sandbox script container?</summary>
}
sandbox.prototype.evaluate = function(script){}
sandbox.prototype.include = function(path){}
sandbox.prototype.isIncluded = function(path){}
sandbox.prototype.clearScope = function(){}

function d2bsScriptObject (){}
d2bsScriptObject.prototype.name = function(){}
d2bsScriptObject.prototype.type = function(){}
d2bsScriptObject.prototype.running = function(){}
d2bsScriptObject.prototype.threadid = function(){}
d2bsScriptObject.prototype.getNext = function(){
    return new d2bsScriptObject()
}
d2bsScriptObject.prototype.pause = function(){}
d2bsScriptObject.prototype.resume = function(){}
d2bsScriptObject.prototype.stop = function(){}
d2bsScriptObject.prototype.send = function(args){}
function getScript(true_forCurrent_or_threadID_or_scriptName){
    return new d2bsScriptObject()
}
function SQLite (dbPath){}
SQLite.prototype.path = function(){}
SQLite.prototype.statements = function(){}
SQLite.prototype.isOpen = function(){}
SQLite.prototype.lastRowId = function(){}
SQLite.prototype.execute = function(sql){}
SQLite.prototype.query = function(sql){
    return new DBStatement()
}
SQLite.prototype.open = function(){}
SQLite.prototype.close = function(){}

function DBStatement (){}
DBStatement.prototype.getObject = function(){}
DBStatement.prototype.getColumnCount = function(){}
DBStatement.prototype.getColumnValue = function(){}
DBStatement.prototype.getColumnName = function(){}
DBStatement.prototype.go = function(){}
DBStatement.prototype.next = function(){
    return new DBStatement()
}
DBStatement.prototype.skip = function(){}
DBStatement.prototype.reset = function(){}
DBStatement.prototype.close = function(){}
DBStatement.prototype.bind = function(colum,myVar){}
DBStatement.prototype.sql = function(){}



function unitObject() {  }
unitObject.prototype.canel = function () {
    /// <summary>Closes UI screen.</summary>
}
unitObject.prototype.getNext = function () {
    return new unitObject()
}


unitObject.prototype.repair = function () { }
unitObject.prototype.useMenu = function (menuId) {
/// <summary>Uses a Npc menu command</summary>
/// <param name="menuId" type="Number">0x0D35 Talk, 0x0D44 Trade 0x0D06 Trade/Repair 0x0FB1 Imbue 0x0D46 Gamble 0x0D45 Hire, 0x0D36 Go East, 0x0D37 Go West, 0x0FB4 Id Items, 0x0D38 Sail East 0x0D39 Sail West, 0x1507 Ressurect Merc 0x58DC Add Sockets, 0x58DD Personalize,0x58D2 Travel to Harrogath</param>
 }
unitObject.prototype.interact = function () { }
unitObject.prototype.getItem = function (Item_Name_or_Classid,itemMode,itemGID) { 
    return new unitObject()
}

unitObject.prototype.getItems = function () { 
    var ret = [] // mock fill array
    var p = new unitObject
    for (var j = 0; j < 100; j++) {
        ret.push(p)
    }
    return ret
  }

unitObject.prototype.getMerc = function (simpleTest_or_return_mercunitObject) {
    return new unitObject()
}
unitObject.prototype.getSkill = function (Hand_or_ID,cumulitive) {
 /// <param name="Hand_or_ID" type="Number">2=right, 3=left Or id with 2nd Pram</param>
 /// <param name="cumulitive" type="Number">0=base 1=cumulitive skill</param>
 }
unitObject.prototype.getParent = function () { }
unitObject.prototype.overhead = function (text) { }
unitObject.prototype.getFlags = function () { 
 /// <summary>returns items full flag value</summary>
}
unitObject.prototype.getFlag = function (flag) {
    /// <param name="flag" type="Number">0x4000000	isRuneword, 0x400000 isEthereal, 0x200000 ear, 0x4000 isNotInSocket, 0x2000	isInStore, 0x800 isSocketed, 0x100 isBroken, 0x10 isIdentified</param>
}
  unitObject.prototype.getStat = function (stat,subStat) {
 /// <param name="stat" type="Number">stat or -1 for stat list -2 for complete list</param>
}
unitObject.prototype.getState = function (stateID) { 
 /// <param name="stateID" type="Number">1 - 159</param>
}
unitObject.prototype.getPrice = function (DEPRECATED) { }
unitObject.prototype.shop = function (mode) {
 /// <param name="mode" type="Number">1=sell, 2=buy 6=shift buy</param>
 }
unitObject.prototype.getEnchant = function (enchantID) {
 /// <param name="enchantID" type="Number">5:Strong, 6:Fast, 7:cursed, 8:magicResitant, 16:chapion, 17 Lightning, 18: cold, 25:manaBurn /yamb/sdk/enchants </param>
}
unitObject.prototype.setSkill = function (skillId_OR_skillName, hand,chargedItem) {
/// <param name="hand" type="Number">0 = right, 1 = left</param>
/// <param name="chargedItem" type="unit">item with the charged skill</param>
}
unitObject.prototype.move = function (x,y) {
 /// <summary>clickMap at x,y </summary>
 }

unitObject.prototype.getQuest = function (QuestNumber, QuestCompletion) {
    /// <param name="QuestNumber" type="Number">0 = Spoke to Warriv, 1 = Den of Evil,2 = Sisters' Burial Grounds, 4 = The Search for Cain, 5 = Forgotten Tower, 3 = Tools of the Trade, 6 = Sisters to the Slaughter, 7 = Able to go to Act II, 8 = Spoke to Jerhyn, 9 = Radament's Lair, 10 = The Horadric Staff, 11 = The Tainted Sun, 12 = The Arcane Sanctuary, 13 = The Summoner ,14 = The Seven Tombs, 15 = Able to go to Act III, 16 = Spoke to Hratli, 20 = The Golden Bird, 19 = Blade of the Old Religion, 18 = Khalim's Will, 17 = Lam Esen's Tome, 21 = The Blackened Temple, 22 = The Guardian, 23 = Able to go to Act IV, 24 = Spoke to Tyrael, 25 = The Fallen Angel, 27 = Hell's Forge, 26 = Terror's End, 28 = Able to go to Act V,35 = Seige on Haggorath, 36 = Rescue on Mount Arreat, 37 = Prison of Ice, 38 = Betrayal of Haggorath, 39 = Rite of Passage, 40 = Eve of Destruction</param>
        /// <param name="QuestCompletion" type="Number">0 = Quest Complete, 2 = Quest Started 3-10 = Parts of Quest Complete Varies by Quest, 12 = Quest Box Filled in</param>
}

unitObject.prototype.getMinionCount = function () { }
unitObject.prototype.getRepairCost = function () { }
unitObject.prototype.getItemCost = function (mode,npcClassId,Diff) { 
 /// <param name="mode" type="Number">0 buy, 1 sell, 2 repair</param>
} 	 
 	 
  
unitObject.prototype.x = function () { }
unitObject.prototype.y = function () { }
unitObject.prototype.name = function () { }
unitObject.prototype.type = function () { }
unitObject.prototype.classid = function () { }
unitObject.prototype.mode = function () { }
unitObject.prototype.act = function () { }
unitObject.prototype.gid = function () { }
unitObject.prototype.area = function () { }
unitObject.prototype.hp = function () { }
unitObject.prototype.mp = function () { }
unitObject.prototype.stamina = function () { }
unitObject.prototype.staminamax = function () { }
unitObject.prototype.charlvl = function () { }
unitObject.prototype.itemcount = function () { }
unitObject.prototype.owner = function () { }
unitObject.prototype.ownertype = function () { }
unitObject.prototype.spectype = function () { }
unitObject.prototype.direction = function () { }
unitObject.prototype.uniqueid = function () { }
unitObject.prototype.code = function () { }
unitObject.prototype.prefix = function () { }
unitObject.prototype.suffix = function () { }
unitObject.prototype.prefixnum = function () { }
unitObject.prototype.suffixnum = function () { }
unitObject.prototype.fname = function () { }
unitObject.prototype.quality = function () { }
unitObject.prototype.node = function () { }
unitObject.prototype.location = function () { }
unitObject.prototype.sizex = function () { }
unitObject.prototype.sizey = function () { }
unitObject.prototype.description = function () { }
unitObject.prototype.bodylocation = function () { }
unitObject.prototype.ilvl = function () { }
unitObject.prototype.lvlreq = function () { }
unitObject.prototype.runwalk = function () { }
unitObject.prototype.weaponswitch = function () { }
unitObject.prototype.objtype = function () { }
unitObject.prototype.islocked = function () { }

function Frame(x, y, xsize, ysize, alignment, automap, onClick, onHover){
 /// <returns type="ScreenHook">Returns a Frame ScreenHook.</returns>
 /// <param name="alignment" type="Number">0 = left, 1 = Middle, 2 = Right</param>
}
Frame.prototype.x = function () {}
Frame.prototype.y = function () {}
Frame.prototype.xsize = function () {}
Frame.prototype.ysize = function () {}
Frame.prototype.visible = function () {}
Frame.prototype.align = function () {}
Frame.prototype.zorder = function () {}
Frame.prototype.hover = function () {}
Frame.prototype.click = function (){}


function Line(x, y, x2, y2, color, AutoMap, click, hover){
/// <returns type="ScreenHook">Returns a Line ScreenHook.</returns>
/// <param name="color" type="Number">1,5,8,10:red  6,7,9,11-14:Yellow, 0,15,16,33-37:Black 22-32 gray->white 4,6,7,50:Brown 6,16:Grey 11:Purple 8:Orange</param>
}
Line.prototype.x = function () {}
Line.prototype.y = function () {}
Line.prototype.x2 = function () {}
Line.prototype.y2 = function () {}
Line.prototype.color = function () {}
Line.prototype.zorder = function () {}
Line.prototype.click = function () {}
Line.prototype.hover = function () {}
Line.prototype.visible = function (){}

function Box(x, y, xsize, ysize, color, opacity, alignment, automap, onClick, onHover) {
 /// <returns type="ScreenHook">Returns a Line ScreenHook.</returns>
 /// <param name="alignment" type="Number">0 = left, 1 = Middle, 2 = Right</param>
 /// <param name="color" type="Number">1,5,8,10:red  6,7,9,11-14:Yellow, 0,15,16,33-37:Black 22-32 gray->white 4,6,7,50:Brown 6,16:Grey 11:Purple 8:Orange</param>
}
Box.prototype.x = function () {}
Box.prototype.y = function () {}
Box.prototype.xsize = function () {}
Box.prototype.ysize = function () {}
Box.prototype.visible = function () {}
Box.prototype.color = function () {}
Box.prototype.opacity = function () {}
Box.prototype.align = function () {}
Box.prototype.zorder = function (){}
Box.prototype.hover = function () {}
Box.prototype.visible = function (){}

function Text(text, x, y, color, font, alignment, automap, onClick, onHover) {
    /// <param name="color" type="Number">0,13,18,21:White 1,19:Red, 2,10,12:Green 3:Blue 4,7,9:Brown 6,16:Grey 11:Purple 8:Orange</param>
    /// <param name="alignment" type="Number">0 = left, 1 = Middle, 2 = Right</param>
    /// <param name="font" type="Number">1-13 large -> small: 3,2,7,5,1,8,13,4,11,6,12  9,10 =buggy</param>
}
Text.prototype.x = function () {}
Text.prototype.y = function () {}
Text.prototype.color = function () {}
Text.prototype.font = function () {}
Text.prototype.visible = function () {}
Text.prototype.align = function () {}
Text.prototype.zorder = function () {}
Text.prototype.click = function () {}
Text.prototype.hover = function () { }


function meUnit() { 

}
meUnit.prototype.account = function () { }
meUnit.prototype.charname = function () { }
meUnit.prototype.diff = function () { }
meUnit.prototype.gamename = function () { }
meUnit.prototype.gamepassword = function () { }
meUnit.prototype.gameserverip = function () { }
meUnit.prototype.gamestarttime = function () { }
meUnit.prototype.gametype = function () { }
meUnit.prototype.itemoncursor = function () { }
meUnit.prototype.ladder = function () { }
meUnit.prototype.ping = function () { }
meUnit.prototype.fps = function () { }
meUnit.prototype.playertype = function () { }
meUnit.prototype.realm = function () { }
meUnit.prototype.realmshort = function () { }
meUnit.prototype.mercrevivecost = function () { }
meUnit.prototype.runwalk = function () { }
meUnit.prototype.weaponswitch = function () { }
meUnit.prototype.chickenhp = function () { }
meUnit.prototype.chickenmp = function () { }
meUnit.prototype.quitonhostile = function () { }
meUnit.prototype.blockKeys = function () { }
meUnit.prototype.blockMouse = function () { }
meUnit.prototype.gameReady = function () { }
meUnit.prototype.profile = function () { }
meUnit.prototype.nopickup = function () { }
meUnit.prototype.pid = function () { }
meUnit.prototype.screensize = function () { }
meUnit.prototype.windowtitle = function () { }
meUnit.prototype.ingame = function () { }
meUnit.prototype.quitonerror = function () { }
meUnit.prototype.maxgametime = function () { }
meUnit.prototype.mapid = function () { }
meUnit.prototype.hpmax = function () { }
meUnit.prototype.mpmax = function () { }
meUnit.prototype.staminamax = function () { }
meUnit.prototype.charlvl = function () { }


var me = new meUnit()
var testUnit = new unitObject()
function MergeRecursive(obj1, obj2) {

    for (var p in obj2) {
        try {
            // Property in destination object set; update its value.
            if (obj2[p].varructor == Object) {
                obj1[p] = MergeRecursive(obj1[p], obj2[p]);

            } else {
                obj1[p] = obj2[p];

            }

        } catch (e) {
            // Property in destination object not set; create it and set its value.
            obj1[p] = obj2[p];
        }
    }

    return obj1;
}
MergeRecursive(me, testUnit)


function getUnit(type, localizedName_OR_classid, mode, globalid) {
    /// <param name="type" type="Number">0 = Player, 1 = Monster/NPC, 2 = Object, 3=Missle, 4=Item, 5=Tile, 100= ItemOnCursor, 101 = Hoverd unit</param>
    /// <param name="localizedName_OR_classid" type="Number">string name or int classid</param>
    return new unitObject()
}
function getPresetUnits(areaId, type, classid) {
    /// <param name="type" type="Number">0 = Player, 1 = Monster/NPC, 2 = Object, 3=Missle, 4=Item, 5=Tile, 100= ItemOnCursor, 101 = Hoverd unit</param>
    var ret = [] // mock fill array
    var p = new presetUnitObject
    for (var j = 0; j < 1000; j++) {
        ret.push(p)
    }
    return ret
   }
function getPresetUnit(areaId, type, classid) {
    /// <param name="type" type="Number">0 = Player, 1 = Monster/NPC, 2 = Object, 3=Missle, 4=Item, 5=Tile, 100= ItemOnCursor, 101 = Hoverd unit</param>
    return new presetUnitObject()
}


var NTC_UNIT_PLAYER = 0;
var NTC_UNIT_NPC = 1;
var NTC_UNIT_MERC = 1;
var NTC_UNIT_MONSTER = 1;
var NTC_UNIT_OBJECT = 2;
var NTC_UNIT_MISSILE = 3;
var NTC_UNIT_ITEM = 4;
var NTC_UNIT_TILE = 5;
var NTC_UNIT_STASH = 267;

function NTC_DoCast(skillid, usehand, x, y, retrys) {}
function NTC_PutSkill(skillid, hand) {}
function NTC_CheckSkill(skillid) {}

function NTC_GetCastDelay(skillid) {}
function NTC_IsLeftSkill(skillid) {}
function NTC_IsPassiveSkill(skillid) { }


function NTC_FindMonster(classid_or_Name, spectype) {
    return new unitObject
}
function NTC_DoWeaponSwitch(switchto) { }
function NTC_DoClick(button, shift, x_or_unit, y) {}
function NTC_MyGold() {}
function NTC_MyGoldCarry() {}
function NTC_StashGoldFull() {}
function NTC_CharGoldFull() {}
function NTC_CheckOwner(object, ownername) {}
function NTC_ItemToCursor(item) {}
function NTC_ClearCursor() {}
function NTC_GetMerc() {
return new unitObject
}
function NTC_OpenChest(chest) {}
function NTC_Delay(amount, random) {}
function NTC_PingDelay(amount) {}
function NTC_InTown(who) {}
function NTC_InMyParty(playerName) {}
function NTC_GetUnit(unittype, classid, retries) {
return new unitObject
}

function NTC_GetPresetUnits(unittype, classid, area) {
   return new presetUnitObject
}
function getPotsInCol(collum) {
    var count = 0;
    var items = NTC_GetItems(me);
    for (var i = 0; i < items.length; i++) {
        if (items[i].mode == 2) {
            //if ((items[i].x==col) || (items[i].x==col+4) || (items[i].x==col+8 ) || (items[i].x==col+12))
            for (var a = col; a < (col + 13); a = a + 4) {
                if (items[i].x == a)
                    count++
            }
        }
    }

    return count;
}
function getFirstPotCode(col) {

    var items = NTC_GetItems(me);
    for (var i = 0; i < items.length; i++) {
        if (items[i].mode == 2) {
            if (items[i].x == col)
                return items[i].code;
        }
    }
    return false;
}
function debugPrintBeltGID() {

    var items = NTC_GetItems(me);
    for (var i = 0; i < items.length; i++) {
        if (items[i].mode == 2) {
            debugPrint(items[i].gid);
        }
    }
}
function NTC_GetItems(who) {
var a = getUnit()
return a.getItems()
}
function NTC_GetBeltSize() {
    debugPrint("ntcGetbeltsize");
    var _items = NTC_GetItems();

    if (!_items)
        return 1;

    for (var i = 0; i < _items.length; i++) {
        if (_items[i].mode == 1 && _items[i].bodylocation == 8) {
            if (_items[i].code == "lbl" || _items[i].code == "vbl")
                return 2;
            else if (_items[i].code == "mbl" || _items[i].code == "tbl")
                return 3;

            return 4;
        }
    }

    return 1;
}

function NTC_FindScript(scriptname) {
    debugPrint("ntc findScript");
    var _script;
    var _scriptpath;

    _script = getScript();

    if (_script) {
        do {
            _scriptpath = _script.name.toLowerCase();

            if (_scriptpath.indexOf(scriptname.toLowerCase()) != -1)
                return _script;
        } while (_script.getNext());
    }

    return false;
}

function NTC_SendMsgToScript(scriptname, msg) {
    //debugPrint("ntcSentMsgToScript" );
    var _script = NTC_FindScript(scriptname);

    if (_script) {
        _script.send(msg);

        return true;
    }

    return false;
}

function NTC_StopScript(scriptname) {
    debugPrint("ntc Stop Script");
    var _script = NTC_FindScript(scriptname);

    if (_script) {
        _script.stop();

        return true;
    }

    return false;
}

function NTC_WaitCastDone() {
    debugPrint("ntc WaitCastDone");

    var _mode;

    for (var i = 0; i < 75; i++) {
        _mode = me.mode;

        if (!(_mode == 7 || _mode == 8 || (_mode >= 10 && _mode <= 16) || _mode == 18)) {
            debugPrint("ntc WaitCastDone return true mode: " + me.mode);
            return true;
        }

        NTC_Delay(25);
    }

    debugPrint("ntc WaitCastDone return false");

    return false;
}

function NTC_SendLogToOOG(logString) {
    debugPrint("ntc sendlogtooog: " + logString);
    return sendCopyData(null, "D2Manager 2.5", (65535 << 16) | 65535, me.windowtitle + ";" + logString);
}

// Internal function
function NTC_CheckDoCastInt() {
}

/**
*	@name	NTC_GetBaseStat()
*
*	@args	tableid
*			classid
*			statid
*
*	@desc	Wrapper for getBaseStat() to handle a potential crash condition in D2BS v1.4
*	@see	http://docs.d2bs.org/group__globalFunctions.html#gb4ebba6bce777cb9705e199af1d08bc8
*/
function NTC_GetBaseStat(tableid, classid, statid) {
    //	handle arguments
    //	TODO:	make a tableArgsInRange() closure?
    if (arguments.length != 3) {
        print('NTC_GetBaseStat() failed, incorrect argument number');
        return undefined;
    }

    var output;

    //if crash condition **getBaseStat() called on item table**	(no longer 1.4 specific.. crashes in item table seem to happen often in 1.3.3 as well :\
    if ((tableid == 0)) {
        //check for itemTable object
        if (!isIncluded('objects/itemTable.json'))
            NTC_Include('objects/itemTable.json');
        if (typeof (itemTable) != 'object') {
            print('ÿc1Error: NTC_GetBaseStat() couldn\'t find itemTable object to reference..');
            return undefined;
        }

        //collect array of itemTable object keys
        var tablekeys = Object.keys(itemTable);

        //check type of statid and do requested lookup
        if (typeof (statid) == 'string') {
            if ((tablekeys.indexOf(statid) != -1) && (classid < itemTable[statid].length)) {
                return itemTable[statid][classid];
            }
        }
        else if (typeof (statid) == 'number') {
            if ((statid < tablekeys.length) && (classid < itemTable[tablekeys[statid]].length)) {
                return itemTable[tablekeys[statid]][classid];
            }
        }

        //if here, object statid didn't exist, or classid array would have been out of bounds;
        return undefined;
    }
    else	//not an item table lookup, so call getBaseStat() with given args then return output;
    {
        output = getBaseStat.apply(this, arguments);
    }

    return output;
}


var NTConfig_PublicMode;
var NTConfig_CastStatic;
var NTConfig_CastShadowMaster;
var NTConfig_CastShadowWarrior;
var NTConfig_CastBrustOfSpeed;
var NTConfig_CastFade;
var NTConfig_CastBladeShield;
var NTConfig_CastCloakOfShadows;
var NTConfig_UseTraps;
var NTConfig_PutAura;
var NTConfig_UseRedemption;
var True = true;
var False = false;
var NTConfig_Script = new Array();
var NTConfig_CouncilExtension;
var NTConfig_DacFarrenExtension;
var NTConfig_ShenkExtension;
var NTConfig_PindleskinExtension;
var NTConfig_NihlathakExtension;
var NTConfig_WSK2Extension;
var NTConfig_WSK3Extension;
var NTConfig_RuinedExtension;
var NTConfig_DisusedExtension;
var NTConfig_KillBaal;
var NTConfig_ClearPitLevel1;
var NTConfig_KillFangskin;
var NTConfig_FireEyeExtension;
var NTConfig_HelpPreClear = false;
var NTConfig_HelpWithWaves = false;
var NTConfig_HelpWithBaal = false;
var NTConfig_GetBaalItems = false;
var NTConfig_ClearLeechPosition = true;
var KillBaalMsg;
var BaalTPMsg;
var BaalTPSafeMsg;
var BaalNextGameMsg;
var DiabloTPMsg;
var KillingDiabloMsg;
var DiabloNewGameMsg;
var NTConfig_Moattrick;

var NTConfig_StartDelay;
var NTConfig_AreaDelay;
var NTConfig_SnagDelay;

var NTConfig_SkipHealLife;
var NTConfig_SkipHealMana;
var NTConfig_SkipHealCurses;
var NTConfig_UseMerc;
var NTConfig_ResetWeapon;

var NTConfig_LifeThresh;
var NTConfig_LifeRejuvThresh;
var NTConfig_ManaThresh;
var NTConfig_ManaRejuvThresh;
var NTConfig_LifeChicken;
var NTConfig_ManaChicken;

var NTConfig_MercLifeThresh;
var NTConfig_MercRejuvThresh;
var NTConfig_MercChicken;

var NTConfig_FreeSpace;

var NTConfig_Columns = new Array(4);
NTConfig_Columns[0] = new Array(10);
NTConfig_Columns[1] = new Array(10);
NTConfig_Columns[2] = new Array(10);
NTConfig_Columns[3] = new Array(10);

var NTConfig_MinGoldToStash;

var NTConfig_BeltColType = new Array(4);
var NTConfig_BeltColMin = new Array(4);

var NTConfig_NIPFilePath = new Array();
var NTConfig_SnagRange;
var NTConfig_UseTeleportForPickit;

var NTConfig_GambleIt;
var NTConfig_GambleStashThreshold = 2500000;
var NTConfig_GambleStopGold;

var NTConfig_GambleItem = new Array();

var NTConfig_OpenChest;

var NTConfig_AttackFirst;
var NTConfig_AttackBoss;
var NTConfig_AttackOthers;
var NTConfig_AttackSecondary;
var NTConfig_ClearPosition;
var NTConfig_CheckSafe;
var NT_LeechType = 3;

var SimonNextLevel = 109;
var SimonPrevousLevel = 111;
var SimonEscToTown = 69;
var SimonEnableSnagging;
var SimonAutoReveal;
var SimonStopLifeWatch;
var SimonEnableCommands;
var SimonPOI = 106;
var SimonFollowMode = 0;

// necro attack vars
var XP_Curse = 91;
var XP_BuildArmyCurse = 87
var XP_Golm = 0;
var XP_useSkel = true;
var XP_useSkelMage = true;
var XP_useRevive = true;
var XP_CorpseExplosion = true;
var XP_BuildArmyThresh = 4;

// barb attack vars
var NTConfig_WarcryBoss;
var NTConfig_WarcryOthers;

//	Leader Variables
//	Do not edit any of these lines concerning the leader vars here (that's what the char config file is for!)
var leader = "";
var NT_Leader = [];
var QuitWithLeaderList = [];

//Superunique Script Variables
var NTConfig_Funnytext = true;
var NTConfig_Act1 = true;
var NTConfig_SuperCorpsefire = true;
var NTConfig_CorpsefireChest = true;
var NTConfig_SuperBishibosh = true;
var NTConfig_BishiboshChest = true;
var NTConfig_SuperColdcrow = true;
var NTConfig_ColdcrowChest = true;
var NTConfig_SuperRaven = true;
var NTConfig_RavenChest = true;
var NTConfig_SuperBonebreaker = true;
var NTConfig_BonebreakerChest = true;
var NTConfig_SuperRakanishu = true;
var NTConfig_RakanishuChest = true;
var NTConfig_SuperGriswold = true;
var NTConfig_GriswoldChest = true;
var NTConfig_SuperTreehead = true;
var NTConfig_TreeheadChest = true;
var NTConfig_SuperCountess = true;
var NTConfig_CountessChest = true;
var NTConfig_SuperSmith = true;
var NTConfig_SmithChest = true;
var NTConfig_SuperPitspawn = true;
var NTConfig_PitspawnChest = true;
var NTConfig_SuperBoneAsh = true;
var NTConfig_BoneAshChest = true;
var NTConfig_SuperAndariel = true;
var NTConfig_AndarielChest = true;
var NTConfig_Act2 = true;
var NTConfig_SuperDarkElder = true;
var NTConfig_DarkElderChest = true;
var NTConfig_SuperBloodWitch = true;
var NTConfig_BloodWitchChest = true;
var NTConfig_SuperColdworm = true;
var NTConfig_ColdwormChest = true;
var NTConfig_SuperCreepingFeature = true;
var NTConfig_CreepingFeatureChest = true;
var NTConfig_SuperFireEye = true;
var NTConfig_FireEyeChest = true;
var NTConfig_SuperRadament = true;
var NTConfig_RadamentChest = true;
var NTConfig_SuperSummoner = true;
var NTConfig_SummonerChest = true;
var NTConfig_SuperDuriel = true;
var NTConfig_DurielChest = true;
var NTConfig_Act3 = true;
var NTConfig_SuperSarina = true;
var NTConfig_SarinaChest = true;
var NTConfig_SuperStormtree = true;
var NTConfig_StormtreeChest = true;
var NTConfig_SuperEndugu = true;
var NTConfig_EnduguChest = true;
var NTConfig_SuperSszark = true;
var NTConfig_SszarkChest = true;
var NTConfig_SuperIcehawk = true;
var NTConfig_IcehawkChest = true;
var NTConfig_SuperMephisto = true;
var NTConfig_MephistoChest = true;
var NTConfig_Council = true;

var NTConfig_Moat = true;
var NTConfig_Act4 = true;
var NTConfig_SuperHephasto = true;
var NTConfig_HephastoChest = true;
var NTConfig_SuperIzual = true;
var NTConfig_IzualChest = true;
var NTConfig_Act5 = true;
var NTConfig_SuperDacFarren = true;
var NTConfig_DacFarrenChest = true;
var NTConfig_SuperShenk = true;
var NTConfig_SuperEldritch = true;
var NTConfig_EldritchChest = true;
var NTConfig_SuperBonesaw = true;
var NTConfig_SuperEyeback = true;
var NTConfig_SuperSharptooth = true;
var NTConfig_SuperThreshSocket = true;
var NTConfig_ThreshSocketChest = true;
var NTConfig_SuperFrozenstein = true;
var NTConfig_FrozensteinChest = true;
var NTConfig_SuperPindleskin = true;
var NTConfig_SuperSnapchip = true;
var NTConfig_SnapchipChest = true;
var NTConfig_SuperNihlathak = true;
var NTConfig_NihlathakChest = true;
var NTConfig_DoCain = false;
var NTConfig_DoRaven = false;
var NTConfig_DoSmith = false;
var NTConfig_DoRadament = false;
var NTConfig_DoTome = false;
var NTConfig_DoIzual = false;
var NTConfig_DoForge = false;
var NTConfig_DoShenk = false;
var NTConfig_DoAnya = false;
var NTConfig_DoAncients = false;
var NTConfig_DoBaal = false;

var NTSB_StopShopping;
var NTSB_ShopOnNpcVisit;

var NT_IdleOnClone = false;
var NT_IdleOnSOJ = false;
var NT_IdleOnSOJTimeoutSec = 0;
var NT_HostileAction = 0; //0 = nothing 1 = quit 2 = kill
var NT_EnableCubing = false;
var NT_RecipieBox = [];
var recipieList = [];
var utilityList = [];


// stuff that should be here
var bobDebug = false;

var currentAction = "";
var NTcfgSellWhenFull = true;
var Bob_ItemList = new Array;
var revealedAreas = new Array;
var FastSnag = false;
var NT_PickUtility = true; // pickit fills belt and tombs
var NT_Pause = false;
var NT_LastPause = getTickCount();
var SumXP_GoAct4 = true;
var NT_LoggingMaxLines = 1000000;
var NT_logItems = true;
var NT_MinGameTime = false;
var inFastSnag = false;
var NTCreatedConfig = false;


var NTConfig_BeepOnFull = false;
var NTConfig_PrintAttack = false;

var NTConfig_CastBOForParty = false;
var NTConfig_BOMsg = "Precasting for party benefit in";
var NTConfig_BODelay = 5;

var NTCOW_DistanceToKing = 40;
var NTCOW_KingChicken = 50;

me.useTeleport = true; //enables use of teleport by default;
var NTConfig_TakeScreenshotOnChicken = false;







