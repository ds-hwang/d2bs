/**
*	@name		area.js
*
*	@author		alogwe
*
*	@version	1.0
*
*	@desc		Prototype library to extend D2BS 'Area' Class
*				
*				Note: 	Fold syntax on sets of {} for easier viewing 
*						If using notepad++ with JavaScript syntax highlighting:
*							Fold All	: Alt + 0
*							Unfold All	: Alt + Shift + 0
*/

// Area Class Properties
// Act I
// Area.prototype.__defineGetter__('isRogueEncampment', function(){ return ( this.id == 1 ); });
// Area.prototype.__defineGetter__('isBloodMoor', function(){ return ( this.id == 2 ); });
// Area.prototype.__defineGetter__('isColdPlains', function(){ return ( this.id == 3 ); });
// Area.prototype.__defineGetter__('isStonyField', function(){ return ( this.id == 4 ); });
// Area.prototype.__defineGetter__('isDarkWood', function(){ return ( this.id == 5 ); });
// Area.prototype.__defineGetter__('isBlackMarsh', function(){ return ( this.id == 6 ); });
// Area.prototype.__defineGetter__('isTamoeHighland', function(){ return ( this.id == 7 ); });
// Area.prototype.__defineGetter__('isDenOfEvil', function(){ return ( this.id == 8 ); });
// Area.prototype.__defineGetter__('isCaveLevel1', function(){ return ( this.id == 9 ); });
// Area.prototype.__defineGetter__('isUndergroundPassageLevel1', function(){ return ( this.id == 10 ); });
// Area.prototype.__defineGetter__('isHoleLevel1', function(){ return ( this.id == 11 ); });
// Area.prototype.__defineGetter__('isPitLevel1', function(){ return ( this.id == 12 ); });
// Area.prototype.__defineGetter__('isCaveLevel2', function(){ return ( this.id == 13 ); });
// Area.prototype.__defineGetter__('isUndergroundPassageLevel2', function(){ return ( this.id == 14 ); });
// Area.prototype.__defineGetter__('isHoleLevel2', function(){ return ( this.id == 15 ); });
// Area.prototype.__defineGetter__('isPitLevel2', function(){ return ( this.id == 16 ); });
// Area.prototype.__defineGetter__('isBurialGrounds', function(){ return ( this.id == 17 ); });
// Area.prototype.__defineGetter__('isCrypt', function(){ return ( this.id == 18 ); });
// Area.prototype.__defineGetter__('isMausoleum', function(){ return ( this.id == 19 ); });
// Area.prototype.__defineGetter__('isForgottenTower', function(){ return ( this.id == 20 ); });
// Area.prototype.__defineGetter__('isTowerCellarLevel1', function(){ return ( this.id == 21 ); });
// Area.prototype.__defineGetter__('isTowerCellarLevel2', function(){ return ( this.id == 22 ); });
// Area.prototype.__defineGetter__('isTowerCellarLevel3', function(){ return ( this.id == 23 ); });
// Area.prototype.__defineGetter__('isTowerCellarLevel4', function(){ return ( this.id == 24 ); });
// Area.prototype.__defineGetter__('isTowerCellarLevel5', function(){ return ( this.id == 25 ); });
// Area.prototype.__defineGetter__('isMonasteryGate', function(){ return ( this.id == 26 ); });
// Area.prototype.__defineGetter__('isOuterCloister', function(){ return ( this.id == 27 ); });
// Area.prototype.__defineGetter__('isBarracks', function(){ return ( this.id == 28 ); });
// Area.prototype.__defineGetter__('isJailLevel1', function(){ return ( this.id == 29 ); });
// Area.prototype.__defineGetter__('isJailLevel2', function(){ return ( this.id == 30 ); });
// Area.prototype.__defineGetter__('isJailLevel3', function(){ return ( this.id == 31 ); });
// Area.prototype.__defineGetter__('isInnerCloister', function(){ return ( this.id == 32 ); });
// Area.prototype.__defineGetter__('isCathedral', function(){ return ( this.id == 33 ); });
// Area.prototype.__defineGetter__('isCatacombsLevel1', function(){ return ( this.id == 34 ); });
// Area.prototype.__defineGetter__('isCatacombsLevel2', function(){ return ( this.id == 35 ); });
// Area.prototype.__defineGetter__('isCatacombsLevel3', function(){ return ( this.id == 36 ); });
// Area.prototype.__defineGetter__('isCatacombsLevel4', function(){ return ( this.id == 37 ); });
// Area.prototype.__defineGetter__('isTristram', function(){ return ( this.id == 38 ); });

// Act II
// Area.prototype.__defineGetter__('isLutGholein', function(){ return ( this.id == 40 ); });
// Area.prototype.__defineGetter__('isRockyWaste', function(){ return ( this.id == 41 ); });
// Area.prototype.__defineGetter__('isDryHills', function(){ return ( this.id == 42 ); });
// Area.prototype.__defineGetter__('isFarOasis', function(){ return ( this.id == 43 ); });
// Area.prototype.__defineGetter__('isLostCity', function(){ return ( this.id == 44 ); });
// Area.prototype.__defineGetter__('isValleyOfSnakes', function(){ return ( this.id == 45 ); });
// Area.prototype.__defineGetter__('isCanyonOfTheMagi', function(){ return ( this.id == 46 ); });
// Area.prototype.__defineGetter__('isSewersLevel1', function(){ return ( this.id == 47 ); });
// Area.prototype.__defineGetter__('isSewersLevel2', function(){ return ( this.id == 48 ); });
// Area.prototype.__defineGetter__('isSewersLevel3', function(){ return ( this.id == 49 ); });
// Area.prototype.__defineGetter__('isHaremLevel1', function(){ return ( this.id == 50 ); });
// Area.prototype.__defineGetter__('isHaremLevel2', function(){ return ( this.id == 51 ); });
// Area.prototype.__defineGetter__('isPalaceCellarLevel1', function(){ return ( this.id == 52 ); });
// Area.prototype.__defineGetter__('isPalaceCellarLevel2', function(){ return ( this.id == 53 ); });
// Area.prototype.__defineGetter__('isPalaceCellarLevel3', function(){ return ( this.id == 54 ); });
// Area.prototype.__defineGetter__('isStonyTombLevel1', function(){ return ( this.id == 55 ); });
// Area.prototype.__defineGetter__('isHallsOfTheDeadLevel1', function(){ return ( this.id == 56 ); });
// Area.prototype.__defineGetter__('isHallsOfTheDeadLevel2', function(){ return ( this.id == 57 ); });
// Area.prototype.__defineGetter__('isClawViperTempleLevel1', function(){ return ( this.id == 58 ); });
// Area.prototype.__defineGetter__('isStonyTombLevel2', function(){ return ( this.id == 59 ); });
// Area.prototype.__defineGetter__('isHallsOfTheDeadLevel3', function(){ return ( this.id == 60 ); });
// Area.prototype.__defineGetter__('isClawViperTempleLevel2', function(){ return ( this.id == 61 ); });
// Area.prototype.__defineGetter__('isMaggotLairLevel1', function(){ return ( this.id == 62 ); });
// Area.prototype.__defineGetter__('isMaggotLairLevel2', function(){ return ( this.id == 63 ); });
// Area.prototype.__defineGetter__('isMaggotLairLevel3', function(){ return ( this.id == 64 ); });
// Area.prototype.__defineGetter__('isAncientTunnels', function(){ return ( this.id == 65 ); });
// Area.prototype.__defineGetter__('isTalRashasTomb1', function(){ return ( this.id == 66 ); });
// Area.prototype.__defineGetter__('isTalRashasTomb2', function(){ return ( this.id == 67 ); });
// Area.prototype.__defineGetter__('isTalRashasTomb3', function(){ return ( this.id == 68 ); });
// Area.prototype.__defineGetter__('isTalRashasTomb4', function(){ return ( this.id == 69 ); });
// Area.prototype.__defineGetter__('isTalRashasTomb5', function(){ return ( this.id == 70 ); });
// Area.prototype.__defineGetter__('isTalRashasTomb6', function(){ return ( this.id == 71 ); });
// Area.prototype.__defineGetter__('isTalRashasTomb7', function(){ return ( this.id == 72 ); });
// Area.prototype.__defineGetter__('isDurielsLair', function(){ return ( this.id == 73 ); });
// Area.prototype.__defineGetter__('isArcaneSanctuary', function(){ return ( this.id == 74 ); });

// Act III
// Area.prototype.__defineGetter__('isKurastDocktown', function(){ return ( this.id == 75 ); });
// Area.prototype.__defineGetter__('isSpiderForest', function(){ return ( this.id == 76 ); });
// Area.prototype.__defineGetter__('isGreatMarsh', function(){ return ( this.id == 77 ); });
// Area.prototype.__defineGetter__('isFlayerJungle', function(){ return ( this.id == 78 ); });
// Area.prototype.__defineGetter__('isLowerKurast', function(){ return ( this.id == 79 ); });
// Area.prototype.__defineGetter__('isKurastBazaar', function(){ return ( this.id == 80 ); });
// Area.prototype.__defineGetter__('isUpperKurast', function(){ return ( this.id == 81 ); });
// Area.prototype.__defineGetter__('isKurastCauseway', function(){ return ( this.id == 82 ); });
// Area.prototype.__defineGetter__('isTravincal', function(){ return ( this.id == 83 ); });
// Area.prototype.__defineGetter__('isSpiderCave', function(){ return ( this.id == 84 ); });
// Area.prototype.__defineGetter__('isSpiderCavern', function(){ return ( this.id == 85 ); });
// Area.prototype.__defineGetter__('isSwampyPitLevel1', function(){ return ( this.id == 86 ); });
// Area.prototype.__defineGetter__('isSwampyPitLevel2', function(){ return ( this.id == 87 ); });
// Area.prototype.__defineGetter__('isFlayerDungeonLevel1', function(){ return ( this.id == 88 ); });
// Area.prototype.__defineGetter__('isFlayerDungeonLevel2', function(){ return ( this.id == 89 ); });
// Area.prototype.__defineGetter__('isSwampyPitLevel3', function(){ return ( this.id == 90 ); });
// Area.prototype.__defineGetter__('isFlayerDungeonLevel3', function(){ return ( this.id == 91 ); });
// Area.prototype.__defineGetter__('isKurastSewersLevel1', function(){ return ( this.id == 92 ); });
// Area.prototype.__defineGetter__('isKurastSewersLevel2', function(){ return ( this.id == 93 ); });
// Area.prototype.__defineGetter__('isRuinedTemple', function(){ return ( this.id == 94 ); });
// Area.prototype.__defineGetter__('isDisusedFane', function(){ return ( this.id == 95 ); });
// Area.prototype.__defineGetter__('isForgottenReliquary', function(){ return ( this.id == 96 ); });
// Area.prototype.__defineGetter__('isForgottenTemple', function(){ return ( this.id == 97 ); });
// Area.prototype.__defineGetter__('isRuinedFane', function(){ return ( this.id == 98 ); });
// Area.prototype.__defineGetter__('isDisusedReliquary', function(){ return ( this.id == 99 ); });
// Area.prototype.__defineGetter__('isDuranceOfHateLevel1', function(){ return ( this.id == 100 ); });
// Area.prototype.__defineGetter__('isDuranceOfHateLevel2', function(){ return ( this.id == 101 ); });
// Area.prototype.__defineGetter__('isDuranceOfHateLevel3', function(){ return ( this.id == 102 ); });

// Act IV
// Area.prototype.__defineGetter__('isPandemoniumFortress', function(){ return ( this.id == 103 ); });
// Area.prototype.__defineGetter__('isOuterSteppes', function(){ return ( this.id == 104 ); });
// Area.prototype.__defineGetter__('isPlainsOfDespair', function(){ return ( this.id == 105 ); });
// Area.prototype.__defineGetter__('isCityOfTheDamned', function(){ return ( this.id == 106 ); });
// Area.prototype.__defineGetter__('isRiverOfFlame', function(){ return ( this.id == 107 ); });
// Area.prototype.__defineGetter__('isChaosSanctuary', function(){ return ( this.id == 108 ); });

// Act V
// Area.prototype.__defineGetter__('isHarrogath', function(){ return ( this.id == 109 ); });
// Area.prototype.__defineGetter__('isBloodyFoothills', function(){ return ( this.id == 110 ); });
// Area.prototype.__defineGetter__('isFrigidHighlands', function(){ return ( this.id == 111 ); });
// Area.prototype.__defineGetter__('isArreatPlateau', function(){ return ( this.id == 112 ); });
// Area.prototype.__defineGetter__('isCrystallinePassage', function(){ return ( this.id == 113 ); });
// Area.prototype.__defineGetter__('isFrozenRiver', function(){ return ( this.id == 114 ); });
// Area.prototype.__defineGetter__('isGlacialTrail', function(){ return ( this.id == 115 ); });
// Area.prototype.__defineGetter__('isDrifterCavern', function(){ return ( this.id == 116 ); });
// Area.prototype.__defineGetter__('isFrozenTundra', function(){ return ( this.id == 117 ); });
// Area.prototype.__defineGetter__('isAncientsWay', function(){ return ( this.id == 118 ); });
// Area.prototype.__defineGetter__('isIcyCellar', function(){ return ( this.id == 119 ); });
// Area.prototype.__defineGetter__('isArreatSummit', function(){ return ( this.id == 120 ); });
// Area.prototype.__defineGetter__('isNihlathaksTemple', function(){ return ( this.id == 121 ); });
// Area.prototype.__defineGetter__('isHallsOfAnguish', function(){ return ( this.id == 122 ); });
// Area.prototype.__defineGetter__('isHallsOfPain', function(){ return ( this.id == 123 ); });
// Area.prototype.__defineGetter__('isHallsOfVaught', function(){ return ( this.id == 124 ); });
// Area.prototype.__defineGetter__('isAbaddon', function(){ return ( this.id == 125 ); });
// Area.prototype.__defineGetter__('isPitOfAcheron', function(){ return ( this.id == 126 ); });
// Area.prototype.__defineGetter__('isInfernalPit', function(){ return ( this.id == 127 ); });
// Area.prototype.__defineGetter__('isWorldstoneKeepLevel1', function(){ return ( this.id == 128 ); });
// Area.prototype.__defineGetter__('isWorldstoneKeepLevel2', function(){ return ( this.id == 129 ); });
// Area.prototype.__defineGetter__('isWorldstoneKeepLevel3', function(){ return ( this.id == 130 ); });
// Area.prototype.__defineGetter__('isThroneOfDestruction', function(){ return ( this.id == 131 ); });
// Area.prototype.__defineGetter__('isWorldstoneChamber', function(){ return ( this.id == 132 ); });

// Special Areas
// Area.prototype.__defineGetter__('isMooMooFarm', function(){ return ( this.id == 39 ); });
// Area.prototype.__defineGetter__('isMatronsDen', function(){ return ( this.id == 133 ); });
// Area.prototype.__defineGetter__('isForgottenSands', function(){ return ( this.id == 134 ); });
// Area.prototype.__defineGetter__('isFurnaceOfPain', function(){ return ( this.id == 135 ); });
// Area.prototype.__defineGetter__('isUberTristram', function(){ return ( this.id == 136 ); });











