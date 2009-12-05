var Healer = new function() {
	this.hooks = [];
	this.config = {};
	this.timers = [];
	
	this.Init = function() {
		addEventListener("melife", Healer.event);
		this.config = Interface.readConfig("Healer", [
		{Name:"HealPotPercent", Default:70}, {Name:"MercPotPercent", Default:40}, {Name:"ManaPotPercent", Default: 60},
		{Name:"SmallJuvPercent", Default:40}, {Name:"SmallJuvMerc", Default:20}, {Name:"SmallJuvMana", Default: 10},
		{Name:"FullJuvPercent", Default:20}, {Name:"FullJuvMerc", Default:10}, {Name:"FullJuvMana", Default:-1},
		{Name:"HealPotDelay", Default:5000}, {Name:"ManaPotDelay", Default:5000}, {Name:"MercPotDelay", Default:5000},
		{Name:"ChickenHealth", Default:30}, {Name:"ChickenMerc", Default:-1}, {Name:"ChickenMana", Default:-1},
		{Name:"ChickenOnHostile", Default:true}]);
		this.hooks.push(new Frame(175,530,220,20));
		this.hooks.push(new Text("ÿc1Life: 0% ÿc3Mana: 0% ÿc2Merc:ÿc0 0%", 285, 547, 0, 0, 2));
		me.chickenonhostile = this.config.ChickenOnHostile;
		this.timers.Heal = getTickCount();
		this.timers.Mana = getTickCount();
		this.timers.Merc = getTickCount();
		this.event();
	}
	
	this.drinkPotion = function(nType) {
		try {
			Interface.message(DetailedDebug, "Attempting to drink a " + nType);
			var nUnit = mBot.getPotion(nType);
			if (!nUnit)
				throw new Error("Unable to find a " + nType + " potion to drink.");
			if (!nUnit.drink((arguments.length == 2) ? true : false))
				throw new Error("Unable to drink " + nUnit.name);
			if (nType.indexOf("Juv") == -1)
				Healer.timers[nType] = getTickCount();
			Interface.message(Normal, "Drank a " + nUnit.name + ".");
			return true;
		} catch(e) {
			mBot.throwError(e);
			return false;
		}
	}
	
	this.event = function() {
		var nLifePercent = Math.round((me.getStat(Stats.Life) / me.getStat(Stats.MaxLife)) * 100);
		var nManaPercent = Math.round((me.getStat(Stats.Mana) / me.getStat(Stats.MaxMana)) * 100);
		var nMercPercent = getMercHP();
		
		Healer.hooks[1].text = "ÿc1Life: " + nLifePercent + "% ÿc3Mana: " + nManaPercent + "% ÿc2Merc:ÿc0 " + (!nMercPercent ? "Dead" : nMercPercent + "%");
		
		if (mBot.inTown())
			return;
			
		//Chicken Health
		if (nLifePercent <= Healer.config.ChickenHealth) {
			quit();
			Interface.message(Warning, "Chickening from game due to critcal health.");
		}
		
		//Chicken Mana
		if (nManaPercent <= Healer.config.ChickenMana) {
			quit();
			Interface.message(Warning, "Chickening from game due to critcal mana.");
		}
		
		//Chicken Merc
		if (nMercPercent <= Healer.config.ChickenMerc) {
			quit();
			Interface.message(Warning, "Chickening from game due to critcal merc health.");
		}
		
		//Using big rejuvination potions
		if (nLifePercent <= Healer.config.FullJuvPercent)
			Healer.drinkPotion("FullJuv");
		if (nMercPercent <= Healer.config.FullJuvMerc)
			Healer.drinkPotion("FullJuv", true);
		if (nManaPercent <= Healer.config.FullJuvMana)
			Healer.drinkPotion("FullJuv");
			
		//Using small rejuvination potions
		if (nLifePercent <= Healer.config.SmallJuvPercent)
			Healer.drinkPotion("SmallJuv");
		if (nMercPercent <= Healer.config.SmallJuvMerc)
			Healer.drinkPotion("SmallJuv", true);
		if (nManaPercent <= Healer.config.SmallJuvMana)
			Healer.drinkPotion("SmallJuv");
			
		//Using regular healing and mana potions
		if (nLifePercent <= Healer.config.HealPotPercent && (getTickCount() - Healer.timers.Heal) >= Healer.config.HealPotDelay)
			Healer.drinkPotion("Heal");
			
		if (nManaPercent <= Healer.config.ManaPotPercent && (getTickCount() - Healer.timers.Mana) >= Healer.config.ManaPotDelay)
			Healer.drinkPotion("Mana");
			
		if (nMercPercent <= Healer.config.MercPotPercent && (getTickCount() - Healer.timers.Merc) >= Healer.config.MercPotDelay)
			Healer.drinkPotion("Heal", true);
	}
	
	
	
	
	
	
	
}