if (getSaveEntry("AuspiciousBoatComplete")!=1)
 spawnNPC("AuspiciousIsland/EntranceThug.npc",303,50,2,0);
 
if (getSaveEntry("AlmostDone")==1 && getSaveEntry("BossThugBeaten")!=1) {
 spawnNPC("AuspiciousIsland/ThugBoss.npc",302,29,2,0);
 setCollisions(305,33,2,2,0);
 editTile(305,32,5,400);
}