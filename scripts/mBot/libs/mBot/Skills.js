var Skill = {
	Attack:0, Kick:1, Throw:2, Unsummon:3, LeftHandThrow:4, LeftHandSwing:5,
	
	MagicAarow:6, FireAarow:7, InnerSight:8, CriticalStrike:9, Jab:10, ColdAarow:11, MultipleShot:12, 
	Dodge:13, PowerStrike:14, PoisonJavelin:15, ExplodingAarow:16, SlowMissiles:17, 
	Avoid:18, Impale:19, LightningBolt:20, IceAarow:21, GuidedAarow:22, Penetrate:23, 
	ChargedStrike:24, PlagueJavelin:25, Strafe:26, ImmolationAarow:27, Decoy:28, Evade:29, 
	Fend:30, FreezingAarow:31, Valkyrie:32, Pierce:33, LightningStrike:34, LightningFury:35,
	
	FireBolt:36, Warmth:37, ChargedBolt:38, IceBolt:39, FrozenArmor:40, Inferno:41, StaticField:42,
	Telekinesis:43, FrostNova:44, IceBlast:45, Blaze:46, FireBall:47, Nova:48, Lightning:49, 
	ShiverArmor:50, FireWall:51, Enchant:52, ChainLightning:53, Teleport:54, GlacialSpike:55,
	Meteor:56, ThunderStorm:57, EnergyShield:58, Blizzard:59, ChillingArmor:60, FireMastery:61,
	Hydra:62, LightningMastery:63, FrozenOrb:64, ColdMastery:65, 
	
	AmplifyDamage:66, Teeth:67, BoneArmor:68, SkeletionMastery:69, RaiseSkeleton:70, DimVision:71,
	Weaken:72, PoisonDagger:73, CorpseExplosion:74, ClayGolem:75, IronMaiden:76, Terror:77, BoneWall:78,
	GolemMastery:79, RaiseSkeletalMage:80, Confuse:81, LifeTap:82, PoisonExplosion:83, BoneSpear:84,
	BloodGolem:85, Attract:86, Decrepify:87, BonePrison:88, SummonResist:89, IronGolem:90, 
	LowerResist:91, PoisonNova:92, BoneSpirit:93, FireGolem:94, Revive:95,
	
	Sacrifice:96, Smite:97, Might:98, Prayer:99, ResistFire:100, HolyBolt:101, HolyFire:102, 
	Thorns:103, Defiance:104, ResistCold:105, Zeal:106, Charge:107, BlessedAim:108, Cleansing:109,
	ResistLightning:110, Vengeance:111, BlessedHammer:112, Concentration:113, HolyFreeze:114,
	Vigor:115, Converison:116, HolyShield:117, HolyShock:118, Sanctuary:119, Meditation:120,
	FistofTheHeavens:121, Fanaticism:122, Conviction:123, Redemption:124, Salvation:125, 
	
	Bash:126, SwordMastery:127, AxeMastery:128, MaceMastery:129, Howl:130, FindPotion:131, 
	Leap:132, DoubleSwing:133, PoleArmMastery:134, ThrowingMastery:135, SpearMastery:136,
	Taunt:137, Shout:138, Stun:139, DoubleThrow:140, IncreasedStamina:141, FindItem:142,
	LeapAttack:143, Concentrate:144, IronSkin:145, BattleCry:146, Frenzy:147, 
	IncreasedSpeed:148, BattleOrders:149, GrimWard:150, Whirlwind:151, Berserk:152, 
	NaturalResistance:153, Warcry:154, BattleCommand:155, 
	
	ScrollofTownPortal:219, BookOfTownPortal:220, 
	
	Raven:221, PoisonCreeper:222, Werewolf:223, ShapeShifting:224, Firestorm:225, OakSage:226,
	SummonSpiritWolf:227, Werebear:228, MoltenBoulder:229, ArcticBlast:230, CarrionVine:231,
	FeralRage:232, Maul:233, Fissure:234, CycloneArmor:235, HeartOfWolverine:236,
	SummonDireWolf:237, Rabies:238, FireClaws:239, Twister:240, SolarCreeper:241,
	Hunger:242, ShockWave:243, Volcano:244, Tornado:245, SpiritOfBarbs:246, SummonGrizzly:247,
	Fury:248, Armageddon:249, Hurricane:250,
	
	FireBlast:251, ClawMastery:252, PsychicHammer:253, TigerStrike:254, DragonTalon:255, 
	ShockWeb:256, BladeSentinel:257, BurstOfSpeed:258, FistsOfFire:259, DragonClaw:260,
	ChargedBoltSentry:261, WakeOfFire:262, WeaponBlock:263, CloakOfShadows:264, 
	CobraStrike:265, BladeFury:266, Fade:267, ShadowWarrior:268, ClawsOfThunder:269,
	DragonTail:270, LightningSentry:271, WakeOfInferno:272, MindBlast:273, BladesOfIce:274,
	DragonFlight:275, DeathSentry:276, BladeShield:277, Venom:278, ShadowMaster:279, PheonixStrike:280,
	
	SkillType:{Ranged:0, Combat:1, Aura:2, Mastery:3, Summon:4, Curse:5, Scroll:6},
	SkillTypeName:["Ranged", "Combat", "Aura", "Mastery", "Summon", "Curse", "Scroll"],
	SkillElem:{None:0, Fire:1, Lightning:2, Magic:3, Cold:4, Poison:5, Stun:9},
	SkillElemName:["None", "Fire", "Lightning", "Magic", "Cold", "Poison", "Unknown", "Unknown", "Unknown", "Stun"],
	Range:{None:0, Range:1, HandToHand:2, Both:3},
	RangeName:[ "none", "rng", "h2h", "both" ]
}
Skill.get = function (nId) { return new SkillClass(nId); };


var SkillClass = function(id) {
	this.id = id;
	this.name = getLocaleString(getBaseStat(2, this.id, "str name"));
	this.level = me.getSkill(this.id, 1);
	this.base = me.getSkill(this.id, 0);
	this.classId = getBaseStat(3, this.id, "charclass");
	
	this.SkillType = {Ranged:0, Combat:1, Aura:2, Mastery:3, Summon:4, Curse:5, Scroll:6}
	this.SkillTypeName = ["Ranged", "Combat", "Aura", "Mastery", "Summon", "Curse", "Scroll"];
	this.SkillElem = {None:0, Fire:1, Lightning:2, Magic:3, Cold:4, Poison:5, Stun:9};
	this.SkillElemName = ["None", "Fire", "Lightning", "Magic", "Cold", "Poison", "Unknown", "Unknown", "Unknown", "Stun"];
	this.Range = {None:0, Range:1, HandToHand:2, Both:3}
	this.Range.Name = [ "none", "rng", "h2h", "both" ];
	
	this.getType = function () {
		if (getBaseStat(3, this.id, "aura") == 1)
			return (arguments.length == 1) ? this.SkillTypeName[2]: this.SkillType.Aura;
		//if (getBaseStat(3, this.id, "passivestate") > 0)
		//	return (arguments.length == 1) ? this.SkillTypeName[3]: this.SkillType.Mastery;
		//if (getBaseStat(3, this.id, "summon") > 0)
		//	return (arguments.length == 1) ? this.SkillTypeName[4]: this.SkillType.Summon;
		if (getBaseStat(3, this.id, "cltstfunc") == 18)
			return (arguments.length == 1) ? this.SkillTypeName[5]: this.SkillType.Curse;
		if (getBaseStat(3, this.id, "scroll") > 0)
			return (arguments.length == 1) ? this.SkillTypeName[6]: this.SkillType.Scroll;
		if (getBaseStat(3, this.id, "srvmissile") > 0 || 
			getBaseStat(3, this.id, "srvmissilea") > 0 || 
			getBaseStat(3, this.id, "srvmissileb") > 0 || 
			getBaseStat(3, this.id, "srvmissilec") > 0)
			return (arguments.length == 1) ? this.SkillTypeName[0]: this.SkillType.Ranged;
		return (arguments.length == 1) ? this.SkillTypeName[1]: this.SkillType.Combat;
	}
	
	this.getManaCost = function() {
		var skillLvl = me.getSkill(this.id, 3);
		var effectiveShift = [1,2,4,8,16,32,64,128,256,512,1024];
		var ret = Math.max((getBaseStat(3, this.id, "mana") + (getBaseStat(3, this.id, "lvlmana") *(skillLvl-1))) * (effectiveShift[getBaseStat(3, this.id, "manashift")] / 256), getBaseStat(3, this.id, "minmana"));
		return ret;	
	}
	
	this.getRange = function () {
		return getBaseStat(3, this.id, "range");
	}
	
	this.getRangeName = function () {
		return this.Range.Name[this.range];
	}
	
	this.getElemType = function () {
		return (arguments.length == 1) ? this.SkillElemName[getBaseStat(3, this.id, "EType")] : getBaseStat(3, this.id, "EType");
	}	
	
	this.inTown = function () {
		return (getBaseStat(3, this.id, "InTown"));
	}
	
	this.isObtainable = function () {
		if (this.classId != me.classid)
			return false;
		if (me.getStat(12) < getBaseStat(3, this.id, "reqlevel"))
			return false;
		for (var n = 1; n < 4; n++) {
			var nSkillId = getBaseStat(3, this.id, "reqskill" + n);
			if (nSkillId > 0 && me.getSkill(nSkillId, 0) == 0)
				return false;
		}
		return true;
	}
	
	this.getState = function () {
		return getBaseStat(3, this.id, "aurastate");
	}
	
	this.setSkill = function() {
		if (arguments.length == 1)
			this.hand = arguments[0];
		else
			this.hand = Hand.Right;
		if (me.getSkill(((this.hand == Hand.Right) ? 2 : 3)) == this.id)
			return true;
		if (me.setSkill(this.id, this.hand))
			return true;
		return false;
	}

	this.multiCount = function () {
		this.list = {221:5, 227:5, 237:3};
		if (this.list.hasOwnProperty(this.id))
			return (this.level > this.list[this.id]) ? this.list[this.id] : this.level;
		return 1;
	}
	
	this.cast = function () {
		if (arguments.length > 0)
			this.hand = arguments[0];
		else
			this.hand = Hand.Right;
		this.loc = {x:me.x + 3, y:me.y + 3};
		if (arguments.length == 3)
			this.loc = {x:arguments[1], y:arguments[2]};
		else if (arguments.length == 2)
			this.loc = copyUnit(arguments[1]);
		this.clickType = (this.hand == Hand.Left) ? ClickType.Left : ClickType.Right;
		this.shift = (this.hand == Hand.Left) ? Shift.On : Shift.Off;
		//Let's make sure actually have the skill!
		if (this.level == 0) {
			Interface.message(Debug, "Unable to cast " + this.name + " because we do not have the skill!");
			return false;
		}
		
		//Check to make sure we can cast this skill if were in town.
		if (getBaseStat(3, this.id, "InTown") == 0 && mBot.inTown()) {
			Interface.message(Debug, "Unable to cast " + this.name + " because we are in town.");
			return false;
		}
			
		//Check to make sure we have enough mana
		//if (this.getManaCost(this.id) > me.mp) {
		//	Interface.message(Debug, "Unable to cast " + this.name + " because we do not have enough mana.");
		//	return false;
		//}
		
		
		//Attempt to place skill on hand.
		if(!me.setSkill(this.id, this.hand)) {
			Interface.message(Debug, "Unable to cast " + this.name + " because we couldn't set the skill.");
			return false;
		}
		
		//Lets do this shit.
		if (arguments.length == 2)
			clickMap(this.clickType.Down, this.shift, this.loc);
		else
			clickMap(this.clickType.Down, this.shift, this.loc.x, this.loc.y);
		delay(10,true);
		if (arguments.length == 2)
			clickMap(this.clickType.Up, this.shift, this.loc);
		else
			clickMap(this.clickType.Up, this.shift, this.loc.x, this.loc.y);
		
		//Who cares if we have finished casting a hammer?
		if (this.id == Skill.BlessedHammer && arguments.length == 1)
			return true;
		
		//Wait until we're done casting.
		while(mBot.checkMode(Mode.Player.Cast))
			delay(1,true);
		return true;
	}
}