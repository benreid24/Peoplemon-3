if (getSaveEntry("ReadyForBoss1")==1 && getSaveEntry("BeatBoss1")!=1)
	{
	 spawnTrainer("Ghettopolis/ThugBossJordan.tnr",10,7,1,0);
	 spawnNPC("Ghettopolis/GProf.npc",11,7,3,0);
	 setTrainerBeaten("ThugJerry2",0);
	 setPlayerLock(1);
	 runScript("Ghettopolis/LabLoad2.psc",1);
	}

if (getSaveEntry("BeatBoss1")==1)
	 spawnNPC("Ghettopolis/GProf.npc",11,7,3,0);