//---------------Exchamp Enters and Leaves------------
spawnNPC("CurveForest/Exchamp3.npc",328,206,0,1);
npcToLocation("Exchamp3",playerXWorldPos()/32,playerYWorldPos()/32+1,1);
print("0");
movePlayer(2,0,1);
interact();
print("1stScriptFine");