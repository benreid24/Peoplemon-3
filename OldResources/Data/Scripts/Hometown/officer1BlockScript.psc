if (getSaveEntry("professorDead")!=1 && getSaveEntry("Delivered")!=1 && playerXWorldPos()/32>=343 && playerYWorldPos()/32<=345) //added check to make sure we don't try and make the npc walk into the trees
{
	if (playerXWorldPos()/32!=npcXPos("officer1")) //don't lock the player if the npc is already underneath him
		setPlayerLock(1);
	npcToLocation("officer1",playerXWorldPos()/32,261,1);
	setPlayerLock(0);
}