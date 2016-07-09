# d2bs
D2BS, short for Diablo 2 Botting System, uses the open source Javascript engine named 'SpiderMonkey' to execute user scripts inside of Diablo 2. It's mirror of official svn repo (https://app.assembla.com/spaces/d2bs/wiki/Home)

* [SVN repo](https://subversion.assembla.com/svn/d2bs)
* [1.14a Function Pointers](https://docs.google.com/spreadsheets/d/1N0C0peHZGf74TghsP8JW3IH7hT4J3RFnrsIX1kdNl4g)

# How it works
## D2BS core
* hook D2 client event by memory injection. 
 * [D2Intercepts.h](branches/bob-18/D2Intercepts.h)
 * [Patch.h](branches/bob-18/Patch.h)
* query D2 status by reading specific memory
 * [D2Ptrs.h](branches/bob-18/D2Ptrs.h)
* notify event to javascript world
 * [Events.h](branches/bob-18/Events.h)
* provide JS API
 * [JSGlobalFunc.h](branches/bob-18/JSGlobalFuncs.h)

## JS script
* decide how the character behaves
 * [How to write a d2bs script](http://www.blizzhackers.cc/viewtopic.php?f=209&t=388565)
```
function main() {
   var pickit = new Pickit("items.ini"); // assume the default items file
   
   // we won't check what act we're in because we don't care
   // we'll do whatever we need to in whatever act we're in
   
   // this is from the Town Manager library, and according to
   // the documentation it restocks the TP and ID scrolls as well as repairs and heals
   Town.Restock();
   
   Town.Waypoint.Use(101); // here we're taking the waypoint to durance of hate 2
   waitForArea(101); // we're gonna wait until we're in durance 2
   precast();
   
   var stairs = findStairs(102); // find the stairs that lead to durance 3
   moveTo(stairs); // I should be checking to make sure it found the stairs, but I'll assume it did
   takeStairs(stairs);
   waitForArea(102); // wait until I'm in durance 3

   moveTo(x,y); // here I'd replace x,y with some coordinates I find that are where I want them to be: near meph, but not too close

   if(!checkSwitch()) { // if we're not on the right switch, do that first
      switchTab();
   }
   if(!checkSkills()) { // if our skills don't check out, fix 'em
      precast();
      switchToSkills();
   }
   attack(); // kill mephisto!
   pickit.grabItems(); // grab whatever he dropped if we want it

   var portal = findPortal(); // now I'll find the portal to act 4
   moveTo(portal);
   portal.interact();
   quitGame();
}
```

# Current Version
```
commit ffc972b9b2edc1f8eee75951239c1377f82f95d0
Author: bobode <bobode@080cfd25-d5d9-4c59-82ea-70d02a200d8b>
Date:   Mon Feb 22 02:01:02 2016 +0000

    me.charflags oog flags for hc and dead, extending getState from 159 to 183
        
	    git-svn-id: https://subversion.assembla.com/svn/d2bs@1856 080cfd25-d5d9-4c59-82ea-70d02a200d8b

```
