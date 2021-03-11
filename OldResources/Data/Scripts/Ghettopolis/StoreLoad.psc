if(getSaveEntry("JustBeatM")!=1)
{
 spawnNPC("Ghettopolis/LoanShark.npc",13,8,3,0);
 
	if(getSaveEntry("ForgiveLateEmployee")==1)
	{
	 spawnNPC("Ghettopolis/StorekeeperLate2c.npc",2,6,1,0);
	}
	
	if(getSaveEntry("ForgiveLateEmployee")!=1)
	{
	 spawnNPC("Ghettopolis/StorekeeperManager.npc",2,6,1,0);
	}
}