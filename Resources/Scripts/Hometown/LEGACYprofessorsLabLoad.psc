//---------------------Spawning the Starters-------------------------------

	if (getSaveEntry("Joe")!=1 && getSaveEntry("professorDead")!=1)
	 editTile(1,4,3,326);

	if (getSaveEntry("Nick")!=1 && getSaveEntry("professorDead")!=1)
	 editTile(1,5,3,326);

	if (getSaveEntry("Vince")!=1 && getSaveEntry("professorDead")!=1)
	 editTile(1,6,3,326);

//----------------------Spawning the Professor---------------------------
if (getSaveEntry("Delivered")!=1)
	 spawnNPC("Hometown/professor1.npc",8,4,2);
	
if (getSaveEntry("Delivered")==1 && getSaveEntry("aster1Done")!=1)
	 spawnNPC("Hometown/professor2.npc",8,4,2);

if (getSaveEntry("aster1Done")==1 && getSaveEntry("professorDead")!=1)
	{
	 spawnNPC("Hometown/professor3.npc",9,12,0);
	 
	//Professor Walks/Talks
	 setPlayerLock(1);
	 runScript("Hometown/professorsLabProfessorDies.psc",1);
	}
	
//-------------------------Spawning the Fridge----------------------------
if (getSaveEntry("professorDead")==1)
	{
	 editTile(16,1,2,0);
	}