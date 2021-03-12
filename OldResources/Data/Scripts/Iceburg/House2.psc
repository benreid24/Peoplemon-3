if(getSaveEntry("SleepInBed")!=1)
 spawnNPC("Iceburg/BrattyKidHouse.npc",12,8,3,0);

if(getSaveEntry("SleepInBed")==1 && getSaveEntry("BrattyKidFound")!=1)
 spawnNPC("Iceburg/BrattyDad.npc",11,7,1,0);
 
if(getSaveEntry("BrattyKidFound")==1) {
 spawnNPC("Iceburg/BrattyDad2.npc",6,5,2,0);
 spawnNPC("Iceburg/BrattyKidHouse2.npc",9,7,1,0);
}