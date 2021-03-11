npcToLocation("Exchamp3",328,206,1);
removeNPC("Exchamp3");

//---------------Officer enters-------------
spawnNPC("CurveForest/Officer3.npc",328,205,0,1);
npcToLocation("Officer3",playerXWorldPos()/32,playerYWorldPos()/32+1,1);
sleep(600);
movePlayer(2,0,1);
interact();

removeNPC("sketchyCharacter2");
removeNPC("officer2");
removeNPC("curveForestBlocker2");
addSaveEntry("boatGuy1",1);
setCollisions(368,238,1,1,1);
setCollisions(334,238,5,1,1);
spawnTrainer("CurveForest/aster3.tnr",319,170,1,0);