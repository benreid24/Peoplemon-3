setPlayerLock(1);
startTrainerBattle("TriliteIsland/ChampsGirl.tnr");

spawnTrainer("TriliteIsland/Champ.tnr",5,12,0,1);
runScript("npcToLocation(\"Champ\", playerXWorldPos() / 32, playerYWorldPos() / 32 +1 ,1);",1);
sleep(500);
messageBox("Pooooooop!","I gotta go now, bye.");
npcToLocation("ChampsGirl",6,12,0);
sleep(1500);
movePlayer(2,0,1);
