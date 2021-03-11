spawnTrainer("TriliteIsland/ChampsGirl.tnr",5,4,2,0);
spawnNPC("TriliteIsland/TriliteDoorBlockerChamp.npc",9,3,2,0);

if (getSaveEntry("ChampBeaten")==1)
 spawnTrainer("TriliteIsland/Champ.tnr",10,7,3,0);
 
if (getSaveEntry("AllChampsBeaten")==1)
 spawnTrainer("TriliteIsland/ChampsUncle.tnr",10,5,3,0);