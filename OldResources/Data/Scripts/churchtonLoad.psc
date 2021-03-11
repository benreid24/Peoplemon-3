runScript("Churchton/DoorClergySpawner.psc",1);
runScript("Churchton/MansionTreeSpawner.psc",1);
if (getSaveEntry("GotDivineBadge") !=1)
{
	spawnNPC("Churchton/Thug1.npc",200,296,3,0);
	spawnNPC("Churchton/Thug3.npc",200,295,3,0);
	spawnNPC("Churchton/Thug1.npc",200,294,3,0);
	spawnNPC("Churchton/Thug2.npc",198,295,3,0);
}