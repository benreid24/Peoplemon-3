if (getSaveEntry("WellThugsBeat")!=1)
{
	spawnNPC("Churchton/DoorClergy.npc",248,301,2,1);
	spawnNPC("Churchton/DoorClergy2.npc",251,301,2,1);
	
while(getDaytime() <= 660 && (getDaytime()) > 480)
	{
	 npcToLocation("DoorClergy",248,301,1);
	 setNPCLock("DoorClergy",0);
	 npcToLocation("DoorClergy2",251,301,1);
	 setNPCLock("DoorClergy2",0);
	sleep(1000);
	}
}
elif (getSaveEntry("MovedClergy")!=1)
{
	spawnNPC("Churchton/DoorClergy3.npc",249,301,2,0);
	spawnNPC("Churchton/DoorClergy4.npc",250,301,2,0);
}
else
{
	spawnNPC("Churchton/DoorClergy3.npc",248,301,2,0);
	spawnNPC("Churchton/DoorClergy4.npc",251,301,2,0);
}
