var Container = function (name, width, height, location) {
	this.name = name;
	this.width = width;
	this.height = height;
	this.location = location;
	this.buffer = new Array();
	this.itemList = new Array();
	this.openPositions = this.height * this.width;
	
	// Initalize the buffer array for use, set all as empty.
	for (var h = 0; h < this.height; h++) {
		this.buffer.push(new Array());
		for (var w = 0; w < this.width; w++)
			this.buffer[h][w] = 0;
	}
	
	/* Container.Mark(item)
	 *	Marks the item in the buffer, and adds it to the item list. 
	 */
	this.Mark = function(item) {
		//Make sure it is in this container.
		if (item.location != this.location || item.mode != 0)
			return false;
		Interface.message(DetailedDebug, "Marking " + item.name + "ÿc0 in " + this.name + " buffer.");
		//Mark item in buffer.
		for(var x = item.x; x < (item.x + item.sizex); x++)
			for (var y = item.y; y < (item.y + item.sizey); y++) {
				this.buffer[y][x] = this.itemList.length + 1;
				this.openPositions--;
			}
		//Add item to list.
		this.itemList.push(copyUnit(item));
		if (this.location == 0)
			Interface.writeLog("Test.log", JSON.stringify(this.buffer));
		
		return true;
	}
	
	this.Reset = function () {
		for (var h = 0; h < this.height; h++)
			for (var w = 0; w < this.width; w++)
				this.buffer[h][w] = 0;
		this.itemList = new Array();
		return true;
	}
	
	/* Container.CanFit(item)
	 *	Checks to see if we can fit the item in the buffer.
	 */
	this.CanFit = function(item) { return (!!this.FindSpot(item)); };
	
	/* Container.FindSpot(item)
	 *	Finds a spot available in the buffer to place the item.
	 */
	this.FindSpot = function(item) {
		//Make sure it's a valid item
		if (!item)
			return false;
			
		Storage.Reload();
			
		//Loop buffer looking for spot to place item.
		for (var x = 0; x < this.height - (item.sizey - 1); x++) {
			itemLoop:for (var y = 0; y < this.width - (item.sizex - 1); y++) {
				//Check if there is something in this spot.
				if (this.buffer[x][y] > 0)
					continue;
				//Loop the item size to make sure we can fit it.
				for (var nx = 0; nx < item.sizey; nx++)
					for (var ny = 0; ny < item.sizex; ny++)
						if (this.buffer[x + nx][y + ny])
							continue itemLoop;
				
				return({x:x, y:y});
			}
		}
		return false;
	}
	
	/* Container.MoveTo(item)
	 *	Takes any item and moves it into given buffer.
	 */
	this.MoveTo = function(item) {
		try {
			//Can we even fit it in here?
			var nPos = this.FindSpot(item);
			if (!nPos)
				return false;
				
			//Can't deal with items on ground!
			if (item.mode == Mode.Item.OnGround)
				return false;
			
			//Item already on the cursor.
			if (me.itemoncursor && item.mode != Mode.Item.OnCursor)
				return false;
			
			//Pick to cursor if not already.
			item.toCursor();
			
			//Loop three times to try and place it.
			for (var n = 0; n < 3; n++) {
				clickItem(0, nPos.y, nPos.x, (this.location == 2 ? 5 : this.location));
				
				var nDelay = getTickCount();
				while((getTickCount() - nDelay) < 2000) {
					if (!me.itemoncursor) {
						Interface.message(DetailedDebug, "Successfully placed " + item.name + " at X: " + nPos.x + " Y: " + nPos.y);
						return true;
					}
					delay(1);
				}
			}
			return true;
			
		} catch(e) {
			mBot.throwError(e);
			return false;
		}
	}
	
	/* Container.Dump()
	 *	Prints all known information about container.
	 */
	this.Dump = function () {
		print(this.name + " has the width of " + this.width + " and the height of " + this.height);
		print(this.name + " has " + this.itemList.length + " items inside, and has " + this.openPositions + " spots left.");
		for (var x = 0; x < this.height; x++) {
			var string = "";
			for (var y = 0; y < this.width; y++)
				string += (this.buffer[x][y] > 0) ? "ÿc1x" : "ÿc0o";
			print(string);
		}
	}
	
	/* Container.compare(reference)
	 *	Compare given container versus the current one, return all new items in current buffer.
	 */
	this.Compare = function(baseRef) {
		try {
			var itemList = new Array();
			var reference = baseRef.slice(0, baseRef.length);
			//Insure valid reference.
			if (!isObject(reference) || reference.length != this.buffer.length || reference[0].length != this.buffer[0].length)
				throw new Error("Unable to compare different containers.");				
			
			for (var h = 0; h < this.height; h++) {
				Loop:for (var w = 0; w < this.width; w++) {
					var item = this.itemList[this.buffer[h][w] - 1];
					
					if (!item)
						continue;
						
					for (var n = 0; n < itemList.length; n++)
						if (itemList[n].id == item.id)
							continue Loop;
					//Check if the buffers changed and the current buffer has an item there.
					if (this.buffer[h][w] > 0 && reference[h][w] == 0) 
						itemList.push(copyUnit(item));
				}
			}
			return itemList;
		} catch(e) {
			mBot.throwError(e);
			return false;
		}
	}
	
	this.toSource = function() {
		return this.buffer.toSource();
	}
}

var Storage = new function () {
	this.Inventory = new Container("Inventory", 10, 4, 0);
	this.Stash = new Container("Stash", 6, 8, 4);
	this.Belt = new Container("Belt", 16, 1, 2);
	this.Cube = new Container("Horadric Cube", 3, 4, 3);
	this.InvRef = [];

	this.Init = function() {
		this.Reload();
	}
	
	this.Reload = function () {
		this.Inventory.Reset();
		this.Stash.Reset();
		this.Belt.Reset();
		this.Cube.Reset();
		var item = me.getItem();
		do {
			switch(item.loc) {
				case itemLocation.Inventory:
					this.Inventory.Mark(item);
				break;
				case itemLocation.Belt:
					this.Belt.Mark(item);
				break;
				case itemLocation.Cube:
					this.Cube.Mark(item);
				break;
				case itemLocation.Stash:
					this.Stash.Mark(item);
				break;
			}
		} while(item.getNext());
		return true;
	}
	
	this.FindNewItems = function() {
		
	}
}