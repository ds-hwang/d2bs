/* Pather Library
 *	Summary:
 *		Moves to any given point in any given area, moves thru waypoints, tiles, and doors.
 *	Notes:
 *		Still a little buggy, but it's still a nice pathing library, only thing I feel
 *		it's missing is cross-area-pathing, but that is a huge pain in the ass.
 *	Functions:
 *		Pather.Init() - Initalizes pathing, also known as reads configuration.
 *		Pather.moveTo({object}) - Moves to a given point, with a wide varity of configurable options.
 *		Pather.moveNode(nPos, nUseTele) - Moves to a given node, given an x/y and method of moving.
 *		Pather.moveExit(nLevelId, nMoveThru) - Moves to a level entrance, and optionally moves through it.
 *		Pather.movePreset(nType, presetId, [...]) - Looks for first preset based on arguments, and moves to it.
 *		Pather.useDoors() - Looks for any doors in range, and opens them.
 *		Pather.moveToTown([true]) - Casts a portal, and if no arguments given, moves through the portal.
 *		Pather.useWaypoint(nAreaId) - Finds waypoint, moves to it, and uses it to move to given area.
 *	Configuation:
 *		[Pather]
 *		TeleRange - Sets the default range for the bot to move per teleport, default is 35.
 *		TeleRange=35
 *		WalkRange - Sets the default range for the bot to move per walk, default is 20.
 *		WalkRange=20
 *	Todo:
 *		@Cross area pathing.
 *		@Fix some little bugs while moving, if I can ever find them.
 *		@Fix some issues with crossing certain areas(Ex Monastgery Gate).
 *	Library Requirements:
 *		Interface.js - Reading configuration and debugging.
 *		mBot.js - Checking modes, checking if in town.
 *		Skills.js - Checking teleport skill level, and skill id constants.
 *		Constants.js - For constants, Ex ClickType
 *		Unit.js - For finding TP Tome.
 */
var Pather = new function () {
	this.config = {};
	
	/* Pather.Init()
	 *	Initalizes pather, reads configuration.
	 */
	this.Init = function () {
		this.config = Interface.readConfig("Pather", [{Name:"TeleRange", Default:35}, {Name:"WalkRange", Default:20}]);
	}
	
	/* Pather.moveTo(nArgs)
	 *		Moves to a given place based on arguments.
	 *	Arguments:
	 *		x - The x position of the end point.
	 *		y - The y position of the end point.
	 *		preset - The preset you wish to move to.
	 *		level(optional) - The area(or level) you want to path thru.
	 *		distance(optional) - How far you want to be from the end before returning.
	 *		pop(optional) - Sets if you want to pop the last node.
	 *		force(optional) - Sets if you want to forcefully move onto the end point.
	 */
	this.moveTo = function(nArgs) {
		try {
			Interface.message(Debug, "moveTo called with " + arguments.length + " argument(s)");
			
			//Setup teleport and range variables
			this.nPos = {x:0,y:0};
			this.useTele = (!mBot.inTown() && Skill.get(Skill.Teleport).level > 0);
			this.range = (this.useTele) ? this.config.TeleRange : this.config.WalkRange;
			this.pop = false;
			this.level = me.area;
			this.distance = 3;
				
			//Check for specified unit/preset, incase they want other args
			if (nArgs.hasOwnProperty("unit")) {
				this.nPos.x = nArgs.unit.x;
				this.nPos.y = nArgs.unit.y;
			}
			
			//Checks if we are given an x/y argument, ethier static or from a unit/area/exit.
			if (nArgs.hasOwnProperty("x") && nArgs.hasOwnProperty("y")) {
				Interface.message(DetailedDebug, "Given an x/y argument, x: " + nArgs.x + ", y: " + nArgs.y);
				this.nPos = {x:nArgs.x, y:nArgs.y};
			}
				
			//Checks if we are given a preset argument, and calculates the end point.
			if (nArgs.hasOwnProperty("preset")) {
				Interface.message(DetailedDebug, "Given a preset unit, type: " + nArgs.preset.type + ", id: " + nArgs.preset.id);
				this.nPos.x = (nArgs.preset.roomx * 5) + nArgs.preset.x;
				this.nPos.y = (nArgs.preset.roomy * 5) + nArgs.preset.y;
			}
			
			//Checks if we are given a static level we are pathing through.
			if (nArgs.hasOwnProperty("level")) {
				Interface.message(DetailedDebug, "Given a level to use: " + nArgs.level);
				this.level = nArgs.level;
			}

			if (nArgs.hasOwnProperty("range")) {
				this.range = nArgs.range;
			}
			
			//Checks if we are given a distance from the end point before returning.
			if (nArgs.hasOwnProperty("distance")) {
				Interface.message(DetailedDebug, "Given a distance to use: " + nArgs.distance);
				this.distance = nArgs.distance;
			}
			
			//Checks if we told to pop the last node.
			if (nArgs.hasOwnProperty("pop") && nArgs.pop) {
				Interface.message(DetailedDebug, "Told to pop the last coordinate in the path.");
				this.pop = true;
			}
			
			//Make sure we got a valid set of coordinates
			if (this.nPos.x == 0 || this.nPos.y == 0)
				throw new Error("Was not given coordinates to move to.");
				
			//Check if it is a valid end point.
			if (getCollision(me.area, this.nPos.x, this.nPos.y) % 2)
				throw new Error("The end position is not a valid spot.");
			
			var nArea = me.area;
			
			//Main moving loop, won't end until we are there or have failed miserably.
			while(getDistance(me, this.nPos.x, this.nPos.y) > (this.distance + (this.pop ? (this.range + 1) : 0))) {
				//Checks if we can teleport.
				this.useTele = (!mBot.inTown() && Skill.get(Skill.Teleport).level > 0);
				
				//Determines the path based on all the arguments given.
				var nPath = getPath(this.level, me.x, me.y, this.nPos.x, this.nPos.y, this.useTele, this.range);
				
				//Pops the last coordinate if wanted
				if (this.pop)
					nPath.pop();
				
				//If we can't determine a path then kick out of the loop.
				if (nPath.length == 0)
					break;
					
				Interface.message(DetailedDebug, "Moving a path with " + nPath.length + " nodes, with teleport: " + this.useTele + ", range: " + this.range);
				
				//Checks if the the path given is we're we already are!
				if (getDistance(me, nPath[0][0], nPath[0][1]) == 0)
					break;
					
				//Loop thru the nodes
				for (var n in nPath) {
					//Actually move the node.
					if (!this.moveNode({x:nPath[n][0], y:nPath[n][1]}, this.useTele))
						break;
					//Checks if we have moved from town.
					if (nArea != me.area && (nArea == 1 || nArea == 40 || nArea == 75 || nArea == 103 || nArea == 109)) {
						//Check if we can teleport now, since we're out of town.
						Interface.message(DetailedDebug, "This node is in a new area and we used to be in town, checking if we can tele");
						this.useTele = (!mBot.inTown() && Skill.get(Skill.Teleport).level > 0);
						this.range = ((this.useTele) ? this.config.TeleRange : this.config.WalkRange) + 2;
						nArea = me.area;
						break;
					}
				}
				
				//Check if we have made it!
				if (getDistance(me, this.nPos.x, this.nPos.y) > (this.distance + (this.pop ? (this.range + 1) : 0)))
					break;
				
				//Reduce the distance between movements.
				this.range -= 2;
				Interface.message(DetailedDebug, "Reducing the range by 2 to: " + this.range);
				
				//If we have reduced it to nothing, we've failed miserably.
				if (this.range <= 0)
					throw new Error("Unable to move to location.");
			}
			
			//Check if we want to force a move onto the endpoint.
			if (nArgs.hasOwnProperty("force")) {
				this.moveNode({x:this.nPos.x, y:this.nPos.y}, this.useTele, true);
			}
			
			return true;
		} catch (e) {
			mBot.throwError(e);
			return false;
		}
	}
	
	/* Pather.moveNode(nPos, nUseTele)
	 *		Physically moves the bot to the given node.
	 *	Arguments:
	 *		nPos - Takes a x/y object of the node you wish to move to
	 *		nUseTele - Boolean if we should use teleport or not
	 *		nForce(optional) - Boolean if we should forcefully move on the spot.
	 */
	this.moveNode = function(nPos, nUseTele) {
		try {
			Interface.message(DetailedDebug, "Moving a node: " + nPos.x + ", " + nPos.y + ", using teleport: " + nUseTele + ", distance from node: " + getDistance(me, nPos.x, nPos.y));
			
			//Check if we are given a valid spot to move to.
			if (getCollision(me.area, nPos.x, nPos.y) % 2)
				throw new Error("Given invalid position to move to.");
			
			//Determine how we are clicking.
			var click = (nUseTele) ? ClickType.Right : ClickType.Left;
			
			//Set teleport if we need it
			if (nUseTele)
				Skill.get(Skill.Teleport).setSkill();
				
			var nFail = 0;
			var attemptCount = 0;
			//Main loop to get to the node.
			MoveLoop:while(getDistance(me, nPos.x, nPos.y) > 2 || arguments.length == 3) {
			
				//If we're walking, then we need to check for doors.
				if (!nUseTele)
					this.useDoors();
					
				
				Interface.message(DetailedDebug, "Me X: " + me.x + ", Y: " + me.y + ", nPos.x: " + nPos.x + ", y: " + nPos.y);
				Interface.message(DetailedDebug, "We are " + getDistance(me, nPos.x, nPos.y) + " yards away from the current node.");
				
				//Let's move out!
				clickMap(click.Down, Shift.Off, nPos.x, nPos.y);
				delay(5);
				clickMap(click.Up, Shift.Off, nPos.x, nPos.y);
				attemptCount++;
				
				//Watch for movement.
				while(!mBot.checkMode(Mode.Player.Group.Move)) {
					//If we haven't begun moving in 500ms, then it safe to say we aren't.
					if ((getTickCount() - nTimer) > 500) {
						//Increase fail attempts
						nFail++;
						//If we have failed twice, then repath.
						if (nFail == 2) {
							Interface.message(Debug, "This isn't going anywhere, re-path.");
							return false;
						}
						//Try moving in a random direction to un-stick ourselves.
						Interface.message(Debug, "We got stuck! Random click time, attempt #" + nFail);
						clickMap(click.Down, Shift.Off, me.x+rnd(-2,2), me.y+rnd(-2,2));
						delay(5);
						clickMap(click.Up, Shift.Off, me.x+rnd(-2,2), me.y+rnd(-2,2));
						continue MoveLoop;
					}
				}
				
				//Since we've confirmed we are moving, wait until we aren't.
				while(!mBot.checkMode(Mode.Player.Group.Neutral))
					delay(1);
					
				//If we have forced the movement, we are done here.
				if (arguments.length == 3)
					return true;
					
				//Check if we have tried and failed to move, and repath if so.
				if (attemptCount >= 3) {
					Interface.message(DetailedDebug, "Tried three times to move to given node, re-path.");
					return false;
				}
			}
		} catch(e) {
			mBot.throwError(e);
			return false;
		}
		return true;
	}
	
	/* Pather.moveExit(nLevelId, nMoveThru)
	 *	Moves to an exit to the given level.
	 *	Arguments:
	 *		nLevelId - Level id of where to go
	 *		nMoveThru - If we should move into the given level
	 */
	this.moveExit = function(nLevelId, nMoveThru) {
		try {
			//Try and get our current area.
			var nArea = getArea(me.area);
			if (!nArea)
				throw new Error("Unable to get our current area object.");
				
			//Since we have our area, get the exits.
			var nExit = getExits(nArea);
			if (!nExit)
				throw new Error("Unable to get our current exits array.");
				
			//Loop through all the exits in our area.
			for (var n in nExit) {
				//Check if this exit goes to where we want.
				if (nExit[n].id == nLevelId) {
					//Move to the exit!
					if (!this.moveTo(nExit[n]))
						return false;

					//Check if we want to move through to the next area.
					if (nMoveThru) {
						//If it is a tile, then just move through.
						if (nExit[n].type == 2) {
							//Move through the tile.
							if (!this.moveThru(nExit[n].tileid))
								return false;
						} else {
							var otherExit = getExits(getArea(nLevelId));
							if (!otherExit)
								throw new Error("Unable to obtain opposite exit to move through.");
							
							//Loop through the other sides exits to find ours.
							for (var l = 0; l < otherExit.length; l++) {
								//Check if the exit is the opposite the one we're at.
								if (otherExit[l].id == nArea.id) {
									//Move through the area
									for (var n = 0; n < 3; n++) {
										//Try to click the side we want to move to.
										clickMap(ClickType.Left.Down, Shift.Off, otherExit[l].x, otherExit[l].y);
										
										//Wait and see if we moved through
										var nDelay = getTickCount();
										while((getTickCount() - nDelay) < 500) {
											if (me.area != nArea.id)
												break;
											delay(1);
										}
										
										//Check if we have moved through
										if (me.area != nArea.id)
											break;
									}
									//Last check to see if we've moved, if not, error.
									if (me.area != nArea.id)
										throw new Error("Failed to move through exit after three tries.");
								}
							}
						}
						
						//Make sure we've moved through to where we want to be.
						var nDelay = getTickCount();
						while((getTickCount() - nDelay) < 3000) {
							if (me.area == nLevelId)
								return true;
							delay(1);
						}
						throw new Error("Unable to move thru the exit.");
					}
					return true;
				}
			}
		} catch(e) {
			mBot.throwError(e);
			return false;
		}
		return false;
	}
	
	/* Pather.moveThru(nId, [nType])
	 *		Interacts with a given unit to move thru areas.
	 *	Arguments:
	 *		nId - The id of the unit
	 *		nType - The type of unit 
	 */
	this.moveThru = function(nId) {
		try {
			//Obtain the unit we are trying to move through
			var nUnit;
			if (arguments.length == 1) {
				//We're given the unit.
				if (typeof(arguments[0]) == "object")
					nUnit = copyUnit(arguments[0]);
				else
				//We're given a tile id.
					nUnit = getUnit(5, nId);
			} else
				//We're given both a id and a type.
				nUnit = getUnit(arguments[1], nId);
				
			//Make sure we have a unit.
			if (!nUnit)
				throw new Error("moveThru unable to find unit, id: " + nId);
				
			//Store our current area for comparison.
			var nOldArea = me.area;
			
			//Try three times to move through
			for (var n = 0; n < 3; n++) {
				//Move to the place we want to move through.
				if (!this.moveTo({unit:nUnit}))
					return false;
				
				//Interact with it.
				nUnit.interact();
				
				//Wait and see if we've moved on through.
				var nDelay = getTickCount();
				while((getTickCount() - nDelay) < 2000) {
					if (nOldArea != me.area)
						break;
					delay(1);
				}
				//Check if we have moved through
				if (nOldArea != me.area)
					break;
			}
			
			//Sometimes, Diablo likes to put us in a void, while it loads the new area.
			for (var n = 0; n < 15; n++) {
				if (me.area != 0)
					break;
				delay(50);
			}

			//Make sure we actually entered the new area.
			if (me.area != nOldArea) {
				mBot.Print("Entering " + getArea(me.area).name);
				return true;
			}
			
			throw new Error("We couldn't move through into the new area.");
		} catch(e) {
			mBot.throwError(e);
			return false;
		}
	}
	
	/* Pather.movePreset(nType, nId, [nId1, nId2, ...])
	 *		Finds and moves to a preset.
	 *	Arguments:
	 *		nType - Preset type(1 = NPC, 2 = Object, ...)
	 *		nIdx - Preset id, if given multiples, first one found is used.
	 */
	this.movePreset = function(nType) {
		try {
			//Check to make sure we're given a valid type.
			if (nType < 0 || nType > 5)
				throw new Error("Given invalid preset type: " + nType);
			
			//Loop through the given id's
			for (var n = 1; n < arguments.length; n++) {
				//Check for given preset
				var preset = getPresetUnits(me.area, nType, arguments[n]);
				
				//If we didn't find any, continue.
				if (preset.length == 0)
					continue;
					
				//Move to the first found preset.
				if (!this.moveTo({preset:preset[0]}))
					continue;
					
				return true;
			}
			throw new Error("Unable to find preset to move to.");
		} catch(e) {
			mBot.throwError(e);
			return false;
		}
	}
	
	/* Pather.useDoors()
	 *		Looks through all objects, and opens doors in range.
	 */
	this.useDoors = function () {
		//Apparently looping objects in town is bugged, never ends, so we don't check in town.
		if (mBot.inTown())
			return;
		Interface.message(DetailedDebug, "We are going to try and open any doors within range.");
		//Loop objects
		var door = getUnit(2);
		do {
			//Make sure we have an object
			if (!door)
				continue;
				
			//Make sure we are within distance
			if (getDistance(me, door) > 4)
				continue;
				
			//Make sure it's a door
			if (door.name != "Door")
				continue;
				
			//Check if it is already open.
			if (door.mode == Mode.Object.Opened)
				continue;
				
			Interface.message(DetailedDebug, "Opening a door!");
			//Open the door!
			clickMap(ClickType.Left.Down, Shift.Off, door);
			delay(50,true);
			clickMap(ClickType.Left.Up, Shift.Off, door);
			return;
		} while(door && door.getNext ());
		Interface.message(DetailedDebug, "Finished looking for doors.");
	}
	
	this.moveToTown = function() {
		try {
			if (mBot.inTown())
				return true;
				
			var tome = Unit.findItem({code:"tbk", location:0, mode:0, ownerid:me.gid});
			
			if (tome.length == 0)
				throw new Error("Unable to TP to town! No tome.");
				
			var nUnit = false;
			for (var n = 0; n < 3; n++) {
				tome[0].interact();
				
				var nDelay = getTickCount();
				while((getTickCount() - nDelay) < 3000) {
					var nTp = getUnit(2,59);
					if (!nTp)
						continue;
					do {
						if (nTp.getParent() == me.name) {
							nUnit = copyUnit(nTp);
							break;
						}
					} while(nTp.getNext());
					if (nUnit)
						break;
				}
				if (nUnit)
					break;
			}
			if (!nUnit)
				throw new Error("Unable to find our portal!");
				
			if (arguments.length == 1)
				return true;
				
			if (!this.moveThru(nUnit))
				throw new Error("Couldn't move thru the portal.");
				
			return true;
		} catch(e) {
			mBot.throwError(e);
			return false;
		}
	}
	
	this.useWaypoint = function(nAreaId) {
		
		try {
			//Look for waypoint, if not there move to preset.
			if (!getUnit(2, "Waypoint"))
				if (!this.movePreset(2, 119, 157, 156, 402, 323, 288, 237, 324, 238, 496, 511, 494))
					return false;
					
			//Check again for waypoint
			var nWP = getUnit(2, "Waypoint");
			
			//Make sure we have it.
			if (!nWP)
				throw new Error("Unable to locate the Waypoint unit.")
				
			//Move to waypoint
			if (!this.moveTo(nWP))
				return false;
			
			Interface.message(Debug, "Trying to use waypoint to move to: " + nAreaId);
			for (var n = 0; n < 2; n++) {
				Interface.message(DetailedDebug, "Attempt #" + (n + 1) + " to use waypoint.");
				nWP.interact(nAreaId);
				
				var nDelay = getTickCount();
				while((getTickCount() - nDelay) < 3000) {
					if (nAreaId == me.area)
						return true;
					delay(10);
				}
			}
			throw new Error("Unable to move thru waypoint after three tries.");
			return false;
			
		} catch(e) {
			mBot.throwError(e);
			return false;
		}
	}
}