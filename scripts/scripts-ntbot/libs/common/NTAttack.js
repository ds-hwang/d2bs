/// <reference path="/../../d2bsAPI.js" /> 

var NTA_DAMAGE_NONE = 0;
var NTA_DAMAGE_PHYSICAL = 1;
var NTA_DAMAGE_MAGIC = 2;
var NTA_DAMAGE_FIRE = 3;
var NTA_DAMAGE_LIGHTNING = 4;
var NTA_DAMAGE_COLD = 5;
var NTA_DAMAGE_POISON = 6;
var _NTA_SkillDamage=[];
var _NTA_SkillDelay =[];
var _NTA_SkillHand =[];
var ignoreKorpse =[];
var DEAD_MODE = (1 << 29) | (1 << 12);
var ntMap= new collMap(0)
var SortPoint = false;
var bestAttackDistance = []
bestAttackDistance[getSkillByName("Nova")] = 7
bestAttackDistance[getSkillByName("Charged Bolt")] = 20
bestAttackDistance[getSkillByName("Lightning")] = 20
bestAttackDistance[getSkillByName("Chain Lightning")] = 20
bestAttackDistance[getSkillByName("Fire Ball")] = 20
bestAttackDistance[getSkillByName("Fire Bolt")] = 25
bestAttackDistance[getSkillByName("Meteor")] = 35
bestAttackDistance[getSkillByName("Frost Nova")] = 7
bestAttackDistance[getSkillByName("Blizzard")] = 35
bestAttackDistance[getSkillByName("Frozen Orb")] = 12
bestAttackDistance[getSkillByName("Glacial Spike")] = 20
bestAttackDistance[getSkillByName("Ice Blast")] = 20
bestAttackDistance[getSkillByName("Ice Bolt")] = 20
function NTA_KillBoss(classid)
{
	var _target;
      
	for(var i = 0; i < 5; i++)
	{
		_target = NTC_FindMonster(classid);
		if (_target)
        {
			if(NTConfig_AttackFirst > -1 && NTA_GetResistance(_target, NTA_GetDamageType(NTConfig_AttackFirst)) < 100)
			{
				if(NTC_IsLeftSkill(NTConfig_AttackFirst))
					NTC_DoCast(NTConfig_AttackFirst, 2, _target);
				else
					NTC_DoCast(NTConfig_AttackFirst, NTC_HAND_RIGHT, _target);
			}
            if(NTConfig_AttackBoss < 0 && NTConfig_AttackSkill[1] <0 ) // no boss attack try regular attacks
    	        return NTA_Attack(_target, false, 200)
          
			return NTA_Attack(_target, true, 200)
   		}
	}

	return false;
}

function NTA_ClearPosition(range, spectype)
{
	var _orgx, _orgy;
	var _target;
	var _killnum = 0;
	var _units = 0;

	//if(NTConfig_AttackOthers < 1)
	//	return false;

	if(arguments.length < 1)
		range = 20;

	if(arguments.length < 2)
		spectype = 0;

	_orgx = me.x;
	_orgy = me.y;
	
	var mobs = NTC_GetUnit(NTC_UNIT_MONSTER);
	var MobList = [];
	if(mobs){
		do {
			if(NTA_IsValidTarget(mobs))
				MobList.push(copyUnit(mobs));
		} while(mobs.getNext());
	}
	_units = 0;

	if(MobList.length >0)
	{
		// print("Start Unit Loop.");
		MobList.sort(SortMonsters)
		while (MobList.length >= 1) {	
            SortPoint = new Point(_orgx,_orgy)
			MobList.sort(SortMonsters)
            SortPoint=false;
			_target = copyUnit(MobList[MobList.length-1]);	  // not sure if this is right
			MobList.pop();		
	
			if(NTA_IsValidTarget(_target) && (spectype == 0 || (_target.spectype & spectype)) && //getDistance(_orgx, _orgy, _target.x, _target.y) < range)
			(_orgx - range <_target.x) && (_orgx + range >_target.x) &&  //square vs circle calc
			(_orgy - range <_target.y) && (_orgy + range >_target.y))
			{
				//PreCasting							
				if(_killnum == 0 && _target.hp > 0){			
                    preAttack(_target)					
				}
				//(at this point we already know that (spectype == 0 || (_target.spectype & spectype)) so...
				//if spectype includes SuperUnique or Boss spectypes call NTA_Attack with boss arg set true and maxattacks at a high value;
				if((spectype & 0x01) || (spectype & 0x04))
				{
					if(NTA_Attack(_target, true, 200))
						_killnum++;
				}
				else
				{
					if(NTA_Attack(_target, false, 20))
						_killnum++;
				}
			
			}

		}
	}

	//Post casting
	if (me.classid == NTC_CHAR_CLASS_NECROMANCER){ //build army
		if (XP_useSkel) {			
			var success = XP_CheckRevives(70,363);
		}
		// Check Skeleton Mages
		if (XP_useSkelMage && success) {		
			success = XP_CheckRevives(80,364);
		}
		// Check Revives
		if (XP_useRevive && success) {			
			success = XP_CheckRevives(95);
		}	
	}
	if(me.classid == NTC_CHAR_CLASS_PALADIN)
	{
		if(NTConfig_UseRedemption && _killnum > 1 && NTC_PutSkill(124, NTC_HAND_RIGHT))
		{
			NTC_PingDelay(750);
		}
	}

	return (_killnum > 0);
}
function preAttack(_target){
    if(NTConfig_AttackFirst > -1 && NTA_GetResistance(_target, NTA_GetDamageType(NTConfig_AttackFirst)) < 100 && checkCollision(me, _target, 3) == 0){							
		if(NTC_IsLeftSkill(NTConfig_AttackFirst)){					
			NTC_DoCast(NTConfig_AttackFirst, 2, _target);						
		} else {						
			NTC_DoCast(NTConfig_AttackFirst, NTC_HAND_RIGHT, _target);						
		}						
		// PickIt (FastSnag)
		NTSI_FastSnag();					
	}
}
function NTA_preSpawnAttack(x,y){
 if (me.classid ===3 ) // pally 
    NTC_DoCast(112, 2);
 if (me.classid === 1 && NTConfig_AttackSkill[1] >-1){ // sorc
     NTC_DoCast(NTConfig_AttackSkill[1],2,x,y)
 }
 if(me.classid === 6 && NTConfig_UseTraps){
       NTA_CheckTraps(me,true)
    }
}
function isRoomReachable(room){
var col = room.getCollision();
		var size = (room.ysize > room.xsize ? room.ysize : room.xsize)
		for (var y =1; y < room.xsize -1; y++){
			for (var x =1; x < room.ysize-1; x++){
				if (col[x][y] == 0)
				//if(col[x-1][y] == && col[x-1][y-1] == 0 && col[x+1][y] == 0 && col[x+1][y+1] == 0 )
						return [room.x*5+y,room.y*5+x,size]; // this is probly wrong fixed x/y swap
				
			}
		}
	//	dumpRoom(room)
	return false;
}
function NTA_ClearRooms(AttackRoutine,minX,minY,maxX,maxY)
{

	if (minX > maxX){
		var tempx = maxX;
		maxX = minX;
		minX = tempx;
	}
	if (minY > maxY){
		var tempy = maxY;
		maxY = minY;
		minY = tempy;
	}
	var _room;
	var _rooms;
	var rx,ry,path,reachable;
	_room = getRoom();

	if(!_room)
		return false;

	_rooms = new Array();
var size = (_room.xsize > _room.ysize) ? _room.xsize : _room.ysize ;
	do
	{
		rx = parseInt(_room.x*5 + _room.xsize/2);
		ry = parseInt(_room.y*5 + _room.ysize/2)
		//path= getPath(me.area, me.x, me.y, rx, ry,false);
		
		reachable =isRoomReachable(_room);
		if (reachable){	
			//if (reachable[0]==7741)
			//	dumpRoom(_room);	
			//	
			if (maxX && maxY){
				if (rx <maxX && ry <maxY && rx > minX && ry > minY){
					_rooms.push([reachable[0],reachable[1]])//([parseInt(_room.x*5 + _room.xsize/2), parseInt(_room.y*5 + _room.ysize/2)]);
				}			
			}else{
				_rooms.push([reachable[0],reachable[1]])//[parseInt(_room.x*5 + _room.xsize/2), parseInt(_room.y*5 + _room.ysize/2)]);
			}
		}
			
	} while(_room.getNext());

	while(_rooms.length > 0)
	{
		_rooms.sort(NTA_SortRoomInt);
		_room = _rooms.shift();
		attackPrint(_rooms.toSource());		
			NTM_MoveTo(_room[0], _room[1]);		
			if(typeof(AttackRoutine) == 'function'){
				if(!AttackRoutine(size))
					return false;
			}else{
				NTA_ClearPosition(size+10);     //clear as big as the room are some are up to 60   
				NTSI_PickItems();
				NTP_UpdatePrecast();
			}		
	}	
	return true;
}

function NTA_ClearRange(AttackRoutine,minX,minY,maxX,maxY){

    var ar = getArea() 
    if(arguments.length <2){ // full area
        minX = ar.x*5
        maxX = ar.x*5+ar.xsize*5
        minY = ar.y*5
        maxY = ar.y*5+ar.ysize*5
    }

	if (minX > maxX){
		var tempx = maxX;
		maxX = minX;
		minX = tempx;
    }
	if (minY > maxY){
		var tempy = maxY;
		maxY = minY;
		minY = tempy;
   	}
  
    var ingnoreList =[];
    var _rooms = []
    for (var x = minX ; x < maxX; x=x+10){
        for (var y = minY ; y < maxY ; y=y+10){
            if(ntMap.getColl(me.area,x,y) %2 ==0 )
                _rooms.push(new Point(x,y));
         }
    }
    var _orgx = me.x;
    var _orgy = me.y;
    var _target;
	 var ignoreList =[];
	var mobs = NTC_GetUnit(NTC_UNIT_MONSTER);
	var MobList = [];
	if(mobs){
		do {
			if(NTA_IsValidTarget(mobs))
				MobList.push(copyUnit(mobs));
		} while(mobs.getNext());
	}
	
    _rooms.sort(SortDistance);

	while(_rooms.length > 0){
       var _room = _rooms.shift();
        for(var j = 0 ; j < _rooms.length; j++){
                        if (getDistance(me.x,me.y,_rooms[j].x, _rooms[j].y)<40)
                            _rooms.remove(j)
        }
       if(getDistance(me.x,me.y,_room.x,_room.y)>40){
	        NTM_MoveTo(_room.x, _room.y);	

            mobs = NTC_GetUnit(NTC_UNIT_MONSTER);
	        var MobList = [];
	        if(mobs){
	        	do {
			         if(NTA_IsValidTarget(mobs) && mobs.x > minX && mobs.x < maxX && mobs.y > minY && mobs.y < maxY )
			    	    MobList.push(copyUnit(mobs));
		        } while(mobs.getNext());
            }
            var _killnum = 0;
		    while (MobList.length >= 1) {	
                     mobs = NTC_GetUnit(NTC_UNIT_MONSTER);
	                 MobList.length =0 ;
	                if(mobs){
	                    do {
			                if(NTA_IsValidTarget(mobs) && mobs.x > minX && mobs.x < maxX && mobs.y > minY && mobs.y < maxY )
			    	                MobList.push(copyUnit(mobs));
		                } while(mobs.getNext());
                    }
                    MobList.sort(SortDistanceRev)
                    _target=MobList.pop()
                    if (!_target) break;
                    for (var j = 0 ; j < ignoreList.length; j++){
                       if(_target && _target.gid == ignoreList[j].gid)
                              _target=MobList.pop()                        
                    }
                    if (!_target) break;
                    if(_killnum == 0 && _target.hp > 0)			
                        preAttack(_target)					
				    if(NTA_Attack(_target, false, 30)){
						_killnum++;
                        NTSI_PickItems();
                    }
                    else
                       ignoreList.push(_target)
                    for(var j = 0 ; j < _rooms.length; j++){
                        if (getDistance(me.x,me.y,_rooms[j].x, _rooms[j].y)<45)
                            _rooms.remove(j)
                    }
	        }				
			if(typeof(AttackRoutine) == 'function'){
				if(!AttackRoutine(size))
					return false;			
	        }
        }// distance if
	}// loop rooms
    return true;
}

function NTA_IsValidTarget(monster, simple)
{
	if (monster && monster.type == 0 && NT_HostileAction ==2){		
		return true;
	}
	if(!monster || (monster.type != NTC_UNIT_MONSTER) )// added for pking 
		return false;

	if(monster.mode == 0 || monster.mode == 12 || monster.hp <= 0)
		return false;

	if(arguments.length < 2)
		simple = false;

	if(!simple)
	{
		if(!monster.name)
			return false;

		switch(monster.name.toLowerCase())
		{
		case "dummy":
		case "an evil force":
			return false;
		}

		switch(monster.classid)
		{
		case 271:	// Merc
		case 338:
		case 359:
		case 561:

		case 356:	// Decoy
		case 357:	// Valkyrie
		case 418:	// Shadow Warrior
		case 419:	// Shadow Master

		case 363:	// Necro Skeleton
		case 364:	// Necro Mage

		case 366:	// Compelling Orb
		case 406:	// Izual's Spirit

		case 266:	// Act1: Flavie		(Navi)		-> Between BloodMoor & ColdPlains
		case 408:	// Act4: Hadriel	(Malachai)	-> Entrance of River of Flames

		case 516:	// Act5: Impact point for Catapults (Invisible)	-> Frigid Highlands
		case 517:	// Act5: Impact point for Catapults (Invisible)
		case 518:	// Act5: Impact point for Catapults (Invisible)	-> Bloodyfoot Hill
		case 519:	// Act5: Impact point for Catapults (Invisible)
		case 522:	// Act5: Combatant Barbarian
		case 523:	// Act5: Combatant Barbarian

		case 543:	// Baal on stairs outside Worldstone Chamber
		case 545:	// Baal taunts
			return false;
		}

		if(monster.classid >= 289 && monster.classid <= 292) // Necro's Golem
			return false;

		if((monster.classid >= 326 && monster.classid <= 330) || (monster.classid >= 410 && monster.classid <= 417)) // Traps
			return false;

		if(monster.classid >= 351 && monster.classid <= 353) // Hydra
			return false;

		if(monster.classid >= 420 && monster.classid <= 432) // Druid's friend
			return false;
	}

	if(((monster.classid >= 110 && monster.classid <= 113) || monster.classid == 608) && monster.mode == 8)
		return false;

	if(monster.classid == 68 && monster.mode == 14)
		return false;

	if((monster.classid == 258 || monster.classid == 261) && monster.mode == 14)
		return false;

	if(monster.getStat(172) == 2 || monster.getState(105) == 0)
		return false;

	if(monster.getState(53) || monster.getState(96)) // Conversion, Revive
		return false;

	//if(NTC_GetBaseStat(1, monster.classid, 23))
	//	return false;
	if(checkCollision(monster, monster, 3)){
		attackPrint("mob off wall");
		
	}
	return true;
}


function NTA_GetDamageType(skillid)
{
	switch(skillid)
	{
	case 74: 	//	Corpse Explosion
	case 97: 	//	Smite
	case 106: 	//	Zeal
	case 107: 	//	Charge
	case 151: 	//	Whirlwind
	case 245: 	//	Tornado
		return NTA_DAMAGE_PHYSICAL;

	case 112:	//	Blessed Hammer
	case 84:	//	Bone Spear
		return NTA_DAMAGE_MAGIC;

	case 47:	//	Fire Ball
	case 56:	//	Meteor
		return NTA_DAMAGE_FIRE;

	case 48:	//	Nova
	case 49:	//	Lightning
	case 53:	//	Chain Lightning
		return NTA_DAMAGE_LIGHTNING;

	case 59:	//	Blizzard
	case 64:	//	Frozen Orb
		return NTA_DAMAGE_COLD;

	case 92:	//	Poison Nova
		return NTA_DAMAGE_POISON;
	}

	var _etype;

	if(skillid == 74)
		return NTA_DAMAGE_PHYSICAL;
	if(skillid == 101)
		return NTA_DAMAGE_NONE;
	_etype = NTC_GetBaseStat(3, parseInt(skillid, 10), 211);

	switch(_etype)
	{
		case 0: return NTA_DAMAGE_PHYSICAL;
		case 1: return NTA_DAMAGE_FIRE;
		case 2: return NTA_DAMAGE_LIGHTNING;
		case 3: return NTA_DAMAGE_MAGIC;
		case 4: return NTA_DAMAGE_COLD;
		case 5: return NTA_DAMAGE_POISON;
	}

	return NTA_DAMAGE_NONE;
}

function NTA_GetResistance(enemy, type)
{
	if(!enemy)
		return false;

	if(arguments.length == 2)
	{
		switch(type)
		{
		case NTA_DAMAGE_PHYSICAL:
			return enemy.getStat(36);

		case NTA_DAMAGE_MAGIC:
			return enemy.getStat(37);

		case NTA_DAMAGE_FIRE:
			return enemy.getStat(39);

		case NTA_DAMAGE_LIGHTNING:
			return enemy.getStat(41);

		case NTA_DAMAGE_COLD:
			return enemy.getStat(43);

		case NTA_DAMAGE_POISON:
			return enemy.getStat(45);
		}
	}

	return 0;
}

function NTA_Attack(target, boss, maxattacks)
{
	switch(me.classid)
	{
	case NTC_CHAR_CLASS_AMAZON:
		return NTA_AmazonAttackInt(target, boss, maxattacks);

	case NTC_CHAR_CLASS_SORCERESS:
		return NTA_SorceressAttackInt(target, boss, maxattacks);

	case NTC_CHAR_CLASS_NECROMANCER:
		return NTA_NecromancerAttackInt(target, boss, maxattacks);

	case NTC_CHAR_CLASS_PALADIN:
		return NTA_PaladinAttackInt(target, boss, maxattacks);

	case NTC_CHAR_CLASS_BARBARIAN:
		return NTA_BarbarianAttackInt(target, boss, maxattacks);

	case NTC_CHAR_CLASS_DRUID:
		return NTA_DruidAttackInt(target, boss, maxattacks);

	case NTC_CHAR_CLASS_ASSASSIN:
		return NTA_AssassinAttackInt(target, boss, maxattacks);
	}

	return false;
}

//Thank you to whoever wrote this on the bh.cc forums!
function NTA_AmazonAttackInt(target, boss, maxattacks)
{
	var _attackprimary;
	var _usehand2;
	var _range1, _range2;
	var _nohit = true;

	_attackprimary = boss ? NTConfig_AttackBoss : NTConfig_AttackOthers;

	if(NTConfig_AttackSecondary > -1)
	{
		_usehand2 = NTC_IsLeftSkill(NTConfig_AttackSecondary) ? 2 : NTC_HAND_RIGHT;
	}

	switch(_attackprimary)
	{
		case 10: // Jab
			_range1 = 1;
			break;
		case 14: // Power Strike
			_range1 = 1;
			break;
		case 19: // Impale
			_range1 = 1;
			break;
		case 20: // Lighting Bolt
			_range1 = 10;
			break;
		case 24: // Charged Strike
			_range1 = 1;
			break;
		case 26: // Strafe
			_range1 = 20;
			break;
		case 30: // Fend
			_range1 = 1;
			break;
		case 34: // Lighting Strike
			_range1 = 1;
			break;
		case 35: // Lighting Fury
			_range1 = 10;
			break;
		default:
			_range1 = 20;
			break;
	}

	switch(NTConfig_AttackSecondary)
	{
		case 10: // Jab
			_range1 = 1;
			break;
		case 14: // Power Strike
			_range1 = 1;
			break;
		case 19: // Impale
			_range1 = 1;
			break;
		case 20: // Lighting Bolt
			_range1 = 10;
			break;
		case 24: // Charged Strike
			_range1 = 1;
			break;
		case 26: // Strafe
			_range1 = 20;
			break;
		case 30: // Fend
			_range1 = 1;
			break;
		case 34: // Lighting Strike
			_range1 = 1;
			break;
		case 35: // Lighting Fury
			_range1 = 10;
			break;
		default:
			_range1 = 20;
			break;
	}

	while(NTA_IsValidTarget(target, true))
	{   
		if(getDistance(me,target) > _range1)
			NTA_MoveCloseInt(target, _range1);

		if (NTConfig_AttackOthers == 20 && NTA_GetResistance(target, NTA_DAMAGE_LIGHTNING) >= 100)
		{
			attackPrint('Cancelled attack on '+ target.name +', it seems to be immune to lightning dmg..');
			return false;
		}

		NTC_DoCast(_attackprimary, _usehand2, target);
	}

	NTSI_FastSnag();
	return true;
}

function NTA_SorceressAttackInt(target, boss, maxattacks)
{
	var _attackprimary; 
   var _primaryindex = boss ? 1 : 2; 
   var _maxattacks = 0; 
   var _usehand1, _usehand2; 
   var _damagetype1, _damagetype2; 
   var _range1, _range2; 
   var _prehp;
   var brokeImune =false;

	_attackprimary = boss ? NTConfig_AttackBoss : NTConfig_AttackOthers;

	_usehand1 = NTC_IsLeftSkill(_attackprimary) ? 2 : NTC_HAND_RIGHT;
	_damagetype1 = NTA_GetDamageType(_attackprimary);

	if(NTConfig_AttackSecondary > -1)
	{
		_usehand2 = NTC_IsLeftSkill(NTConfig_AttackSecondary) ? 2 : NTC_HAND_RIGHT;
		_damagetype2 = NTA_GetDamageType(NTConfig_AttackSecondary);
	}

	switch(_attackprimary)
	{
	case 48: // Nova
		_range1 = 20;
		break;
	default:
		_range1 = 25;
		break;
	}

	switch(NTConfig_AttackSecondary)
	{
	case 48: // Nova
		_range2 = 20;
		break;
	default:
		_range2 = 25;
		break;
	}

	if(NTConfig_CastStatic < 100 && staticTarget(target.name))
	{
      
		var _staticlevel = NTC_CheckSkill(42);

		if(_staticlevel > 0 && NTA_GetResistance(target, NTA_DAMAGE_LIGHTNING) < 100 && (boss || checkCollision(me, target, 3) == 0))
		{
			var _cast = 0;
			var _staticrange = Math.floor((5+_staticlevel-1)*2/3);
            
			while(_cast++ < 10 && parseInt(target.hp*100/128) > NTConfig_CastStatic)
			{
				NTA_MoveCloseInt(target, _staticrange);
				NTC_DoCast(42, NTC_HAND_RIGHT, target);
			}
		}
	}

	_prehp = target.hp;

	while(_maxattacks++ < maxattacks && NTA_IsValidTarget(target, true)) 
   {
       if (!checkLineLos(me.x,me.y,target.x, target.y)) {
         var los  = FindLOS(target,35)
			if (los)
			    NTM_MoveTo(los[0],los[1]);
            else {
               // print ("los fail return false")
               // ntMap.Dump()
                return false;
            }
    }
      NTSI_FastSnag();
	if (getDistance(me,target )>40 )
	    NTA_MoveCloseInt(target, 35) 
        

      if(NTA_GetResistance(target, _NTA_SkillDamage[_primaryindex]) < 100) 
      { 
         if((_maxattacks % 2) == 1) 
         { 
			if (me.area != 102 && NTConfig_Moattrick){
				if(!NTA_MoveCloseInt(target, _range1) && !boss) {
				 //   print ("move closer return false;")
                	return false; 
                }
			}
         } 

         NTA_DoCastInt(_primaryindex, target); 
      } 
      else if(NTConfig_AttackSkill[4] > -1 && NTA_GetResistance(target, _NTA_SkillDamage[4]) < 100) 
      { 
         if((_maxattacks % 2) == 1) 
         { 
			if (me.area != 102 && NTConfig_Moattrick){
				if(!NTA_MoveCloseInt(target, _range2) && !boss)  {
				   // print ("move closer return false2;")
                	return false; 
                }
			}
         } 
		
         NTA_DoCastInt(4, target); 

      } 
      else 
      { 
         if(boss) 
         { 
			if (me.area != 102 && NTConfig_Moattrick){
				if((_maxattacks % 2) == 1) 
					NTA_MoveCloseInt(target, _range1); 
			}
            NTA_DoCastInt(_primaryindex, target); 


         } 
         else  {
				  //  print (target.name + " is immune to attacks ") //untested
                if(me.getMerc() && me.getMerc().getState(28) && !target.getState(29) && !brokeImune){
                    brokeImune = true;
                    NTA_MoveCloseInt(target, 10)

                }else
                	return false; 
         }
      } 

      if(boss) 
      { 
         if((_maxattacks % 4) == 0 && target.hp >= _prehp) {
             if (target.classid == 526) {
                // print("nta Boss Stupid 1")
                 return false;
             }
         } 

         if((_maxattacks % 8) == 0) {
             if (target.hp < _prehp)
                 _prehp = target.hp;
             else {
              //  print("nta Boss Stupid 2")
                 return false;
             }
         } 
      } 
      else 
      { 
         if((_maxattacks % 4) == 0) 
         { 
            if(target.hp < _prehp) 
               _prehp = target.hp; 
            else  {
				//    print ("max 4 return false;")
                	return false; 
                }
         } 
		 
      } 

      if(NTConfig_CheckSafe) 
      { 
         if(!NTTMGR_CheckSafe(1))  {
				 //   print ("safe Check return false;")
                	return false; 
                }
      } 
   }
	NTSI_FastSnag();
	return true;
}
function getCurseState(curse){
if (curse == 66) // Amplify Damage 
	return 9;
if (curse == 76) //iron maden
	return 55;
if (curse == 82) //life tap
	return 58;
if (curse == 87)//Decrepify
	return 60;	
if (curse == 91) //lower resist
	return 61;	
return 1;
}

function getIntoLOS(target) {
    var los = false;
 		 if (!CheckColl(me.x,me.y,target.x, target.y)) {
       // if(!checkCollision(me, target,5)){
			//attackPrint("moving closer collision 10");
			
			los = FindLOS(target,20)
			if (los){
			
		//	print ("found los:"+los[0]+","+los[1])
			return	NTM_MoveTo(los[0],los[1]);
			}else return false;
			//NTA_MoveCloseInt(target, Math.round(getDistance(me, target)/2));
		}
		
	return true	
}

function NTA_NecromancerAttackInt(target, boss, maxattacks)
{
var _maxattacks = 0
//move these to global
var armySize = 0;
var success;
			

	while(_maxattacks++ < maxattacks && NTA_IsValidTarget(target, true) && target.hp > 0){
	armySize =me.getMinionCount(4)+me.getMinionCount(5)+me.getMinionCount(6);
		if (armySize > XP_BuildArmyThresh){		
			if(target && (!target.getState(getCurseState(XP_Curse)) ) && target.classid != 371) 		
				NTC_DoCast(XP_Curse,0,target);
			}else{
			if(target && (!target.getState(getCurseState(XP_BuildArmyCurse)) ) && target.classid != 371) 		
				NTC_DoCast(XP_BuildArmyCurse,0,target);			
		}	
		
		
		
		getIntoLOS(target);
		if (boss)		
			NTC_DoCast(NTConfig_AttackBoss,0,target);
		else
			NTC_DoCast(NTConfig_AttackOthers,0,target);
		
		if (armySize > XP_BuildArmyThresh && getDistance(me.x,me.y,target.x,target.y)>5){
		//sendCopyData(null, "OOG", 0,"going closer");		
			NTM_MoveTo(target.x+1, target.y, 0) //stay on mob if we got army going
			clickMap(0,0,me.x,me.y+1);
		}
		if (XP_CorpseExplosion) { 			
			XP_UseNecroCE(target); 
		}	
		if (XP_useSkel) {			
			 success = XP_CheckRevives(70,363);
		}
		// Check Skeleton Mages
		if (XP_useSkelMage && success) {		
			success = XP_CheckRevives(80,364);
		}
		// Check Revives
		if (XP_useRevive && success) {			
			success = XP_CheckRevives(95);
		}
	}
	
		
}
function XP_CheckRevives(reviveSkill,reviveID) {
	if (reviveSkill != 70 && reviveSkill != 80 && reviveSkill != 95){
		//DC_DPrint("XP_CheckRevives() -> Invalid skill passed!");
		return false;
	}
	var success    = false;
	var haveRevive = 0;
	var maxRevives = XP_MaxRevives(reviveSkill);
	var needMana   = 0;
	var skeltype   = 0;
	var XP_ToRevive = 3;
	var i = 0;
	// get the right skeltype id (4 = skeleton, 5 = skeletonmage, 6 = revive)
	switch(reviveSkill){
		case 70:
			needMana = 5 + NTC_CheckSkill(reviveSkill);
			skeltype = 4;
			break;
		case 80:
			needMana = 7 + NTC_CheckSkill(reviveSkill);
			skeltype = 5;
			break;
		case 95:
			needMana = 45;
			skeltype = 6;
			break;
	}
	// Count my revives
	haveRevive = me.getMinionCount(skeltype);//0;
	success = (haveRevive >= maxRevives || me.mp <= needMana);
	//XP_ToRevive controls how many corpses to proccess per function call
	while (haveRevive < maxRevives && me.mp > needMana && i++ < XP_ToRevive) {
		var korpse = getBodyTarget(25);
		if (reviveSkill == 95) {
			if (korpse && ((korpse.spectype & 0x07) == 0)	&& korpse.classid != 571
				&& korpse.classid != 572 && korpse.classid != 573
				&& korpse.classid != 312 && korpse.classid != 702
				&& NTC_GetBaseStat(6, korpse.classid, 8) && NTC_GetBaseStat(6, korpse.classid, 9)) {
				//DC_DPrint("Reviving " + korpse.name + " from range " + DA_GetRange(korpse));
				//	print("Reviving " + korpse.name + " from range " + DA_GetRange(korpse));
				getIntoLOS(korpse);
				//print("Raising " + (reviveSkill ==  70 ? "Skel": "Mage") + " from " + korpse.name + " from range " + getDistance(me,korpse));
				NTC_DoCast(reviveSkill,0,korpse.x,korpse.y,1); // casting revive fails some times so im just going to try once
				//delay(500);
				success = true;
			}
			else if (!korpse){
				break;
			}
		}
		else { // Skel or SkelMage
			if(korpse){
				var iLoop = 0;
				//DC_DPrint("Raising " + (reviveSkill ==  70 ? "Skel": "Mage") + " from " + korpse.name + " from range " + DA_GetRange(korpse));
				
				while(korpse && haveRevive == me.getMinionCount(skeltype) && iLoop++ < XP_ToRevive){
					//print("in loop: "+iLoop+" haveRevive = " + haveRevive + " minionCount = " + me.getMinionCount(skeltype));
					getIntoLOS(korpse);
					if(getDistance(me.x,me.y,korpse.x,korpse.y) <40){
					//print("Raising " + (reviveSkill ==  70 ? "Skel": "Mage") + " from " + korpse.name + " from range " + getDistance(me,korpse));
						NTC_DoCast(reviveSkill,0,korpse.x,korpse.y,1);
						//delay(500);
						//delay(((typeof me.serverip !== "undefined") ? me.ping : 50));
					}
				}
				success = true;
			}
			else{
				break;
			}
		}
		delay(300);
		haveRevive = me.getMinionCount(skeltype);
	}
	return success;
}
function XP_MaxRevives(revSkill) {
	switch(revSkill){
		case 70:
			return((me.getSkill(70,1) < 4) ? me.getSkill(70,1) : 2 + Math.floor(me.getSkill(70,1) / 3));
		case 80:
			return((me.getSkill(80,1) < 4) ? me.getSkill(80,1) : 2 + Math.floor(me.getSkill(80,1) / 3));
		case 95:
			return(me.getSkill(95,1));
	}
return false;
}
function XP_UseNecroCE(target) {
	if (!target || target.hp < 1 || (target.getStat(36) > 99 && target.getStat(39) > 99)) return;
	var CE_level = NTC_CheckSkill(74);
	var CEa = (CE_level - 1) / 3;
	var CE_rad = 2.6 + CEa;
	var bomb = getBodyTarget(25,target.x,target.y,CE_rad);
	//if (bomb && target && target.hp > 0 && me.mp > 20 && DA_GetRangeXY(target,bomb.x,bomb.y) <= CE_rad) {
	if (bomb && target && target.hp > 0 && me.mp > 20 && getDistance(target.x,target.y,bomb.x,bomb.y) <= CE_rad) {
		//DC_DoCast(74,0,bomb);
		NTC_DoCast(74,0, bomb);
		delay(100);
	}
}
function getBodyTarget(range,tagX,tagY,dist) {
	var body_target = false;
	
	var BodyTarget = getUnit(1, null, DEAD_MODE);
	if(BodyTarget) do {
		if (BodyTarget.getStat(172) == 2 || BodyTarget.getState(105) == 0) {
			continue;
		}
		if (BodyTarget.getState(99) || BodyTarget.getState(104) || BodyTarget.getState(118)) {
			continue;
		}
		// Use NeverCount base monstat for traps, hydra, etc.
		if (NTC_GetBaseStat(1,BodyTarget.classid,23)) {
			continue;
		}
		if(getDistance(me, BodyTarget) < (!range ? 25 : range) && !BodyTarget.getParent() &&
			/*BodyTarget.classid != 271 && BodyTarget.classid != 289 &&
			BodyTarget.classid != 290 && BodyTarget.classid != 291 &&
			BodyTarget.classid != 292 && BodyTarget.classid != 338 &&
			BodyTarget.classid != 351 && BodyTarget.classid != 352 &&
			BodyTarget.classid != 353 && BodyTarget.classid != 359 &&
			BodyTarget.classid != 363 && BodyTarget.classid != 364 &&
			BodyTarget.classid != 371 && BodyTarget.classid != 543 &&
			BodyTarget.classid != 561 && */
			NTC_GetBaseStat(6, BodyTarget.classid, 8) &&
			(((me.area == 132 || me.area == 131) && ((me.x < 15099) && (me.x > 15088)) && ((me.y < 5022) && (me.y > 5006))) ||
			!checkCollision(me.area,me.x,me.y,3,BodyTarget.x,BodyTarget.y,3,7))) {
			var skipTarget=false;
			for(var tk = 0; tk<ignoreKorpse.length; tk++) {
				if(ignoreKorpse[tk] == BodyTarget.gid) {
					skipTarget=true;
					break;
				}
			}
			if (tagX  && getDistance (tagX,tagY, BodyTarget.x,BodyTarget.y) > dist)
				skipTarget = true;
			if(!skipTarget) {
				ignoreKorpse.push(BodyTarget.gid);
					return BodyTarget;
			}
		}
	}while(BodyTarget.getNext(null,DEAD_MODE));
	return false;
}

function NTA_PaladinAttackInt(target, boss, maxattacks){
	var _attackprimary;
	var _maxattacks = 0;
	var _usehand1, _usehand2;
	var _damagetype1, _damagetype2;
	var _range1, _range2;
	var _prehp1, _prehp2;
	var _nohit = true;

	_attackprimary = boss ? NTConfig_AttackBoss : NTConfig_AttackOthers;

	_usehand1 = NTC_IsLeftSkill(_attackprimary) ? 2 : NTC_HAND_RIGHT;
	_damagetype1 = NTA_GetDamageType(_attackprimary);

	if(NTConfig_AttackSecondary > -1)
	{
		_usehand2 = NTC_IsLeftSkill(NTConfig_AttackSecondary) ? 2 : NTC_HAND_RIGHT;
		_damagetype2 = NTA_GetDamageType(NTConfig_AttackSecondary);
	}

	switch(_attackprimary)
	{
		case 101: // Holy Bolt
		case 121: // Fist of the Heavens
			_range1 = 20;
			break;
		default:
			_range1 = 2;
			break;
	}

	switch(NTConfig_AttackSecondary)
	{
		case 101: // Holy Bolt
		case 121: // Fist of the Heavens
			_range2 = 20;
			break;
		default:
			_range2 = 2;
			break;
	}

	_prehp1 = target.hp;
	attackPrint(target.hp +target.name);

	while(_maxattacks++ < maxattacks && NTA_IsValidTarget(target, true) && target.hp > 0)
	{
		//print("--Sub Step 1");
		attackPrint(target.hp +target.name+"in loop");
		if(checkCollision(me, target, 3))
		{
			if(boss)
			{
				if(_attackprimary == 101 || _attackprimary == 121)
					NTA_MoveCloseInt(target, Math.round(getDistance(me, target)/2));
			}
			else if(me.area != 131)
				return false;
		}

		_prehp2 = target.hp;

		//print("--Sub Step 2");
		attackPrint(NTA_GetResistance(target, _damagetype1)+" "+target.name+" resistance attack:"+_attackprimary);
		if(NTA_GetResistance(target, _damagetype1) < 100)
		{
			//print("--Sub Step 3A");

			if((_maxattacks % 2) == 1 && _nohit)
			{
				if(_attackprimary == 112)
				{
					//print("--Sub Step 3A-B");
					if(!NTM_MoveTo(target.x+1, target.y, 0) && !boss)
						return false;
				}
				else
				{
					//print("--Sub Step 3A-C");
					if(!NTA_MoveCloseInt(target, _range1) && !boss)
						return false;
				}

				//print("--Sub Step 3A-D");

				if(NTA_GetAttackAuraInt(_attackprimary) > 0)
					NTC_PutSkill(NTA_GetAttackAuraInt(_attackprimary), NTC_HAND_RIGHT);
			}

			//print("--Sub Step 3A-E");            
			attackPrint("casting "+_attackprimary+" on "+target.name);
			NTC_DoCast(_attackprimary, _usehand1, target);

			//print("--Sub Step 3A-F");                        

			NTSI_FastSnag();

			//print("--Sub Step 4A");            
		}
		else if(NTConfig_AttackSecondary > -1 && NTA_GetResistance(target, _damagetype2) < 100)
		{
			//print("--Sub Step 3B");

			if((_maxattacks % 2) == 1 && _nohit)
			{
				if(NTConfig_AttackSecondary == 112)
				{
					if(!NTM_MoveTo(target.x+1, target.y, 0) && !boss)
						return false;
				}
				else
				{
					if(!NTA_MoveCloseInt(target, _range2) && !boss)
						return false;

					if(NTConfig_AttackSecondary == 101)    //Thanks to menellerhur for this ! (walks to other side of enemy, so bolts don't hit merc);
					{
						NTA_GoToOtherSide(target);
					}
				}

				if(NTA_GetAttackAuraInt(NTConfig_AttackSecondary) > 0)
					NTC_PutSkill(NTA_GetAttackAuraInt(NTConfig_AttackSecondary), NTC_HAND_RIGHT);
			}

			NTC_DoCast(NTConfig_AttackSecondary, _usehand2, target);
			NTSI_FastSnag();

			//print("--Sub Step 4B");            
		}
		else
		{
			//print("--Sub Step 3C");        
			if(boss)
			{
				if((_maxattacks % 2) == 1 && _nohit)
				{
					if(_attackprimary == 112)
						NTM_MoveTo(target.x+1, target.y, 0);
					else
						NTA_MoveCloseInt(target, _range1);

					if(NTA_GetAttackAuraInt(_attackprimary) > 0)
						NTC_PutSkill(NTA_GetAttackAuraInt(_attackprimary), NTC_HAND_RIGHT);
				}

				NTC_DoCast(_attackprimary, _usehand1, target);
				NTSI_FastSnag();
			}
			else
				return false;
			//print("--Sub Step 4C");                
		}

		//print("--Sub Step 4");        

		if(boss)
		{
			if((_maxattacks % 8) == 0)
			{
				if(target.hp < _prehp1)
					_prehp1 = target.hp;
				else
					return false;
			}
		}
		else
		{
			if((_maxattacks % 4) == 0)
			{
				if(target.hp < _prehp1)
					_prehp1 = target.hp;
				else
					return false;
			}
		}

		//print("--Sub Step 5");                

		_nohit = (target.hp >= _prehp2);

		if(NTConfig_CheckSafe)
		{
			if(!NTTMGR_CheckSafe(1))
				return false;
		}

		//print("--Sub Step 6");                
	}

	return true;
}


//NTA_BarbarianAttackInt() function written by kolton.  Thanks very much kolton!
function NTA_BarbarianAttackInt(target, boss, maxattacks)
{
   var _attackprimary;
   var _maxattacks = 0;
   var _usehand1, _usehand2;
   var _damagetype1, _damagetype2;
   var _range1, _range2;
   var _castx, _casty;

   _attackprimary = boss ? NTConfig_AttackBoss : NTConfig_AttackOthers;
   
   if(_attackprimary == 152)
      _damagetype1 = NTA_DAMAGE_MAGIC;
   else
      _damagetype1 = NTA_GetDamageType(_attackprimary);
   _usehand1 = NTC_IsLeftSkill(_attackprimary) ? 2 : NTC_HAND_RIGHT;
   
   if(NTConfig_AttackSecondary > -1)
   {
      _usehand2 = NTC_IsLeftSkill(NTConfig_AttackSecondary) ? 2 : NTC_HAND_RIGHT;
      if(NTConfig_AttackSecondary == 152)
         _damagetype2 = NTA_DAMAGE_MAGIC;
      else
         _damagetype2 = NTA_GetDamageType(NTConfig_AttackSecondary);
   }
   
   switch(_attackprimary)
   {
      case 151:
         _range1 = 7;
         break;
      default:
         _range1 = 3;
         break;
   }
   
   switch(NTConfig_AttackSecondary)
   {
      case 152:
         _range2 = 2;
         break;
      default:
         _range2 = 3;
         break;
   }
   
   while(_maxattacks++ < maxattacks && NTA_IsValidTarget(target, true) && target.hp > 0)
   {
      if(_attackprimary == 151 && NTA_GetResistance(target, _damagetype1) < 100)
      {
         if(getDistance(me, target) > _range1)
            NTA_MoveCloseInt(target, _range1);

         _castx =  target.x > me.x ? target.x+3 : target.x-3;
         _casty =  target.y > me.y ? target.y+3 : target.y-3;

         if(me.mp > 30)
            NTC_DoCast(_attackprimary, _usehand1, _castx, _casty);
         else{
            //NTA_MoveCloseInt(target, 2);
            NTC_DoCast(144, NTC_HAND_RIGHT, target);
         }
            
         if(_maxattacks%10 == 1 && NTConfig_WarcryBoss > 0 && (target.spectype&0x01 || boss) && target.hp > 0)
            NTC_DoCast(NTConfig_WarcryBoss, NTC_HAND_RIGHT, target);
         else if(_maxattacks%5 == 1 && NTConfig_WarcryOthers > 0 && target.hp > 0 && !boss && !(target.spectype&0x01))
            NTC_DoCast(NTConfig_WarcryOthers, NTC_HAND_RIGHT, target);
      }
      else if(_attackprimary != 151 && NTA_GetResistance(target, _damagetype1) < 100)
      {
         if(getDistance(me, target) > _range1)
            NTA_MoveCloseInt(target, _range1);

         NTC_DoCast(_attackprimary, _usehand1, target);
      }
      else if(NTConfig_AttackSecondary > -1 && NTA_GetResistance(target, _damagetype2) < 100)
      {
         if(getDistance(me, target) > _range2)
            NTA_MoveCloseInt(target, _range2);

         NTC_DoCast(NTConfig_AttackSecondary, _usehand2, target);
      }

      //probably needs some more checks added later
   
      if(NTConfig_CheckSafe)
      {
         if(!NTTMGR_CheckSafe(1))
            return false;
      }
      
      NTC_Delay(10);
   }

   NTSI_FastSnag();
   return true;
}

function NTA_DruidAttackInt(target, boss, maxattacks)
{ 
        var _attackprimary; 
        var _primaryindex = boss ? 1 : 2; 
        var _maxattacks = 0; 
        var _usehand1, _usehand2; 
        var _damagetype1, _damagetype2; 
        var _range1, _range2; 
        var _prehp; 

   _attackprimary = boss ? NTConfig_AttackBoss : NTConfig_AttackOthers; 

   _usehand1 = NTC_IsLeftSkill(_attackprimary) ? 2 : NTC_HAND_RIGHT; 
   _damagetype1 = NTA_GetDamageType(_attackprimary); 

   if(NTConfig_AttackSecondary > -1) 
   { 
      _usehand2 = NTC_IsLeftSkill(NTConfig_AttackSecondary) ? 2 : NTC_HAND_RIGHT; 
      _damagetype2 = NTA_GetDamageType(NTConfig_AttackSecondary); 
   } 

   switch(_attackprimary) 
   { 
   case 245: // Tornado 
      _range1 = 2; 
      break; 
   default: 
      _range1 = 2; 
      break; 
   } 

   switch(NTConfig_AttackSecondary) 
   { 
   case 245: // Tornado 
      _range2 = 2; 
      break; 
   default: 
      _range2 = 2; 
      break; 
   } 

    

   _prehp = target.hp; 

   while(_maxattacks++ < maxattacks && NTA_IsValidTarget(target, true)) 
   { 
      NTSI_FastSnag(); 

      if(NTA_GetResistance(target, _NTA_SkillDamage[_primaryindex]) < 100) 
      { 
         if((_maxattacks % 2) == 1) 
         { 
            if(!NTA_MoveCloseInt(target, _range1) && !boss) 
               return false; 
         } 

         NTA_DoCastInt(_primaryindex, target); 
          
         if (!me.getState(149))  //refresh oak sage 
             NTC_DoCast(226,0); 

    if (!me.getState(144))  //refresh hurricane 
             NTC_DoCast(250,0); 
          
         if (!me.getState(151))  //refresh Cyclone Armor 
             NTC_DoCast(235,0); 
      } 
      else if(NTConfig_AttackSkill[4] > -1 && NTA_GetResistance(target, _NTA_SkillDamage[4]) < 100) 
      { 
         if((_maxattacks % 2) == 1) 
         { 
            if(!NTA_MoveCloseInt(target, _range2) && !boss) 
               return false; 
         } 

         NTA_DoCastInt(4, target); 

    if (!me.getState(149))  //refresh oak sage 
             NTC_DoCast(226,0); 

    if (!me.getState(144))  //refresh hurricane 
             NTC_DoCast(250,0); 
          
         if (!me.getState(151))  //refresh Cyclone Armor 
             NTC_DoCast(235,0); 

      } 
      else 
      { 
         if(boss) 
         { 
            if((_maxattacks % 2) == 1) 
               NTA_MoveCloseInt(target, _range1); 

            NTA_DoCastInt(_primaryindex, target); 

       if (!me.getState(149))  //refresh oak sage 
                NTC_DoCast(226,0); 
  
       if (!me.getState(144))  //refresh hurricane 
                NTC_DoCast(250,0); 
          
            if (!me.getState(151))  //refresh Cyclone Armor 
                NTC_DoCast(235,0); 


         } 
         else 
            return false; 
      } 

      if(boss) 
      { 
         if((_maxattacks % 4) == 0 && target.hp >= _prehp) 
         { 
            if(target.classid == 526) 
               return false; 
         } 

         if((_maxattacks % 8) == 0) 
         { 
            if(target.hp < _prehp) 
               _prehp = target.hp; 
            else 
               return false; 
         } 
      } 
      else 
      { 
         if((_maxattacks % 4) == 0) 
         { 
            if(target.hp < _prehp) 
               _prehp = target.hp; 
            else 
               return false; 
         } 
      } 

      if(NTConfig_CheckSafe) 
      { 
         if(!NTTMGR_CheckSafe(1)) 
            return false; 
      } 
   } 
   NTSI_FastSnag(); 
   return true; 
}    

function NTA_AssassinAttackInt(target, boss, maxattacks)
{
	var _primaryindex;
	var _maxattacks = 0;
	var _range1, _range2;
	var _prehp;

	_primaryindex = boss ? 1 : 2;

	switch(NTConfig_AttackSkill[_primaryindex])
	{
	case 256: // Shock Field
	case 271: // Lightning Sentry
		_range1 = 10;
		break;
	default:
		_range1 = 20;
		break;
	}

	switch(NTConfig_AttackSkill[4])
	{
	case 256: // Shock Field
	case 271: // Lightning Sentry
		_range2 = 10;
		break;
	default:
		_range2 = 20;
		break;
	}

	_prehp = target.hp;

	while(_maxattacks++ < maxattacks && NTA_IsValidTarget(target, true) && target.hp > 0)
	{
		if(NTConfig_UseTraps)
			NTA_CheckTraps(target,boss);
		NTSI_FastSnag();
		if(!target.hp > 0)
			break;
		if(!getCollision(me.area, target.x,target.y))
		{
			if(boss)
				NTA_MoveCloseInt(target, Math.round(getDistance(me, target)/2));
			else if(me.area != 131)
				return false;
		}

		if(NTA_GetResistance(target, _NTA_SkillDamage[_primaryindex]) < 100)
		{
			if((_maxattacks % 2) == 1)
			{
				if(!NTA_MoveCloseInt(target, _range1) && !boss)
					return false;
			}

			NTA_DoCastInt(_primaryindex, target);
		}
		else if(NTConfig_AttackSkill[4] > -1 && NTA_GetResistance(target, _NTA_SkillDamage[4]) < 100)
		{
			if((_maxattacks % 2) == 1)
			{
				if(!NTA_MoveCloseInt(target, _range2) && !boss)
					return false;
			}

			NTA_DoCastInt(4, target);
		}
		else
		{
			if(boss)
			{
				if((_maxattacks % 2) == 1)
					NTA_MoveCloseInt(target, _range1);

				NTA_DoCastInt(_primaryindex, target);
			}
			else
				return false;
		}

		if(boss)
		{
			if((_maxattacks % 4) == 0 && target.hp >= _prehp)
			{
				if(target.classid == 526)
					return false;
			}

			if((_maxattacks % 8) == 0)
			{
				if(target.hp < _prehp)
					_prehp = target.hp;
				else
					return false;
			}
		}
		else
		{
			if((_maxattacks % 4) == 0)
			{
				if(target.hp < _prehp)
					_prehp = target.hp;
				else
					return false;
			}
		}

		if(NTConfig_CheckSafe)
		{
			if(!NTTMGR_CheckSafe(1))
				return false;
		}
	}

	return true;
}
function NTA_CheckTraps(target,boss)
{
	var traps=NTC_GetUnit(NTC_UNIT_MONSTER);
	var trapcount =0;
	var warr=0;
	var secondTrap =276;
	var x,y; 
	if (traps){
		do{
			if (traps.classid == 412 ||	traps.classid == 413 ||	traps.classid == 416 || traps.classid == 417){				
				if(traps.getParent() == me.name)
					if(getDistance(target.x,target.y,traps.x,traps.y)<35 && traps.hp > 100 )
					trapcount=trapcount+1;					
				
			}
			if (traps.classid == 418 || traps.classid == 419 && traps.mode != 12){
					warr=1;
			}
		}while(traps.getNext());
		
		if (trapcount <	5 && target.hp>0){
			x=target.x;  y=target.y;
			
		
		if(target.classid == 243 || target.classid == 544)
			secondTrap =271;
		NTSI_FastSnag();
			NTC_DoCast(258, NTC_HAND_RIGHT); //quickness
			
			NTSI_FastSnag();
			NTC_DoCast(271, NTC_HAND_RIGHT, x+2,y);
			NTC_DoCast(271, NTC_HAND_RIGHT, x+1,y);
			NTSI_FastSnag();
			NTC_DoCast(271, NTC_HAND_RIGHT, x,y);
			NTSI_FastSnag();
			NTC_DoCast(secondTrap, NTC_HAND_RIGHT, x-1,y);
			NTC_DoCast(secondTrap, NTC_HAND_RIGHT, x-2,y);
			if (NTConfig_CastFade)
			NTC_DoCast(267, NTC_HAND_RIGHT); //fade
			NTSI_FastSnag();
		}
		if (warr==0)
			NTP_UpdatePrecast();
	}

}
function NTA_MoveCloseInt(target, maxrange)
{
	var _dist = getDistance(me, target);
	
	if(_dist > maxrange)
	{
		var _destx, _desty;

		_destx = me.x + Math.round((_dist-maxrange)*(target.x-me.x) / _dist);
		_desty = me.y + Math.round((_dist-maxrange)*(target.y-me.y) / _dist);
    //    me.overhead("move closer")
        if (isPtWalkable(_destx,_desty))
		   return NTM_MoveTo(_destx, _desty, 0);
		var los = FindLOS(target,maxrange-1)
			if (los)
			    NTM_MoveTo(los[0], los[1]);
			
		if (getDistance(me, target)>maxrange)
			NTM_MoveTo(target.x,target.y);
		
	}

	return true;
}

function NTA_Initialize()
{
		if(typeof(NTConfig_PutAura) == 'string') NTConfig_PutAura = getSkillByName(NTConfig_PutAura);
		if(typeof(NTConfig_AttackFirst) == 'string') NTConfig_AttackFirst = getSkillByName(NTConfig_AttackFirst);
		if(typeof(NTConfig_AttackBoss) == 'string') NTConfig_AttackBoss = getSkillByName(NTConfig_AttackBoss);
		if(typeof(NTConfig_AttackOthers) == 'string') NTConfig_AttackOthers = getSkillByName(NTConfig_AttackOthers);
		if(typeof(NTConfig_AttackSecondary) == 'string') NTConfig_AttackSecondary = getSkillByName(NTConfig_AttackSecondary);
		if(typeof(NTConfig_AttackSkill[0]) == 'string') NTConfig_AttackSkill[0] = getSkillByName(NTConfig_AttackSkill[0]);
		if(typeof(NTConfig_AttackSkill[1]) == 'string') NTConfig_AttackSkill[1] = getSkillByName(NTConfig_AttackSkill[1]);
		if(typeof(NTConfig_AttackSkill[2]) == 'string') NTConfig_AttackSkill[2] = getSkillByName(NTConfig_AttackSkill[2]);
		if(typeof(NTConfig_AttackSkill[3]) == 'string') NTConfig_AttackSkill[3] = getSkillByName(NTConfig_AttackSkill[3]);
		if(typeof(NTConfig_AttackSkill[4]) == 'string') NTConfig_AttackSkill[4] = getSkillByName(NTConfig_AttackSkill[4]);
		if(typeof(NTConfig_AttackSkill[5]) == 'string') NTConfig_AttackSkill[5] = getSkillByName(NTConfig_AttackSkill[5]);
		if(typeof(XP_Curse) == 'string') XP_Curse = getSkillByName(XP_Curse);
		if(typeof(XP_BuildArmyCurse) == 'string') XP_BuildArmyCurse = getSkillByName(XP_BuildArmyCurse);
		
		
	if (me.classid == 1){
		//NTConfig_AttackFirst=NTConfig_AttackSkill[0];
		//NTConfig_AttackBoss=NTConfig_AttackSkill[1];
		//NTConfig_AttackOthers=NTConfig_AttackSkill[2];
		//NTConfig_AttackSecondary=NTConfig_AttackSkill[4];
		//if (NTConfig_AttackSecondary == 0) 
		//	NTConfig_AttackSecondary = NTConfig_AttackSkill[4];
	
		if (NTConfig_AttackSkill[0] == 0 && NTConfig_AttackSkill[1] == 0 && NTConfig_AttackSkill[2] == 0 && NTConfig_AttackSkill[3] == 0 && NTConfig_AttackSkill[4] == 0 ){
		// old config file
			NTConfig_AttackSkill[0] = NTConfig_AttackFirst;
			NTConfig_AttackSkill[1] = NTConfig_AttackBoss;
			NTConfig_AttackSkill[2] = NTConfig_AttackOthers;
			NTConfig_AttackSkill[4] = NTConfig_AttackSecondary;
		}
	}
	for(var i = 0 ; i < 6 ; i++)
	{
		if(NTConfig_AttackSkill[i] > -1)
		{
			_NTA_SkillHand[i] = NTC_GetBaseStat(3, NTConfig_AttackSkill[i], 31) ? 2 : NTC_HAND_RIGHT;
			_NTA_SkillDamage[i] = NTA_GetDamageType(NTConfig_AttackSkill[i]);
			_NTA_SkillDelay[i] = NTC_GetCastDelay(NTConfig_AttackSkill[i]);
		}
	}
}
function NTA_DoCastInt(index, target)
{
	var _untimedindex = index < 4 ? 3 : 5;
	if (NTConfig_AttackSkill[index] == 0 )
		return false;
        
        var bestrange = (bestAttackDistance[NTConfig_AttackSkill[index]]) ? bestAttackDistance[NTConfig_AttackSkill[index]] : 30  
        if (NTConfig_AttackSkill[index] == 64){ //Frozen Orb
        var los = FindLOS(target,bestrange)
        if (los)
	        NTM_MoveTo(los[0],los[1]);
        }else if (!checkLineLos(me.x,me.y,target.x,target.y) || getDistance(me,target)> bestrange){
        var los = FindLOS(target,bestrange)
        if (los)
	        NTM_MoveTo(los[0],los[1]);
        }
        if(!NTA_IsValidTarget(target))
        return false;
        if(!NTC_DoCast(NTConfig_AttackSkill[index], _NTA_SkillHand[index], target))
	        return false;
        
        if(_NTA_SkillDelay[index] > 0)
        {
	        if(NTConfig_AttackSkill[_untimedindex] > -1)
	        {
		        var _starttick = getTickCount();

		        while(NTA_IsValidTarget(target, true))
		        {
			        NTC_DoCast(NTConfig_AttackSkill[_untimedindex], _NTA_SkillHand[_untimedindex], target);

			        if(getTickCount()-_starttick >= _NTA_SkillDelay[index])
				        break;
		        }
	        }
	        else
		        NTC_Delay(_NTA_SkillDelay[index]);
        }

	return true;
}

function FindLOS(mob,range){
if (!mob) return false;
var LosPos =[];
var x 
var y
var denc=range*2
var checked =[]
for(var j = 0 ; j < denc; j = j+1){
    x=Math.round( range * Math.cos(6.28/denc*j)+mob.x)
    y=Math.round( range * Math.sin(6.28/denc*j)+mob.y)
   // x=Math.round(mob.x + range *(Math.cos(j)*(Math.PI/180)))
   // y=Math.round(mob.y + range *(Math.sin(j)*(Math.PI/180)))
   // print("checking "+x + " , "+y)
    checked.push([x,y])
    if(isPtWalkable(mob.x+j,mob.y+range)  && checkLineLos(mob.x,mob.y,x,y))
        LosPos.push([x,y])
}

//print("FindLos range : "+range + " mob("+mob.x +", "+mob.y+ ") results:" + LosPos.toSource() + " Checked "+checked.toSource())
//	for (var j =range*-1; j< range ; j=j+5){
//	// check laspoint walkable then los line
//		if (isPtWalkable(mob.x+j,mob.y+range)  && CheckColl(mob.x,mob.y,mob.x+j,mob.y+range)  && CheckColl(mob.x+1,mob.y,mob.x+j+1,mob.y+range) && CheckColl(mob.x-1,mob.y,mob.x+j-1,mob.y+range))
//			LosPos.push( [mob.x+j,mob.y+range])

//		if (isPtWalkable(mob.x + j, mob.y - range) && CheckColl(mob.x, mob.y, mob.x + j, mob.y - range) && CheckColl(mob.x+1,mob.y,mob.x+j+1,mob.y-range)&& CheckColl(mob.x-1,mob.y,mob.x+j-1,mob.y-range))
//			LosPos.push( [mob.x+j,mob.y-range])

//		if (isPtWalkable(mob.x + range, mob.y + j) && CheckColl(mob.x, mob.y, mob.x + range, mob.y + j) && CheckColl(mob.x,mob.y+1,mob.x+range,mob.y+j+1)&& CheckColl(mob.x,mob.y-1,mob.x+range,mob.y+j-1))
//			LosPos.push( [mob.x+range,mob.y+j])

//		if (isPtWalkable(mob.x - range, mob.y + j) && CheckColl(mob.x, mob.y, mob.x - range, mob.y + j) && CheckColl(mob.x,mob.y+1,mob.x-range,mob.y+j+1)&& CheckColl(mob.x,mob.y-1,mob.x-range,mob.y+j-1))
//			LosPos.push([mob.x-range,mob.y+j])
//	}	
	if(LosPos.length == 0 && range >6)
		return FindLOS(mob,range-5)
	if(LosPos.length ==0) return false;
	LosPos.sort(NTA_SortRoomInt);
	

		return LosPos[0];
	
return false
}
function blockLos(bit){
    return (!(bit %2 ==0 )) // not happy with this but it appears like map data is missing some preset los blocking info	
}
function isPtWalkable(x,y){

 try{ 
	if (ntMap.getColl(me.area,x,y) %2 ==0)
		return true;
	}catch (e){
		return false
	}
return false; 

}


function checkLineLos(x0, y0, x1, y1){
   if(! CheckColl(x0,y0,x1,y1)) return false;
    if (!CheckColl(x0+1,y0-1,x1+1,y1-1)) return false;
    if (!CheckColl(x0-1,y0+1,x1-1,y1+1)) return false;
  return true;
}
function CheckColl(x0, y0, x1, y1){
if (x0==x1) x0=x0-1;
if (y0==y1) y0=y0-1;
   var dx = Math.abs(x1-x0)
   var dy = Math.abs(y1-y0) 
   var sx = (x0 < x1) ? 1:-1
   var sy = (y0 < y1) ? 1 : -1
   var err = dx-dy
	var e2 =0
   while (x0 != x1 && y0 != y1 ){
     if (blockLos(ntMap.getColl(me.area,x0,y0))){
    // if (blockLos(getColl(me.area,x0,y0))){
		//print("blockLos retruning false "+getColl(me.area,x0,y0))
		return false;	
     }
	 e2 = 2*err
     if (e2 > ( -1 *dy)){ 
       err= err - dy
       x0 = x0 + sx
     }
     if (e2 <  dx){ 
       err = err + dx
       y0 = y0 + sy 
     }
   }
  return true;
 }


//Thanks and credit goes to menellerhur for this function and it's implementation in the paladin attack function;
function NTA_GoToOtherSide(target) 
{
	var toX;
	var toY;
	var directionX;
	var directionY;
	var merc = me.getMerc();
	
	directionX = (target.x - merc.x);
	directionY = (target.y - merc.y);

	if (directionX < 0)
	  directionX = -1;
	else
	  directionX = 1;

	if (directionY < 0)
	  directionY = -1;
	else
	  directionY = 1;
			   
	toX = target.x + 2 * directionX;
	toY = target.y + 2 * directionY;

	NTC_DoClick(NTC_CLICK_LDOWN, NTC_SHIFT_NONE, toX, toY);

	var delayLoops = 25;
	
	while ((me.mode == 2 || me.mode == 3 || me.mode == 6) && delayLoops > 0)
	{
	  delay(20);
	  delayLoops--;
	}
}

function NTA_GetAttackAuraInt(skillid)
{
	function haveSkill(skillid){ return ( me.getSkill(skillid,1) > 0 ); };
	
	if(NTConfig_PutAura > 0)
		return NTConfig_PutAura;

	switch(skillid)
	{
			case 97: return 122; 		//	case 'Smite', return 'Fanaticism'
			case 101: 					//	case 'Holy Bolt', if i have 'Fanaticism' return 'Fanaticism', else return 'Concentration'
					if(haveSkill(122))
						return 122;
					else
						return 113;
			case 106: return 122; 		//	case 'Zeal', return 'Fanaticism'
			case 112: return 113; 		//	case 'Blessed Hammer', return 'Concentration'
			
			default: break;
	}
	
	return 0;	//anything else, return 0;
}

function Array2D(ymin,ysize){
    var k=new Array;  
    for (var i = ymin; i < ymin+ysize; ++ i)
        k [i] = new Array; 
    return k; 
}
function collMap(Area){
    this.area = Area;
    this.rooms =[];
    var myarea=getArea(this.area);
    var myrooms = getRoom(Area);	
    this.Map =false//= buildCmap(Area)
    this.minY =0;
    this.minX=0;
    this.maxX=0;
    this.maxY=0;
    
}
collMap.prototype.clear = function(){ // resets map array and bounds
    var myarea=getArea(this.area);
    this.Map=false;
    this.minX = myarea.x*5
    this.minY = myarea.y*5
    this.maxY=this.minY + myarea.ysize*5
    this.maxX=this.minX+myarea.xsize*5
    this.Map= Array2D(myarea.x*5-40,myarea.xsize*5+myarea.x*5+40)	
}
collMap.prototype.isValidPt = function(x,y){
    return (x>this.minX && x < this.maxX && y>this.minY && y<this.maxY)
}
collMap.prototype.getColl = function(area,x,y){
if (this.area != area){
    this.area=area;
    this.clear()
   // this.rebuild(area)
    
    }
    try{
        if (this.Map[x] === undefined || this.Map[x][y] === undefined){
           this.addRoom(x,y)// try adding data  .... this works but seems un-needed when building hole area cmap
                if (this.Map[x] === undefined || this.Map[x][y] === undefined)
                    return 99;
                else 
                    return this.Map[x][y];
            return 99;
        } else
          return this.Map[x][y];
    }catch(e){
        print("returning 99 error " +x + " " + y)
        return 99; 
    }

}
collMap.prototype.addRoom = function(x,y){
if (!this.isValidPt(x,y))
    return false
 
   // var room = getRoom(this.area,x,y) // broken in 1.4
   var room = getRoom(this.area)
   do{
        if( room.x*5 <= x && (room.x*5) + room.xsize > x && room.y*5 <= y &&  (room.y*5)+room.ysize>y){
            //print("adding room "+ this.area +","+x+","+y +room.toSource())
            this.addRoomData(room)
            return true;        
        } 
    } while(room.getNext());
   return false;
}
collMap.prototype.addRoomData = function(room){
    var col = room.getCollision();	
    for (var yy =0; yy < room.ysize ; yy++){
		for (var xx =0; xx < room.xsize; xx++){			 			
			    try{
				    this.Map[(room.x*5+xx)][(room.y*5+yy)] = col[yy][xx];	
			    }catch(e){
				    print (e + "x:"+(room.x*5+xx) +"y: "+(room.y*5+yy))
			    }
		 }
	 }		
 }

collMap.prototype.rebuild = function( area){
    var startTime=getTickCount()
   
    var myrooms = getRoom(area);
   	
	do
	{	   
       if (this.area ===108 &&  (myrooms.x*5 > 7600 && myrooms.x*5 < 7950 &&  myrooms.y*5 >5100 )&&! (myrooms.x*5 <7740 && myrooms.y *5>5330) && !(myrooms.x*5 >7820 && myrooms.y *5>5330) && !(myrooms.x*5 >7820 && myrooms.y *5<5230) && !(myrooms.x*5  <7740 && myrooms.y *5<5230))
            this.addRoomData(myrooms)
       if (this.area !== 108)
             this.addRoomData(myrooms)
           
	} while(myrooms.getNext());
print("Generated cmap in "+(getTickCount()-startTime))

}
collMap.prototype.Dump = function(){
   print("Dumping map output//lvlDump2.txt")
	    var	file = File.open( "output//lvlDump2.txt", 1 );
	    if(file) {
	    var line; 
	    var car;
	 
      for(var y = this.minY; y< this.maxY; y++){
		    line = "y:"+ y+ ">";
			for(var x=this.minX; x< this.maxX; x++){
			     if (this.Map[x][y] ){					
					    if (this.Map[x][y] < 10)	
						    car = this.getColl(this.area,x,y) + " "
					    else 
						    car =this.Map[x][y]
					    if (this.Map[x][y] > 99) 
						    car = "$$"
					
				    }else
					    car = "  ";
				    line = line+car;
			    }
			    line = line+"<\n"
			    file.write(line);
			
		    }
		    file.close();
	    }
	    print("lvl dump complete.");
}

function SortMonsters(nUnit1, nUnit2) {  // taken from Mbot cred goes to McGod
		if (nUnit1.spectype & 0x01 || nUnit1.spectype & 0x02 || nUnit1.spectype & 0x04)
			return 1;
		if (nUnit2.spectype & 0x01 || nUnit2.spectype & 0x02 || nUnit2.spectype & 0x04)
			return -1;
		var nResIDs = [58,59,60,61,62,101,102,103,104,105,229,278,279,280,281,282,645,646,647,667,668,669,670];
		for (var n in nResIDs) {
			if (nResIDs[n] == nUnit1.classid)
				return 1;
			if (nResIDs[n] == nUnit2.classid)
				return -1;
		}
		if (getDistance(me, nUnit1) < getDistance(me, nUnit2))
			return 1;
		return -1;
}
function NTA_SortRoomInt(a, b)
{
	if(getDistance(me.x, me.y, a[0], a[1]) < getDistance(me.x, me.y, b[0], b[1]))
		return -1;

	return 1;
}
function SortDistance(nUnit1, nUnit2){
     if (SortPoint ){
        if (getDistance(me.x,me.y, nUnit1.x,nUnit1.y)+getDistance(me.x,me.y,SortPoint.x,SortPoint.y) > getDistance(me.x,me.y, nUnit2.x,nUnit2.y)+getDistance(me.x,me.y,SortPoint.x,SortPoint.y))
	        return 1;
        return -1;
    }else {
        if (getDistance(me.x,me.y, nUnit1.x,nUnit1.y) > getDistance(me.x,me.y, nUnit2.x,nUnit2.y))
	        return 1;
        return -1;
    }
}
function SortDistanceRev(nUnit1, nUnit2){
    if (SortPoint ){
        if (getDistance(me.x,me.y, nUnit1.x,nUnit1.y)+getDistance(me.x,me.y,SortPoint.x,SortPoint.y) < getDistance(me.x,me.y, nUnit2.x,nUnit2.y)+getDistance(me.x,me.y,SortPoint.x,SortPoint.y))
	        return 1;
        return -1;
    }else {
        if (getDistance(me.x,me.y, nUnit1.x,nUnit1.y) < getDistance(me.x,me.y, nUnit2.x,nUnit2.y))
	        return 1;
        return -1;
    }

}
function staticTarget(name){
    if (NTConfigStaticBoss.length == 0)
        return true
    for (var i =0; i< NTConfigStaticBoss.length; i++)
        if(name == NTConfigStaticBoss[i])
            return true
    return false;    
}