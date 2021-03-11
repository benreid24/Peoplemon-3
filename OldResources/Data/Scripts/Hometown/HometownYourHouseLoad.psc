if (getSaveEntry("professorDead")!=1)
	spawnNPC("Hometown/mom2.npc",17,16,0);

if (getSaveEntry("professorDead")==1 && getSaveEntry("AllChampsBeaten")!=1)
  spawnNPC("Hometown/mom3.npc",4,5,2);