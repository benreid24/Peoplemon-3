	//---------Thug Runs Away----------------------
	sleep(500);
	npcToLocation("hometownThug1",366,251,1);
	removeNPC("hometownThug1");
	sleep(200);
	
	//---------Aster Runs Home---------------
	spawnNPC("Hometown/aster2.npc",364,253,0,1);
	npcToLocation("aster2",playerXWorldPos()/32,242,1);
	sleep(300);
		print("Before Interact");
	interact;
		print("After Interact");
	sleep(500);
	npcToLocation("aster2",368,239,1);
	moveNPC("aster2",0);
		print("After 2nd Location");
	sleep(500);
	removeNPC("aster2");
	removeNPC("hometownThug1");
	setCollisions(368,238,1,1,0);
	setCollisions(334,238,5,1,0);
	setPlayerLock(0);
	
	//---------Spawn Thugs in Curlinear Forest----
	spawnNPC("CurveForest/curveForestBlocker2.npc",347,193,1,0);
	spawnTrainer("CurveForest/Thug1.tnr",316,195,2,0);
	addSaveEntry("thugsRunAway",1);
	
	//-------------Spawn ExChamp---------------
	spawnNPC("Hometown/Exchamp1.npc",345,225,1,0);