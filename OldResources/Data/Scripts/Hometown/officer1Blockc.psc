if (getSaveEntry("professorDead")!=1 && getSaveEntry("Delivered")!=1)
{
	npcToLocation("officer1",345,261,1);
	moveNPC("officer1",0);
}
setPlayerLock(0);