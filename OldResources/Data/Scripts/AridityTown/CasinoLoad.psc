spawnNPC("AridityTown/CasinoThug1.npc",9,8,2,0);	//Card Table Guy
spawnNPC("AridityTown/CasinoThug2.npc",5,7,3,0);	//Slots girl
spawnNPC("AridityTown/CasinoThug3.npc",13,10,3,0);	//Legal Stealing
spawnNPC("AridityTown/CasinoThug4.npc",8,4,2,0);	//left counter man
spawnNPC("AridityTown/CasinoThug5.npc",13,4,2,0);	//right counter man
spawnNPC("AridityTown/CasinoThug6.npc",7,11,1,0);	//Door Greeter
spawnNPC("AridityTown/CasinoStore1.npc",8,5,0,0); //Store Left
spawnNPC("AridityTown/CasinoStore2.npc",13,5,0,0); //Store Right
if(getSaveEntry("ShoeRecieved")!=1)
 spawnNPC("AridityTown/KeyHolder.npc",4,3,2,0); //Has Leader's Key
 
if(getSaveEntry("ShoeRecieved")==1)
 spawnNPC("AridityTown/KeyHolder2.npc",4,3,2,0);