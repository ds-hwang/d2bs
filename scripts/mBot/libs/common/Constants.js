//This is just an object verison of lord2800's constants.dbl
//All credits go to him

var NPCMenu = {
	Null: 0,
	Talk: 0x0D35,
	Trade: 0x0D44,
	TradeRepair: 0x0D06,
	Imbue: 0x0FB1,
	Gamble: 0x0D46,
	Hire: 0x0D45,
	GoEast: 0x0D36,
	GoWest: 0x0D37,
	IdentifyItems: 0x0FB4,
	SailEast: 0x0D38,
	SailWest: 0x0D39,
	RessurectMerc: 0x1507,
	AddSockets: 0x58DC,
	Personalize: 0x58DD,
	TravelToHarrogath: 0x58D2
};

var Mode = {}
Mode.Player = {Death:0, Neutral:1, Walk:2, Run:3, GetHit:4, TownNeutral:5, TownWalk:6, Attack1:7, Attack2:8, Block:9, Cast:10,
Throw:11, Kick:12, Skill1:13, Skill2:14, Skill3:15, Skill4:16, Dead:17, Sequence:18, KnockBack:19};

Mode.Player.Group = {Neutral:[Mode.Player.Neutral, Mode.Player.TownNeutral], Move:[Mode.Player.Walk, Mode.Player.TownWalk, Mode.Player.Run, Mode.Player.Cast],Attack:
[Mode.Player.Attack1, Mode.Player.Attack2, Mode.Player.Cast, Mode.Player.Throw, Mode.Player.Kick, Mode.Player.Skill1, Mode.Player.Skill2, Mode.Player.Skill3, Mode.Player.Skill4],
Death:[Mode.Player.Death, Mode.Player.Dead], Hit:[Mode.Player.GetHit, Mode.Player.Block, Mode.Player.Sequence, Mode.Player.KnockBack]}

Mode.Monster = {Death:0, Neutral:1, Walk:2, GetHit:3, Attack1:4, Attack2:5, Block:6, Cast:7, Skill1:8, Skill2:9, Skill3:10, Skill4:11, Dead:12,
Knockback:13, Sequence:14, Run:15};

Mode.Monster.Group = {Neutral:[Mode.Monster.Neutral], Move:[Mode.Monster.Walk], Death:[Mode.Monster.Death, Mode.Monster.Dead], Attack:[Mode.Monster.Attack1,
Mode.Monster.Attack2, Mode.Monster.Cast, Mode.Monster.Skill1, Mode.Monster.Skill2, Mode.Monster.Skill3, Mode.Monster.Skill4]};

Mode.Object = {Neutral:0, Operating:1, Opened:2, Special1:3, Special2:4, Special3:5, Special4:6, Special5:7};

Mode.Item = {PickedUp:0,Equipped:1,InBelt:2,OnGround:3,OnCursor:4,BeingDropped:5,InSocket:6}
Mode.Item.Group = {Ground:[Mode.Item.OnGround, Mode.Item.BeingDropped], Picked:[Mode.Item.PickedUp, Mode.Item.Equipped, Mode.Item.InBelt, Mode.Item.OnCursor, Mode.Item.InSocket]};


var Area = {};

Area.Act1 = {RogueEncampment:1, BloodMoor:2, ColdPlains:3, StonyField:4, DarkWood:5, BlackMarsh:6, TamoeHighland:7, DenOfEvil:8, CaveLevel1:9,
UndergroundPassageLevel1:10, HoleLevel1:11, PitLevel1:12, CaveLevel2:13, UndergroundPassageLevel2:14, HoleLevel2:15, PitLevel2:16,
BurialGrounds:17, Crypt:18, Mausoleum:19, ForgottenTower:20, TowerCellarLevel1: 21, TowerCellarLevel2:22, TowerCellarLevel3:23,
TowerCellarLevel4:24, TowerCellarLevel5:25, MonasteryGate:26, OuterCloister:27, Barracks:28, JailLevel1:29, JailLevel2:30,
JailLevel3:31, InnerCloister:32, Cathedral:33, CatacombsLevel1:34, CatacombsLevel2:35, CatacombsLevel3: 36, CatacombsLevel4: 37, Tristram:38, CowLevel:39};
Area[1] = Area.Act1;

Area.Act1.NPCs = {
	Akara: 148,
	Charsi: 154,
	Gheed: 147,
	DeckardCain: 265,
	Kashya: 150,
	Warriv: 155
}

Area.Act1.NPCNames = {
	148:"Akara",
	154:"Charsi",
	147:"Gheed",
	265:"Deckard Cain",
	150:"Kashya",
	155:"Warriv"
}

Area.Act1.Jobs = {	
	Heal:Area.Act1.NPCs.Akara,
	Gamble:Area.Act1.NPCs.Gheed,
	Repair:Area.Act1.NPCs.Charsi,
	Pots:Area.Act1.NPCs.Akara,
	Aarows:Area.Act1.NPCs.Charsi,
	Merc:Area.Act1.NPCs.Kashya,
	Scrolls:Area.Act1.NPCs.Akara,
	Cain:Area.Act1.NPCs.DeckardCain
}

Area.Act1.Tiles = {
	CatacombsUp:18,
	Tower:10,
	ToTower:12,
	TowerUp:9
}

Area.Act1.Interact = {
	Heal:NPCMenu.Null,
	Gamble:NPCMenu.Gamble,
	Repair:NPCMenu.TradeRepair,
	Pots:NPCMenu.Trade,
	Aarows:NPCMenu.TradeRepair,
	Merc:NPCMenu.RessurectMerc,
	Scrolls:NPCMenu.Trade,
	Cain:NPCMenu.IdentifyItems
}

Area.Act1.Objects = {
	Waypoint:119,
	Stash:267
}

//---------------------------------------------------------------------------
//Act 2 constants
//All Areas, NPCs, Objects, etc.. declared here
//---------------------------------------------------------------------------
Area.Act2 = {LutGholein:40, RockyWaste:41, DryHills:42, FarOasis:43, LostCity:44, ValleyOfSnakes:45, CanyonOfTheMagi:46, SewersLevel1:47,
SewersLevel2:48, SewersLevel3:49, HaremLevel1: 50, HaremLevel2:51, PalaceCellarLevel1:52, PalaceCellarLevel2:53, PalaceCellarLevel3:54,
StonyTombLevel1:55, HallsOfTheDeadLevel1:56, HallsOfTheDeadLevel2:57, ClawViperTempleLevel1:58, StonyTombLevel2:59, HallsOfTheDeadLevel3:60,
ClawViperTempleLevel2:61, MaggotLairLevel1:62, MaggotLairLevel2:63, MaggotLairLevel3:64, AncientTunnels:65, TalRashasTomb1:66, TalRashasTomb2:67,
TalRashasTomb3:68, TalRashasTomb4:69, TalRashasTomb5:70, TalRashasTomb6:71, TalRashasTomb7:72, DurielsLair:73, ArcaneSanctuary:74};
Area[2] = Area.Act2;

Area.Act2.NPCs = {
	Fara: 178,
	Lysander: 202,
	DeckardCain: 244,
	Greiz: 198,
	Elzix: 199,
	Drognan: 177
};

Area.Act2.NPCNames = {
	178:"Fara",
	202:"Lysander",
	244:"Deckard Cain",
	198:"Greiz",
	199:"Elzix",
	177:"Drognan"
};

Area.Act2.Jobs = {	
	Heal:Area.Act2.NPCs.Fara,
	Gamble:Area.Act2.NPCs.Elzix,
	Repair:Area.Act2.NPCs.Fara,
	Pots:Area.Act2.NPCs.Lysander,
	Aarows:Area.Act2.NPCs.Fara,
	Merc:Area.Act2.NPCs.Greiz,
	Scrolls:Area.Act2.NPCs.Drognan,
	Cain:Area.Act2.NPCs.DeckardCain
}

Area.Act2.Interact = {
	Heal:NPCMenu.Null,
	Gamble:NPCMenu.Gamble,
	Repair:NPCMenu.TradeRepair,
	Pots:NPCMenu.Trade,
	Aarows:NPCMenu.TradeRepair,
	Merc:NPCMenu.RessurectMerc,
	Scrolls:NPCMenu.Trade,
	Cain:NPCMenu.IdentifyItems
}

Area.Act2.Objects = {
	Waypoint:156,
	Stash:267
}


//---------------------------------------------------------------------------
//Act 3 constants
//All Areas, NPCs, Objects, etc.. declared here
//---------------------------------------------------------------------------
Area.Act3 = {KurastDocktown:75, SpiderForest:76, GreatMarsh:77, FlayerJungle:78, LowerKurast:79, KurastBazaar:80, UpperKurast:81, KurastCauseway:82,
Travincal:83, SpiderCave:84, SpiderCavern:85, SwampyPitLevel1:86, SwampyPitLevel2:87, FlayerDungeonLevel1:88, FlayerDungeonLevel2:89, SwampyPitLevel3:90,
FlayerDungeonLevel3:91, SewersLevel1:92, SewersLevel2:93, RuinedTemple:94, DisusedFane:95, ForgottenReliquary:96, ForgottenTemple:97,
RuinedFane:98, DisusedReliquary:99, DuranceOfHateLevel1:100, DuranceOfHateLevel2:101, DuranceOfHateLevel3:102};
Area[3] = Area.Act3;

Area.Act3.NPCs = {
	Ormus: 255,
	DeckardCain: 245,
	Asheara:252,
	Alkor:254,
	Hratli:253
}

Area.Act3.NPCNames = {
	255:"Ormus",
	245:"Deckard Cain",
	252:"Asheara",
	254:"Alkor",
	253:"Hratli"
}	

Area.Act3.Tiles = {
	DuranceTile1:67,
	DuranceTile2:68
}

Area.Act3.Jobs = {	
	Heal:Area.Act3.NPCs.Ormus,
	Gamble:Area.Act3.NPCs.Alkor,
	Repair:Area.Act3.NPCs.Hratli,
	Pots:Area.Act3.NPCs.Ormus,
	Aarows:Area.Act3.NPCs.Asheara,
	Merc:Area.Act3.NPCs.Asheara,
	Scrolls:Area.Act3.NPCs.Ormus,
	Cain:Area.Act3.NPCs.DeckardCain
}

Area.Act3.Interact = {
	Heal:NPCMenu.Null,
	Gamble:NPCMenu.Gamble,
	Repair:NPCMenu.TradeRepair,
	Pots:NPCMenu.Trade,
	Aarows:NPCMenu.Trade,
	Merc:NPCMenu.RessurectMerc,
	Scrolls:NPCMenu.Trade,
	Cain:NPCMenu.IdentifyItems
}

Area.Act3.Objects = {
	Waypoint:237,
	Stash:267
}
	
//Everything for Act 4
Area.Act4 = {ThePandemoniumFortress:103, OuterSteppes:104, PlainsOfDespair:105, CityOfTheDamned:106, RiverOfFlame:107, ChaosSanctuary:108};
Area[4] = Area.Act4;

Area.Act4.NPCs = {
	Jamella:405,
	Halbu:257,
	DeckardCain:246,
	Tyrael:367
}

Area.Act4.NPCNames = {
	405:"Jamella",
	257:"Halbu",
	246:"Deckard Cain",
	367:"Tyrael"
}

Area.Act4.Jobs = {	
	Heal:Area.Act4.NPCs.Jamella,
	Gamble:Area.Act4.NPCs.Jamella,
	Repair:Area.Act4.NPCs.Halbu,
	Pots:Area.Act4.NPCs.Jamella,
	Aarows:Area.Act4.NPCs.Halbu,
	Merc:Area.Act4.NPCs.Tyrael,
	Scrolls:Area.Act4.NPCs.Jamella,
	Cain:Area.Act4.NPCs.DeckardCain
}

Area.Act4.Interact = {
	Heal:NPCMenu.Null,
	Gamble:NPCMenu.Gamble,
	Repair:NPCMenu.TradeRepair,
	Pots:NPCMenu.Trade,
	Aarows:NPCMenu.TradeRepair,
	Merc:NPCMenu.RessurectMerc,
	Scrolls:NPCMenu.Trade,
	Cain:NPCMenu.IdentifyItems
}

Area.Act4.Objects = {
	Waypoint:398,
	Stash:267
}

Area.Act5 = {Harrogath: 109, BloodyFoothills: 110,FrigidHighlands: 111,ArreatPlateau: 112,CrystalizedCavernLevel1: 113,CellarOfPity: 114,
	CrystalizedCavernLevel2: 115,EchoChamber: 116,TundraWastelands: 117,GlacialCavesLevel1: 118,GlacialCavesLevel2: 119,ArreatSummit: 120,
	NihlathaksTemple: 121,HallsOfAnguish: 122,HallsOfPain: 123,HallsOfVaught: 124,Hell1: 125,Hell2: 126,Hell3: 127,
	TheWorldStoneKeepLevel1: 128,TheWorldStoneKeepLevel2: 129,TheWorldStoneKeepLevel3: 130,ThroneOfDestruction: 131,TheWorldstoneChamber: 132};
Area[5] = Area.Act5;

Area.Act5.NPCs = {
	Malah: 513,
	Larzuk: 511,
	QualKehk: 515,
	DeckardCain: 520,
	Anya: 512
};

Area.Act5.NPCNames = {
	513:"Malah",
	511:"Larzuk",
	515:"Qual Kehk",
	520:"Deckard Cain",
	512:"Anya"
}
Area.Act5.Tiles = {
	WorldStoneUp:81,
	WorldStoneDown:82
}

Area.Act5.Jobs = {	
	Heal:Area.Act5.NPCs.Malah,
	Gamble:Area.Act5.NPCs.Anya,
	Repair:Area.Act5.NPCs.Larzuk,
	Pots:Area.Act5.NPCs.Malah,
	Aarows:Area.Act5.NPCs.Larzuk,
	Merc:Area.Act5.NPCs.QualKehk,
	Scrolls:Area.Act5.NPCs.Malah,
	Cain:Area.Act5.NPCs.DeckardCain
}

Area.Act5.Interact = {
	Heal:NPCMenu.Null,
	Gamble:NPCMenu.Gamble,
	Repair:NPCMenu.TradeRepair,
	Pots:NPCMenu.Trade,
	Aarows:NPCMenu.TradeRepair,
	Merc:NPCMenu.RessurectMerc,
	Scrolls:NPCMenu.Trade,
	Cain:NPCMenu.IdentifyItems
}

var NPCJob = {
	Heal:"Heal",
	Repair:"Repair",
	Pots:"Pots",
	Aarows:"Aarows",
	Merc:"Merc",
	Scrolls:"Scrolls",
	Cain:"Cain"
}

Area.Act5.Objects = {
	Waypoint:429,
	PindlePortal:60
}

Area.Town = [Area.Act1.RogueEncampment, Area.Act2.LutGholein, Area.Act3.KurastDocktown, Area.Act4.ThePandemoniumFortress, Area.Act5.Harrogath];

var Class = {
	Amazon: 0,
	Sorc: 1,
	Necromancer: 2,
	Paladin: 3,
	Barbarian: 4,
	Druid: 5,
	Assassin: 6
}

var Type = {
	Player: 0,
	Monster: 1,
	Npc: 1,
	Object: 2,
	Missile: 3,
	Item: 4,
	Tile: 5
};

var UIFlags = {
	Inventory: 0x01,
	Character: 0x02,
	QuickSkill: 0x03,
	Skill: 0x04,
	ChatBox: 0x05,
	NPCMenu: 0x08,
	EscMenu: 0x09,
	Automap: 0x0A,
	ConfigControls: 0x0B,
	NPCShop: 0x0C,
	ShowItems: 0x0D,
	Cash: 0x0E,
	Quest: 0x0F,
	QuestLogButton: 0x11,
	StatusArea: 0x12,
	Waypoint: 0x14,
	MiniPanel: 0x15,
	Party: 0x16,
	TradePrompt: 0x17,
	MessageLog: 0x18,
	Stash: 0x19,
	Cube: 0x1A,
	ShowBelt: 0x1F,
	Help: 0x21,
	MercScreen: 0x24,
	ScrollOfInfuiss: 0x25
};

var Waypoint = {
RogueEncampment:0, ColdPlains:1, StonyField:2, DarkWood:3, BlackMarsh:4, OuterCloister:5, JailLevel1:6, InnerCloister:7, CatacombsLevel2:8,
LutGholein:9, SewersLevel2:10, DryHills:11, HallsOfDeadLevel2:12, FarOasis:13, LostCity:14, PalaceCellarLevel1:15, ArcaneSanctuary:16, CanyonOfTheMagi:17,
KurastDocks:18, SpiderForest:19, GreatMarsh:20, FlayerJungle:21, LowerKurast:22, KurastBazaar:23, UpperKurast:24, Travincal:25, DuranceOfHateLevel2:26,
ThePandemoniumFortress:27, CityOfTheDamned:28, RiverOfFlame:29,
Harrogath:30, FrigidHighlands:31, ArreatPlateau:32, CrystallinePassage:33, GlacialTrail:34, HallsOfPain:35, FrozenTundra:36, TheAncientsWay:37, WorldStoneKeepLevel2:38};

var State = { None:0, Freeze:1, Poison:2, ResistFire:3, ResistCold:4, ResistLight:5, ResistMagic:6, PlayerBody:7, ResistAll:8, AmplifyDamage:9, FrozenArmor:10,
	Cold:11, Inferno:12, Blaze:13, BoneArmor:14, Concentrate:15, Enchant:16, InnerSight:17, SkillMove:18, Weaken:19, ChillingArmor:20, Stunned:21, Spiderlay:22,
	DimVision:23, Slowed:24, FetishAura:25, Shout:26, Taunt:27, Conviction:28, Convicted:29, EnergyShield:30, VenomClaws:31, BattleOrders:32, Might:33,
	Prayer:34, HolyFire:35, Thorns:36, Defiance:37, ThunderStorm:38, LightningBolt:39, BlessedAim:40, Stamina:41, Concentration:42, HolyFreeze:43,
	HolyFreezed:44, Cleansing:45, HolyShock:46, Sanctuary:47, Meditation:48, Fanaticism:49, Redemption:50, BattleCommand:51, PreventHeal:52,
	Conversion:53, Uninterruptable:54, IronMaiden:55, Terror:56, Attract:57, LifeTap:58, Confuse:59, Decrepify:60, LowerResists:61, OpenWounds:62, DoppleZon:63,
	CritcalStrike:64, Dodge:65, Avoid:66, Penetrate:67, Evade:68, Pierce:69, Warmth:70, FireMastery:71, LightningMastery:72, ColdMastery:73, SwordMastery:74,
	AxeMastery:75, MaceMastery:76, PolearmMastery:77, ThrowingMastery:78, SpearMastery:79, IncreasedStamina:80, IronSkin:81, IncreasedSpeed:82, NaturalResistance:83,
	FingerMageCurse:84, NoManaRegen:85, JustHit:86, SlowMissiles:87, ShiverArmor:88, BattleCry:89, Blue:90, Red:91, DeathDelay:92, Valkyrie:93, Frenzy:94,
	Berserk:95, Revive:96, ItemFullSet:97, SourceUnit:98, Redeemed:99, HealthPot:100, HolyShield:101, JustPortalized:102, MonsterFrenzy:103, CorspeNoDraw:104,
	Alignment:105, ManaPot:106, Shatter:107, SyncWarped:108, ConversionSave:109, Pregnant:110, Unknown:111, Rabies:112, DefenseCurse:113, BloodMana:114,
	Burning:115, DragonFlight:116, Maul:117, CorpseNoSelect:118, ShadowWarrior:119, FeralRage:120, SkillDelay:121, ProgressiveDamage:122, ProgressiveSteal:123,
	ProgressiveOther:124, ProgressiveFire:125, ProgressiveCold:126, ProgressiveLightning:127, ShrineArmor:128, ShrineCombat:129, ShrineResistLightning:130,
	ShrineResistFire:131, ShrineResistCold:132, ShrineResistPoison:133, ShrineSkill:134, ShrineManaRegen:135, ShrineStamina:136, ShrineExperience:137,
	FenrisRage:138, Wolf:139, Bear:140, BloodLust:141, ChangeClass:142, Attached:143, Hurricane:144, Armageddon:145, Invis:146, Barbs:147, Wolverine:148, OakSage:149, 
	VineBeast:150, CycloneArmor:151, ClawMastery:152, CloakOfShadows:153, Recycled:154, WeaponBlock:155, Cloaked:156, Quickness:157, BladeShield:158, Fade:159 };

State.name = function(id) { for (var n in this) { if (id == this[n]) { return n; }} return false; };

var Hand = {
	Right: 0,
	Left: 1
}

var ClickType = {};

ClickType.Left = {
	Down:0,
	Hold:1,
	Up:2
}

ClickType.Right = {
	Down:3,
	Hold:4,
	Up:5
}

var Shift = {
	On:1,
	Off:0
}


var Stats = {
	Strength:0,Energy:1,Dexterity:2,Vitality:3,StatPoints:4,SkillPoints:5,Life:6,MaxLife:7,Mana:8,MaxMana:9,
	Stamina:10,MaxStamina:11,Level:12,Experience:13,Gold:14,StashGold:15,ArmorPercent:16,MaxDamagePerent:17,
	MinDamagePercent:18,ToHit:19,ToBlock:20,MinDamage:21,MaxDamage:22,SecondMinDamage:23,SecMaxDamage:24,
	DamagePercent:25, ManaRecovery:26,ManaRecoveryBonus:27,StaminaRecoveryBonus:28,LastExp:29,NextExp:30,
	ArmorClass:31,ArmorClassVsMissile:32,ArmorClassVsHth:33,NormalDamageReduction:34,MagicDamageReduction:35,
	DamageResist:36,MagicResist:37,MaxMagicResist:38,FireResist:39,MaxFireResist:40,LightningResist:41,
	MaxLightningResist:42,ColdResist:43, MaxColdResist:44,PoisonResist:45,MaxPoisonResist:46,DamageAura:47,
	FireMinDamage:48,FireMaxDamage:49,LightningMinDamage:50,LightningMaxDamage:51,MagicMinDamage:52,
	MagicMaxDamage:53,ColdMinDamage:54,ColdMaxDamage:55,ColdLength:56,PoisonMinDamage:57,PoisonMaxDamage:58,
	PoisonLength:59,LifeDrainMinDamage:60,LifeDrainMaxDamage:61,ManaDrainMinDamage:62,ManaDrainMaxDamage:63,
	StaminaDrainMinDamage:64,StaminaDrainMaxDamage:65,StunLength:66,VelocityPercent:67,AttackRate:68,OtherAnimRate:69,
	Quantity:70, Value:71,Durability:72,MaxDurability:73,HPRegen:74,MaxDurabilityPercent:75,MaxHPPercent:76,
	MaxManaPercent:77,AttackerTakesDamage:78,GoldBonus:79,MagicBonus:80,Knockback:81,TimeDuration:82,AddClassSkills:83,
	AddExperience:85,HealAfterKill:86,ReducePrices:87,DoubleHerbDuration:88,LightRadius:89,LightColor:90,
	RequirementPercent:91,LevelRequire:92,FasterAttackRate:93,LevelRequirePercent:94,LastBlockFrame:95,
	FasterRunWalk:96,NonClassSkill:97,State:98,FasterHitRecovery:99,PlayerCount:100,PoisonOverrideLength:101,
	FasterBlockRate:102,BypassUndead:103,BypassDemons:104,FasterCastRate:105,BypassBeasts:106,SingleSkill:107,
	RestInPeace:108,CurseResistance:109,PoisonLengthResist:110,NormalDamage:111,Howl:112,Stupidity:113,
	DamageToMana:114,IgnoreTargetsAR:115,FractionalTargetAC:116,PreventHeal:117,HalfFreezeDuration:118,
	ToHitPercent:119,DamageTargetAC:120,DemonDamagePercent:121,UndeadDamagePercent:122,DemonToHit:123,
	UndeadToHit:124,Throwable:125,ElemSkills:126, AllSkills:127, AttackerTakesLightDamage:128,
	IronMaidenLevel:129, LifeTapLevel:130, ThornsPercent:131,BoneArmor:132,BoneArmorMax:133,
	Freeze:134,OpenWounds:135,CrushingBlow:136,KickDamage:137,ManaAfterKill:138,
	HealAfterDemonKill:139,ExtraBlood:140,DeadlyStrike:141,AbsorbFirePercent:142,AbsorbFire:143,
	AbsorbLightPercent:144,AbsorbLight:145,AbsorbMagicPercent:146,AbsorbMagic:147,AbsorbColdPercent:148,
	AbsorbCold:149,Slow:150,Aura:151,Indestructible:152,CannotBeFrozen:153,StaminaDrainPercent:154,
	Reanimate:155,Pierce:156,MagicAarow:157,ExplosiveAarow:158,ThrowMinDamage:159,ThrowMaxDamage:160,
	HandofAthena:161,StaminaPercent:162,PassiveStaminaPercent:163,Concentration:164,Enchant:165,Pierce:166,
	Conviction:167,ChillingArmor:168,Frenzy:169,Decrepify:170,ArmorPercent:171,Alignment:172,Target0:173,
	Target1:174,GoldLost:175,ConverisonLevel:176,ConverisonMaxHP:177,UnitDooverlay:178,AttackVsMonType:179,
	DamageVsMonType:180,Fade:181,ArmorOverridePercent:182,AddSkillTab:188,NumSockets:194,SkillOnAttack:195,
	SkillOnDeath:197,SkillOnHit:198,SkillOnLevelUp:199,SkillOnGetHit:201,ItemChargedSkill:204,
	ArmorPerLevel:214,ArmorPercentPerLevel:215,LifePerLevel:216,ManaPerLevel:217,MaxDamagePerLevel:218,
	MaxDamagePercentPerLevel:219,StrengthPerLevel:220,DexterityPerLevel:221,EnergyPerLevel:222,
	VitalityPerLevel:223,ToHitPerLevel:224,ToHitPercentPerLevel:225,ColdDamageMaxPerLevel:226,
	FireDamageMaxPerLevel:227,LightningDamageMaxPerLevel:228,PoisonDamageMaxPerLevel:229,
	ResistColdPerLevel:230,ResistFirePerLevel:231,ResistLightningPerLevel:232,
	ResistPoisonPerLevel:233,AbsorbColdPerLevel:234,AbsorbFirePerLevel:235,AbsorbLightningPerLevel:236,
	AbsorbPoisonPerLevel:237,ThornsPerLevel:238,FindGoldPerLevel:239,MagicFindPerLevel:240,
	RegenStaminaPerLevel:241,StaminaPerLevel:242,DamageDemonPerLevel:243,DamageUndeadPerLevel:244,
	ToHitDemonPerLevel:245,ToHitUndeadPerLevel:246,CrushingBlowPerLevel:247,OpenWoundsPerLevel:248,
	KickDamagePerLevel:249,DeadlyStrikePerLevel:250,FindGemsPerLevel:251,ReplenishDurability:252,
	ReplenishQuantity:253,ExtraStack:254,FindItem:255,SlashDamage:256,SlashDamagePercent:257,
	CrushDamage:258,CrushDamagePercent:259,ThrustDamage:260,ThrustDamagePercent:261,
	AbsorbSlash:262,AbsorbCrush:263,AbsorbThrust:264,AbsorbSlashPercent:265,AbsorbCrushPercent:266,
	AbsorbThrustPercent:267,ArmorByTime:268,ArmorPercentByTime:269,LifeByTime:270,ManaByTime:271,
	MaxDamageByTime:272,MaxDamagePercentByTime:273,StrengthByTime:274,DexterityByTime:275,
	EnergyByTime:276,VitalityByTime:277,ToHitByTime:278,ToHitPercentByTime:279,ColdDamageMaxByTime:280,
	FireDamageMaxByTime:281,LightningDamageMaxByTime:282,PoisonDamageMaxByTime:283,ResistColdByTime:284,
	ResistFireByTime:285,ResistLightningByTime:286,ResistPoisonByTime:287,AbsorbColdByTime:288,
	AbsorbFireByTime:289,AbsorbLightningByTime:290,AbsorbPoisonByTime:291,FindGoldByTime:292,
	MagicFindByTime:293,RegenStaminaByTime:294,StaminaByTime:295,DamageDemonByTime:296,DamageUndeadByTime:297,
	ToHitDemonByTime:298, ToHitUndeadByTime:299,CrushingBlowByTime:300,OpenWoundsByTime:301, KickDamageByTime:302,
	DeadlyStrikeByTime:303,FindGemsByTime:304,PierceCold:305,PierceFire:306,PierceLightning:307,
	PiercePoison:308, DamageVsMonster:309,DamagePercentVsMonster:310,ToHitVsMonster:311,
	ToHitPercentVsMonster:312,AcVsMonster:313,AcPercentVsMonster:314,FireLength:315,
	BurningMin:316,BurningMax:317,ProgressiveDamage:318, ProgressiveSteal:319,
	ProgressiveOther:320, ProgressiveFire:321, ProgressiveCold:322, ProgressiveLightning:323,
	ExtraCharges:324, ProgressiveToHit:325,PoisonCount:326,DamageFrameRate:327,PierceIdx:328,
	PassiveFireMastery:329,PassiveLightningMastery:330,PassiveColdMastery:331,
	PassivePoisonMastery:332,PassiveFirePierce:333,PassiveLightningPierce:334,
	PassiveColdPierce:335, PassivePoisonPierce:336,PassiveCriticalStrike:337,
	PassiveDodge:338,PassiveAvoid:339,PassiveEvade:340,PassiveWarmth:341,
	PassiveMasteryMeleeToHit:342,PassiveMasteryMeleeDamage:343,
	PassiveMasteryMeleeCritical:344,PassiveMasteryThrowToHit:345,
	PassiveMasteryThrowDamage:346, PassiveMasteryThrowCritical:347,
	PassiveWeaponBlock:348,SummonResist:349,ModifierListSkill:350,
	ModifierListLevel:351,LastSentHPPercent:352,SourceUnitType:353,
	SourceUnitID:354, ShortParam1:355, QuestItemDifficulty:356,
	PassiveMagicMastery:357,PassiveMagicPierce:358
}

Stats.AddSkillTab = {
	Id:188, 
	Bow:0, Passive:1, Javelin:2, 
	Fire:8, Lightning:9, Cold:10,
	Curses:16, PoisonBone:17, NecroSummon:18,
	PalCombat:24, OffensiveAuras:25, DefensiveAuras:26,
	BarCombat:32, Masteries:33, Warcries:34,
	DruSummon:40, ShapeShifting:41, Elemental:42,
	Traps:48, ShadowDiscipline:49, MartialArts:50
}

Stats.AddClassSkills = {
	Id: 83,
	Amazon: 0,
	Sorceress: 1,
	Necromancer: 2,
	Paladin: 3,
	Barbarian: 4,
	Druid: 5,
	Assassin: 6
}

Stats.ElemSkills = {
	Id:126,
	FireSkills:1
}

Stats.find = function (stat)
{
	for (var i in this)
		if (Stats[i] == stat)
			return Stats[i];
	return false;
}
var Range = {
	Melee:1,
	Ranged:2
}

var SkillType = {
	Physical:0,
	Fire:1,
	Cold:2,
	Lightning:3,
	Poison:4,
	Magic:5
}
var SkillImmunities = [Stats.DamageResist, Stats.FireResist, Stats.ColdResist, Stats.LightningResist,
Stats.PoisonResist, Stats.MagicResist]

var BodyLocation = {
	NotEquipped:0,
	Helm:1,
	Amulet:2,
	Armor:3,
	RightSlotI:4,
	LeftSlotI:5,
	RightRing:6,
	LeftRing:7,
	Belt:8,
	Boots:9,
	Gloves:10,
	RightSlotII:11,
	LeftSlotII:12
}

var pickitStats = {strength:0, energy:1, dexterity:2, vitality:3, life:7, maxlife:7, mana:9, maxmana:9, stamina:11, 
maxstamina:11, gold:14, enhanceddefense:16, maxdamagepercent:17, enhanceddamage:18, tohit:19, toblock:20, mindamage:21, 
maxdamage:22, secondarymindamage:23, secondarymaxdamage:24, damagepercent:25, manarecovery:26, manarecoverybonus:27,
staminarecoverybonus:28, defense:31, defensevsmissile:32, defensevshth:33, normaldamagereduction:34,
magicdamagereduction:35, damagereduce:36, magicresist:37, maxmagicresist:38, fireresist:39, maxfireresist:40,
lightresist:41, maxlightresist:42, coldresist:43, maxcoldresist:44, poisonresist:45, maxpoisonresist:46,
damageaura:47, firemindamage:48, firemaxdamage:49, lightmindamage:50, lightmaxdamage:51, magicmindamage:52, 
magicmaxdamage:53, coldmindamage:54, coldmaxdamage:55, coldlength:56, poisonmindamage:57, poisonmaxdamage:58,
poisonlength:59, lifeleech:60, maxlifeleech:61, manaleech:61, maxmanaleech:62, staminaleech:63, maxstaminaleech:64,
stunlength:65, velcocitypercent:66,attackrating:68,otheranimrate:69,
quantity:70, value:71,durability:72,maxdurability:73,replenishlife:74,maxdurabilitypercent:75,maxhppercent:76,
maxmanapercent:77,attackertakesdamage:78,goldbonus:79,magicbonus:80,knockback:81,timeduration:82,
addexperience:85,healafterkill:86,reduceprices:87,doubleherbduration:88,lightradius:89,lightcolor:90,
requirementpercent:91,levelrequire:92,ias:93,levelrequirepercent:94,lastblockframe:95,
frw:96,state:98,fhr:99,playercount:100,poisonoverridelength:101,
fbr:102,bypassundead:103,bypassdemons:104,fcr:105,bypassbeasts:106,singleskill:107,
restinpeace:108,curseresistance:109,poisonlengthresist:110,normaldamage:111,howl:112,stupidity:113,
damagetomana:114,ignoretargetsar:115,fractionaltargetac:116,preventheal:117,halffreezeduration:118,
tohitpercent:119,damagetargetac:120,demondamagepercent:121,undeaddamagepercent:122,demontohit:123,
undeadtohit:124,throwable:125,allskills:127, attackertakeslightdamage:128,
ironmaidenlevel:129, lifetaplevel:130, thornspercent:131,bonearmor:132,bonearmormax:133,
freeze:134,openwounds:135,crushingblow:136,kickdamage:137,manaafterkill:138,
healafterdemonkill:139,extrablood:140,deadlystrike:141,absorbfirepercent:142,absorbfire:143,
absorblightpercent:144,absorblight:145,absorbmagicpercent:146,absorbmagic:147,absorbcoldpercent:148,
absorbcold:149,slowtarget:150,aura:151,indestructible:152,cannotbefrozen:153,staminadrainpercent:154,
reanimate:155,pierce:156,magicaarow:157,explosiveaarow:158,throwmindamage:159,throwmaxdamage:160,
handofathena:161,staminapercent:162,passivestaminapercent:163,concentration:164,enchant:165,pierce:166,
conviction:167,chillingarmor:168,frenzy:169,decrepify:170,armorpercent:171,alignment:172,target0:173,
target1:174,goldlost:175,converisonlevel:176,converisonmaxhp:177,unitdooverlay:178,attackvsmontype:179,
damagevsmontype:180,fade:181,armoroverridepercent:182,sockets:194,
armorperlevel:214,armorpercentperlevel:215,lifeperlevel:216,manaperlevel:217,maxdamageperlevel:218,
maxdamagepercentperlevel:219,strengthperlevel:220,dexterityperlevel:221,energyperlevel:222,
vitalityperlevel:223,tohitperlevel:224,tohitpercentperlevel:225,colddamagemaxperlevel:226,
firedamagemaxperlevel:227,lightningdamagemaxperlevel:228,poisondamagemaxperlevel:229,
resistcoldperlevel:230,resistfireperlevel:231,resistlightningperlevel:232,
resistpoisonperlevel:233,absorbcoldperlevel:234,absorbfireperlevel:235,absorblightningperlevel:236,
absorbpoisonperlevel:237,thornsperlevel:238,findgoldperlevel:239,magicfindperlevel:240,
regenstaminaperlevel:241,staminaperlevel:242,damagedemonperlevel:243,damageundeadperlevel:244,
tohitdemonperlevel:245,tohitundeadperlevel:246,crushingblowperlevel:247,openwoundsperlevel:248,
kickdamageperlevel:249,deadlystrikeperlevel:250,findgemsperlevel:251,replenishdurability:252,
replenishquantity:253,extrastack:254,finditem:255,slashdamage:256,slashdamagepercent:257,
crushdamage:258,crushdamagepercent:259,thrustdamage:260,thrustdamagepercent:261,
absorbslash:262,absorbcrush:263,absorbthrust:264,absorbslashpercent:265,absorbcrushpercent:266,
absorbthrustpercent:267,armorbytime:268,armorpercentbytime:269,lifebytime:270,manabytime:271,
maxdamagebytime:272,maxdamagepercentbytime:273,strengthbytime:274,dexteritybytime:275,
energybytime:276,vitalitybytime:277,tohitbytime:278,tohitpercentbytime:279,colddamagemaxbytime:280,
firedamagemaxbytime:281,lightningdamagemaxbytime:282,poisondamagemaxbytime:283,resistcoldbytime:284,
resistfirebytime:285,resistlightningbytime:286,resistpoisonbytime:287,absorbcoldbytime:288,
absorbfirebytime:289,absorblightningbytime:290,absorbpoisonbytime:291,findgoldbytime:292,
magicfindbytime:293,regenstaminabytime:294,staminabytime:295,damagedemonbytime:296,damageundeadbytime:297,
tohitdemonbytime:298, tohitundeadbytime:299,crushingblowbytime:300,openwoundsbytime:301, kickdamagebytime:302,
deadlystrikebytime:303,findgemsbytime:304,piercecold:305,piercefire:306,piercelightning:307,
piercepoison:308, damagevsmonster:309,damagepercentvsmonster:310,tohitvsmonster:311,
tohitpercentvsmonster:312,acvsmonster:313,acpercentvsmonster:314,firelength:315,
burningmin:316,burningmax:317,progressivedamage:318, progressivesteal:319,
progressiveother:320, progressivefire:321, progressivecold:322, progressivelightning:323,
extracharges:324, progressivetohit:325,poisoncount:326,damageframerate:327,pierceidx:328,
passivefiremastery:329,passivelightningmastery:330,passivecoldmastery:331,
passivepoisonmastery:332,passivefirepierce:333,passivelightningpierce:334,
passivecoldpierce:335, passivepoisonpierce:336,passivecriticalstrike:337,
passivedodge:338,passiveavoid:339,passiveevade:340,passivewarmth:341,
passivemasterymeleetohit:342,passivemasterymeleedamage:343,
passivemasterymeleecritical:344,passivemasterythrowtohit:345,
passivemasterythrowdamage:346, passivemasterythrowcritical:347,
passiveweaponblock:348,summonresist:349,modifierlistskill:350,
passivemagicmastery:357,passivemagicpierce:358
}

var skillTypes = {"onattack":195, "ondeath":197, "onhit":198, "onlevelup":199, "onhit":201, "charge":204, "":97}
for (var type in skillTypes)
	for (var ski in Skill)
		pickitStats[ski.toLowerCase () + type] = [skillTypes[type], Skill[ski]];
		
for (var type in Stats.AddSkillTab)
{
	if (type == "Id")
		continue;
	pickitStats[type.toLowerCase ()] = [Stats.AddSkillTab.Id, Stats.AddSkillTab[type]];
}

for (var type in Stats.AddClassSkills)
{
	if (type == "Id")
		continue;
	pickitStats[type.toLowerCase () + "classskill"] = [Stats.AddClassSkills.Id, Stats.AddClassSkills[type]];
}

pickitStats["fireskills"] = [126, 1];

var pickitFlag = {
	equipped: 0x1,
	insocket: 0x8,
	identified: 0x10,
	switchin: 0x40,
	switchout: 0x80,
	broken: 0x100,
	rejuvination: 0x400,
	socketed: 0x800,
	instore: 0x2000,
	notinsocket: 0x4000,
	ear: 0x10000,
	starteritem: 0x20000,
	runeorpotion: 0x200000,
	ethereal: 0x400000
}

var pickitQuality = {
	inferior: 1,
	normal: 2,
	superior: 3,
	magic: 4,
	set: 5,
	rare: 6,
	unique: 7,
	crafted: 8
}

var itemColor = {
	inferior: 5,
	normal: 0,
	superior: 3,
	magic: 3,
	set: 2,
	rare: 9,
	unique: 4,
	crafted: 8
}

var itemLocation = {
	Inventory:0,
	Belt:1,
	Cube:2,
	Stash:3,
	Trade:4,
	Equip:5
}

var categories = {
	'axes': ['hax','9ha','7ha','axe','9ax','7ax','2ax','92a','72a','mpi','9mp',
			 '7mp','wax','9wa','7wa','lax','9la','7la','bax','9ba','7ba','btx',
			 '9bt','7bt','gax','9ga','7ga','gix','9gi','7gi'],
	'daggers': ['dgr','9dg','7dg','dir','9di','7di','kri','9kr','7kr','bld',
				'9bl','7bl'],
	'maces': ['clb','9cl','7cl','spc','9sp','7sp','mac','9ma','7ma','mst','9mt',
			  '7mf','fla','9fl','7fl','fla','9fl','7fl','whm','9wh','7wh','mau',
			  '9m9','7m7','gma','9gm','7gm'],
	'polearms': ['bar','9b7','7o7','vou','9vo','7vo','scy','9s8','7s8','pax',
				 '9pa','7pa','hal','9h9','7h7','wsc','9wc','7wc'],
	'scepters': ['scp','9sc','7sc','gsc','9qs','7qs','wsp','9ws','7ws'],
	'spears': ['spr','9sr','7sr','tri','9tr','7tr','brn','9br','7br','spt',
			   '9st','7st','pik','9p9','7p7'],
	'staves': ['sst','8ss','6ss','lst','8ls','6ls','gst','8cs','6cs','bst',
			   '8bs','6bs','wst','8ws','6ws'],
	'swords': ['ssd','9ss','7ss','scm','9sm','7sm','sbr','9sb','7sb','flc',
			   '9fc','7fc','crs','9cr','7cr','bsd','9bs','7bs','lsd','9ls',
			   '7ls','wsd','9wd','7wd','2hs','92h','72h','clm','9cm','7cm',
			   'gis','9gs','7gs','bsw','9b9','7b7','flb','9fb','7fb','gsd',
			   '9gd','7gd'],
	'wands': ['wnd','9wn','7wn','ywn','9yw','7yw','bwn','9bw','7bw','gwn','9gw','7gw'],
	'assassin katars': ['ktr','9ar','7ar','wrb','9wb','7wb','axf','9xf','7xf',
						'ces','9cs','7cs','clw','9lw','7lw','btl','9hw','7hw',
						'skr','9qr','7qr'],
	'sorceress orbs': ['ob1','ob6','obb','ob2','ob7','obc','ob3','ob8','obd',
					   'ob4','ob9','obe','ob5','oba','obf'],
	'amazon spears': ['am3','am8','amd','am4','am9','ame'],

	'bows': ['sbw','8sb','6sb','hbw','8hb','6hb','lbw','8lb','6lb','cbw','8cb',
			 '6cb','sbb','8s8','6s7','lbb','8l8','6l7','swb','8sw','6sw','lwb',
			 '8lw','6lw'],
	'crossbows': ['lxb','8lx','6lx','mxb','8mx','6mx','hxb','8hx','6hx','rxb',
				  '8rx','6rx'],
	'javelins': ['jav','9ja','7ja','pil','9pi','7pi','ssp','9s9','7s7','glv',
				 '9gl','7gl','tsp','9ts','7ts'],
	'throwing': ['tkf','9tk','7tk','tax','9ta','7ta','bkf','9bk','7bk','bal',
				 '9b8','7b8'],
	'amazon bows': ['am1','am6','amb','am2','am7','amc'],
	'amazon javelins': ['am5','ama','amf'],
	'throwing potions': ['gps','ops','gpm','opm','gpl','opl'],

	'body armor': ['qui','xui','uui','lea','xea','uea','hla','xla','ula','stu',
				  'xtu','utu','rng','xng','ung','scl','xcl','ucl','chn','xhn',
				  'uhn','brs','xrs','urs','spl','xpl','upl','plt','xlt','ult',
				  'fld','xld','uld','gth','xth','uth','ful','xul','uul','aar',
				  'xar','uar','ltp','xtp','utp'],
	'helmets': ['cap','xap','uap','skp','xkp','ukp','hlm','xlm','ulm','fhl',
				'xhl','uhl','ghm','xhm','uhm','crn','xrn','urn','msk','xsk',
				'usk','bhm','xh9','uh9'],
	'gloves': ['lgl','xlg','ulg','vgl','xvg','uvg','mgl','xmg','umg','tgl',
			   'xtg','utg','hgl','xhg','uhg'],
	'shields': ['buc','xuc','uuc','sml','xml','uml','lrg','xrg','urg','kit',
				'xit','uit','tow','xow','uow','gts','xts','uts','bsh','xsh',
				'ush','spk','xpk','upk'],
	'belts': ['lbl','zlb','ulc','vbl','zvb','uvc','mbl','zmb','umc','tbl','ztb',
			  'utc','hbl','zhb','uhc'],
	'boots': ['lbt','xlb','ulb','vbt','xvb','uvb','mbt','xmb','umb','tbt','xtb',
			  'utb','hbt','xhb','uhb'],
	'paladin shields': ['pa1','pa6','pab','pa2','pa7','pac','pa3','pa8','pad',
						'pa4','pa9','pae','pa5','paa','paf'],
	'druid pelts': ['dr1','dr6','drb','dr2','dr7','drc','dr3','dr8','drd','dr4',
					'dr9','dre','dr5','dra','drf'],
	'barbarian helmets': ['ba1','ba6','bab','ba2','ba7','bac','ba3','ba8','bad',
						  'ba4','ba9','bae','ba5','baa','baf'],
	'necromancer heads': ['ne1','ne6','neb','ne2','ne7','nec','ne3','ne8','ned',
						  'ne4','ne9','nee','ne5','nea','nef'],
	'circlets': ['ci0','ci1','ci2','ci3'],

	'jewelry': ['amu', 'rin'],

	'misc': ['std','jew','key','gld'],

	'keys and organs': ['pk1','pk2','pk3','dhn','bey','mbr'],

	'charms': ['cm1','cm2','cm3'],

	'quest items': ['leg','hdm','bks','bkd','ass','box','tr1','msf','vip','hst',
					'xyz','j34','g34','bbb','g33','qf1','qf2','qey','qhr','qbr',
					'mss','hfh','ice','tr2'],

	'runes': ['r01','r02','r03','r04','r05','r06','r07','r08','r09','r10','r11',
			  'r12','r13','r14','r15','r16','r17','r18','r19','r20','r21','r22',
			  'r23','r24','r25','r26','r27','r28','r29','r30','r31','r32','r33'],
	
	'perfect gems': ['gpw','gpv','gpb','gpy','gpr','skz','gpg'],
	'flawless gems': ['gzv','glw','glg','glr','glb','skl','gly'],
	'normal gems': ['gsv','gsw','gsg','gsr','gsb','sku','gsy'],
	'flawed gems': ['gfv','gfw','gfg','gfr','gfb','skf','gfy'],
	'chipped gems': ['gcv','gcw','gcg','gcr','gcb','skc','gcy'],

	'healing potions': ['hp1','hp2','hp3','hp4','hp5'],
	'mana potions': ['mp1','mp2','mp3','mp4','mp5'],
	'rejuvenation potions': ['rvs','rvl'],
	'misc potions': ['yps','vps','wms'],

	'scrolls': ['isc','tsc'],
	'tomes': ['tbk','ibk'],

	'ammunition': ['aqv','cqv'],

	'junk': ['ear','tch','hrt','brz','jaw','eyz','hrn','hrb','tal','flg','fng',
			 'qll','sol','scz','spe','luv','elx','0sc','rps','rpl','bps','bpl',
			 'hpo','mpo','hpf','mpf']
};

categories['melee'] = categories.axes.concat(categories.daggers, categories.maces,
				categories.polearms, categories.scepters, categories.spears,
				categories.staves, categories.swords, categories.wands,
				categories['assassin katars'], categories['sorceress orbs'],
				categories['amazon spears']);

categories['ranged'] = categories.bows.concat(categories.crossbows, categories.javelins,
				categories.throwing, categories['amazon bows'],
				categories['amazon javelins'], categories['throwing potions']);

categories['weapons'] = categories.melee.concat(categories.ranged);

categories['armors'] = categories['body armor'].concat(categories.helmets, categories.gloves,
						categories.shields, categories.belts, categories.boots,
						categories['paladin shields'], categories['druid pelts'],
						categories['barbarian helmets'], categories['necromancer heads'],
						categories.circlets);

categories['equipment'] = categories.weapons.concat(categories.armors);

categories['gems'] = categories['perfect gems'].concat(categories['flawless gems'],
				categories['normal gems'], categories['flawed gems'],
				categories['chipped gems']);

categories['socketables'] = categories.runes.concat(categories.gems, ['jew']);

categories['potions'] = categories['healing potions'].concat(categories['mana potions'],
				categories['rejuvenation potions'], categories['misc potions']);
				
var itemCodes = {"cap":"cap", "skull cap":"skp", "helm":"hlm", "full helm":"fhl", "great helm":"ghm", "crown":"crn",
"mask":"msk", "bone helm":"bhm", "war hat":"xap", "sallet":"xkp", "casque":"xhm", "basinet":"xhl", "winged helm":"xhm",
"grand crown":"xrn", "death mask":"xsk", "grim helm":"xh9", "shako":"uap", "hydraskull":"ukp", "armet":"ulm",
"giant conch":"uhl", "spired helm":"uhm", "corona":"urn", "demonhead":"usk", "bone visage":"uh9",

"quilted armor":"qui", "leather armor":"lea", "hard armor":"hla", "studded leather":"stu", "ring mail":"rng",
"scale mail":"scl", "chain mail":"chn", "breast plate":"brs", "splint mail":"spl", "plate mail":"plt", 
"field plate":"fld", "gothic plate":"gth", "full plate mail":"ful", "ancient armor":"aar", "ghost armor":"xui",
"serpentskin armor":"xea", "demonhide armor":"xla", "trellised armor":"xtu", "mesh armor":"xhn", "cuirass":"xrs",
"russet armor":"xpl", "templar coat":"xlt", "sharktooth armor":"xld", "embossed plate":"xth", "chaos armor":"xul",
"ornate armor":"xar", "Mage Plate":"xtp", "dusk shroud":"uui", "wyrmhide":"uea", "scarab husk":"ula", 
"wire fleece":"utu", "diamond mail":"ung", "loricated mail":"ucl", "boneweave":"uhn", "great hauberk":"urs",
"balrog skin":"upl", "hellforge plate":"ult", "kraken shell":"uld", "lacquered plate":"uth", "shadow plate":"uul",
"sacred armor":"uar", "archon plate":"utp",

"buckler":"buc", "defender":"xuc", "heater":"uuc", "small shield":"sml", "round shield":"xml", "luna":"uml", 
"large shield":"lrg", "scutum":"xrg", "hyperion":"urg", "kite shield":"kit", "dragon shield":"xit", 
"monarch":"uit", "tower shield":"tow", "pavise":"xow", "aegis":"uow", "gothic shield":"gts", "ancient shield":"xts", 
"ward":"uts", "bone shield":"bsh", "grim shield":"xsh", "troll nest":"ush", "spiked shield":"spk", "barbed shield":"xpk", 

"leather gloves":"lgl", "demonhide glove":"xlg", "bramble mitts":"ulg", "heavy gloves":"vgl", "sharkskin glove":"xvg", 
"vampirebone gloves":"uvg", "chain gloves":"mgl", "heavy bracers":"xmg", "vambraces":"umg", "light gauntlets":"tgl", 
"battle gauntlet":"xtg", "crusader gaunt":"ulg", "gauntlets":"hgl", "war gauntlets":"xhg", "ogre gauntlets":"uhg", 

"boots":"lbt", "demonhide boots":"xlb", "wyrmhide boots":"ulb", "heavy boots":"vbt", "sharkskin boots":"xvb", 
"scarabshell boots":"uvb", "chain boots":"mbt", "mesh boots":"xmb", "boneweave boots":"umb", 
"light plate":"tbt", "battle boots":"xtb", "mirrored boots":"utb", "greaves":"hbt", "war boots":"xhb", 
"greaves":"hbt", "war boots":"xhb", "myrmidon greaves":"uhb",

"sash":"lbl", "demonhide sash":"zlb", "spiderweb sash":"ulc", "light belt":"vbl", "sharkskin belt":"zvb", 
"vampirefang blt":"uvc", "belt":"mbl", "mesh belt":"zmb", "mithril coil":"umc", "heavy belt":"tbl", 
"battle belt":"ztb", "troll belt":"utc", "plated belt":"hbl", "war belt":"zhb", "colossus girdle":"uhc", 

"wolf head":"dr1", "alpha helm":"dr6", "blood spirit":"drb", "hawk helm":"dr2", "griffon headress":"dr7", 
"sun spirit":"drc", "antlers":"dr3", "hunter's guise":"dr8", "earth spirit":"drd", "falcon mask":"dr4", 
"sacred feathers":"dr9", "sky spirit":"dre", "spirit mask":"dr5", "totemic mask":"dra", "dream spirit":"drf", 

"jawbone cap":"ba1", "jawbone visor":"ba6", "carnage helm":"bab", "fanged helm":"ba2", "lion helm":"ba7", 
"fury visor":"bac", "horned helm":"ba3", "rage mask":"ba8", "destroyer helm":"bad", "assualt helmet":"ba4", 
"savage helmet":"ba9", "conqueror crown":"bae", "avenger guard":"ba5", "slayer guard":"baa", 
"guardian crown":"baf",

"targe":"pa1", "akaran targe":"pa6", "sacred targe":"pab", "rondache":"pa2", 
"akaran rondache":"pa7", "sacred rondache":"pac", "heraldic shield":"pa3", "protector shld":"pa8", 
"ancient shield":"pad", "aerin shield":"pa4", "gilded shield":"pa9", "zakarum shield":"pae", 
"crown shield":"pa5", "royal shield":"paa", "vortex shield":"paf", 

"preserved head":"ne1", "mummified trphy":"ne6", "minion skull":"neb", "zombie head":"ne2", "fetish trophy":"ne7", 
"hellspawn skull":"nec", "unraveller head":"ne3", "sexton trophy":"ne8", "overseer skull":"ned", 
"gargoyle head":"ne4", "cantor trophy":"ne9", "succubae skull":"nee", "demon head":"ne5", 
"heirophant trophy":"nea", "bloodlord skull":"nef", 

"hand axe":"hax", "hatchet":"9ha", "tomahawk":"7ha", 
"axe":"axe", "cleaver":"9ax", "small crescent":"7ax", "double axe":"2ax", "twin axe":"92a", 
"ettin axe":"72a", "military pick":"mpi", "crowbill":"9mp", "war spike":"7mp", "war axe":"wax", 
"naga":"9wa", "berserker axe":"7wa", "large axe":"lax", "military axe":"9la", "feral axe":"7la", 
"broad axe":"bax", "bearded axe":"9ba", "silver edged ax":"7ba", "battle axe":"btx", "tabar":"9bt", 
"decapitator":"7bt", "great axe":"gax", "gothic axe":"9ga", "champion axe":"7ga", "giant axe":"gix", 
"ancient axe":"9gi", "glorious axe":"7gi", "club":"clb", "cudgel":"9cl", "truncheon":"7cl", 
"spiked club":"spc", "barbed club":"9sp", "tyrant club":"7sp", "mace":"mac", "flanged mace":"9ma", 
"reinforced mace":"7ma", "morning star":"mst", "jagged star":"9mt", "devil star":"7mf", "flail":"fla", 
"knout":"9fl", "scourge":"7fl", "war hammer":"whm", "battle hammer":"9wh", "legendary mallt":"7wh", 
"maul":"mau", "war club":"9m9", "ogre maul":"7m7", "great maul":"gma", "martel de fer":"9gm", 
"thunder maul":"7gm", "short sword":"ssd", "gladius":"9ss", "falcata":"7ss", "scimitar":"scm", 
"cutlass":"9sm", "ataghan":"7sm", "sabre":"sbr", "shamshir":"9sb", "elegant blade":"7sb", 
"falchion":"flc", "tulwar":"9fc", "hydra edge":"7fc", "crystal sword":"crs", "dimensional bld":"9cr", 
"phase blade":"7cr", "broad sword":"bsd", "battle sword":"9bs", "conquest sword":"7bs", "long sword":"lsd", 
"rune sword":"9ls", "cryptic sword":"7ls", "war sword":"wsd", "ancient sword":"9wd", "mythical sword":"7wd", 
"two-handed swrd":"2hs", "espadon":"92h", "legend sword":"72h", "claymore":"clm", "dacian falx":"9cm", 
"highland blade":"7cm", "giant sword":"gis", "tusk sword":"9gs", "balrog blade":"7gs", "bastard sword":"bsw", 
"gothic sword":"9b9", "champion sword":"7b7", "flamberge":"flb", "zweihander":"9fb", "colossal sword":"7fb", 
"great sword":"gsd", "executioner swr":"9gd", "colossus blade":"7gd", "dagger":"dgr", "poignard":"9dg", 
"bone knife":"7dg", "dirk":"dir", "rondel":"9di", "mithral point":"7di", "kriss":"kri", "cinquedeas":"9kr", 
"fanged knife":"7kr", "blade":"bld", "stilleto":"9bl", "legend spike":"7bl", "throwing knife":"tkf", 
"battle dart":"9tk", "flying knife":"7tk", "throwing axe":"tax", "francisca":"9ta", "flying axe":"7ta", 
"balanced knife":"bkf", "war dart":"9bk", "winged knife":"7bk", "balanced axe":"bal", "hurlbat":"9b8", 
"winged axe":"7b8", "javelin":"jav", "war javelin":"9ja", "hyperion javeln":"7ja", "pilum":"pil", 
"great pilum":"9pi", "stygian pilum":"7pi", "short spear":"ssp", "simbilan":"9s9", "balrog spear":"7s7", 
"glaive":"glv", "spiculum":"9gl", "ghost glaive":"7gl", "throwing spear":"tsp", "harpoon":"9ts", 
"winged harpoon":"7ts", "spear":"spr", "war spear":"9sr", "hyperion spear":"7sr", "trident":"tri", 
"fuscina":"9tr", "stygian pike":"7tr", "brandistock":"brn", "war fork":"9br", "mancatcher":"7br", 
"spetum":"spt", "yari":"9st", "ghost spear":"7st", "pike":"pik", "lance":"9p9", "war pike":"7p7", 
"bardiche":"bar", "lochaber axe":"9b7", "ogre axe":"7o7", "voulge":"vou", "bill":"9vo", "colossus voulge":"7vo", 
"scythe":"scy", "battle scythe":"9s8", "thresher":"7s8", "poleaxe":"pax", "partizan":"9pa", 
"cryptic axe":"7pa", "halberd":"hal", "bec-de-corbin":"9h9", "great poleaxe":"7h7", "war scythe":"wsc", 
"grim scythe":"9wc", "giant thresher":"7wc", "short bow":"sbw", "edge bow":"8sb", "spider bow":"6sb", 
"hunter's bow":"hbw", "razor bow":"8hb", "blade bow":"6hb", "long bow":"lbw", "cedar bow":"8lb", 
"shadow bow":"6lb", "composite bow":"cbw", "double bow":"8cb", "great bow":"6cb", "shrt battle bow":"sbb", 
"short siege bow":"8s8", "diamond bow":"6s7", "long battle bow":"lbb", "long siege bow":"8l8", 
"crusader bow":"6l7", "short war bow":"swb", "rune bow":"8sw", "ward bow":"6sw", "long war bow":"lwb", 
"gothic bow":"8lw", "hydra bow":"6lw", "light crossbow":"lxb", "arbalest":"8lx", "pellet bow":"6lx", 
"crossbow":"mxb", "siege crossbow":"8mx", "gorgon crossbow":"6mx", "heavy crossbow":"hxb", 
"ballista":"8hx", "colossus x-bow":"6hx", "repeating x-bow":"rxb", "chu-ko-nu":"8rx", "demon crossbow":"6rx", 
"short staff":"sst", "jo staff":"8ss", "walking stick":"6ss", "long staff":"lst", "quarterstaff":"8ls", 
"stalagmite":"6ls", "gnarled staff":"gst", "cedar staff":"8cs", "elder staff":"6cs", "battle staff":"bst", 
"gothic staff":"8bs", "shillelagh":"6bs", "war staff":"wst", "rune staff":"8ws", "archon staff":"6ws", 
"wand":"wnd", "burnt wand":"9wn", "polished wand":"7wn", "yew wand":"ywn", "petrified wand":"9yw", 
"ghost wand":"7yw", "bone wand":"bwn", "tomb wand":"9bw", "lich wand":"7bw", "grim wand":"gwn", 
"grave wand":"9gw", "unearthed wand":"7gw", "sceptre":"scp", "rune sceptre":"9sc", "mighty sceptre":"7sc", 
"grand sceptre":"gsc", "holy water spri":"9qs", "seraph rod":"7qs", "war sceptre":"wsp", "divine sceptre":"9ws", 
"caduceus":"7ws", "katar":"ktr", "quhab":"9ar", "suwayyah":"7ar", "wrist blade":"wrb", "wrist spike":"9wb", 
"wrist sword":"7wb", "hatchet hands":"axf", "fascia":"9xf", "war fist":"7xf", "cestus":"ces", 
"hand scythe":"9cs", "battle cestus":"7cs", "claws":"clw", "greater claws":"9lw", "feral claws":"7lw", 
"blade talons":"btl", "greater talons":"9hw", "runic talons":"7hw", "scissors katar":"skr", 
"scissors quhab":"9qr", "scissors suwayh":"7qr", 
"eagle orb":"ob1", "glowing orb":"ob6", "heavenly stone":"obb", 
"sacred globe":"ob2", "crystalline glb":"ob7", "eldritch orb":"obc", "smoked sphere":"ob3", 
"cloudy sphere":"ob8", "demon heart":"obd", "clasped orb":"ob4", "sparkling ball":"ob9", "vortex orb":"obe", 
"dragon stone":"ob5", "swirling crystal":"oba", "dimensional shard":"obf", "stag bow":"am1", 
"ashwood bow":"am6", "matriarchal bow":"amb", "reflex bow":"am2", "ceremonial bow":"am7", 
"grand matron bow":"amc", "maiden spear":"am3", "ceremonial spr":"am8", "matriarchal spr":"amd", 
"maiden pike":"am4", "ceremonial pike":"am9", "matriarchal pik":"ame", "maiden javelin":"am5", 
"ceremonial jav":"ama", "matriarchal jav":"amf", "circlet":"ci0", "coronet":"ci1", "tiara":"ci2", 
"diadem":"ci3", "rancid gas pot":"gps", "oil potion":"ops", "choking gas pot":"gpm", "exploding pot":"opm", 
"strangling gas":"gpl", "fulminating pot":"opl", "wirt's leg":"leg", "horadric malus":"hdm", 
"scroll of inifuss 1":"bks", "scroll of inifuss 2":"bkd", "book of skill":"ass", "horadric cube":"box", 
"horadric scroll":"tr1", "staff of kings":"msf", "viper amulet":"vip", "horadric staff":"hst", 
"potion of life":"xyz", "a jade figurine":"j34", "the golden bird":"g34", "lam esen's tome":"bbb", 
"gidbinn":"g33", "khalim's flail":"qf1", "khalim's will":"qf2", "khalim's eye":"qey", "khalim's heart":"qhr", 
"khalim's brain":"qbr", "mephisto's soulstone":"mss", "hellforge hammr":"hfh", "malah's potion":"ice", 
"scroll of resistance":"tr2", "chipped amethyst":"gcv", "chipped diamond":"gcw", "chipped emerald":"gcg", 
"chipped ruby":"gcr", "chipped saphire":"gcb", "chipped skull":"skc", "chipped topaz":"gcy", 
"flawed amethyst":"gfv", "flawed diamond":"gfw", "flawed emerald":"gfg", "flawed ruby":"gfr", 
"flawed saphire":"gfb", "flawed skull":"skf", "flawed topaz":"gfy", "amethyst":"gsv", "diamond":"gsw", 
"emerald":"gsg", "ruby":"gsr", "saphire":"gsb", "skull":"sku", "topaz":"gsy", "flawless amethyst":"gzv", 
"flawless diamond":"glw", "flawless emerald":"glg", "flawless ruby":"glr", "flawless saphire":"glb", 
"flawless skull":"skl", "flawless topaz":"gly", "perfect diamond":"gpw", "perfect amethyst":"gpv", 
"perfect sapphire":"gpb", "perfect topaz":"gpy", "perfect ruby":"gpr", "perfect skull":"skz", 
"perfect emerald":"gpg", "el rune":"r01", "eld rune":"r02", "tir rune":"r03", "nef rune":"r04", 
"eth rune":"r05", "ith rune":"r06", "tal rune":"r07", "ral rune":"r08", "ort rune":"r09", 
"thul rune":"r10", "amn rune":"r11", "sol rune":"r12", "shael rune":"r13", "dol rune":"r14", 
"hel rune":"r15", "io rune":"r16", "lum rune":"r17", "ko rune":"r18", "fal rune":"r19", "lem rune":"r20", 
"pul rune":"r21", "um rune":"r22", "mal rune":"r23", "ist rune":"r24", "gul rune":"r25", "vex rune":"r26", 
"ohm rune":"r27", "lo rune":"r28", "sur rune":"r29", "ber rune":"r30", "jah rune":"r31", "cham rune":"r32", 
"zod rune":"r33", "antidote potion":"yps", "stamina potion":"vps", "thawing potion":"wms", 
"minor healing potion":"hp1", "minor mana potion":"mp1", "light healing potion":"hp2", "light mana potion":"mp2", 
"healing potion":"hp3", "mana potion":"mp3", "greater healing potion":"hp4", "greater mana potion":"mp4", 
"super healing potion":"hp5", "super mana potion":"mp5", "rejuv potion":"rvs", "full rejuv potion":"rvl", 
"small charm":"cm1", "large charm":"cm2", "grand charm":"cm3", "identify scroll":"isc", "town portal scroll":"tsc", 
"tome of town portal":"tbk", "tome of identify":"ibk", "arrows":"aqv", "bolts":"cqv", "jewel":"jew", 
"amulet":"amu", "gold":"gld", "ring":"rin", "ear":"ear", "torch":"tch", 
"key of terror":"pk1", "key of hate":"pk2", "key of destruction":"pk3", "diablos horn":"dhn", "baals eye":"bey",
"mephistos brain":"mbr"}

itemCodes = {};
itemCodes.IsValid = function(code) {
	for (var n in itemCode)
		if (itemCode[n] == code)
			return true;
	return false;
}
