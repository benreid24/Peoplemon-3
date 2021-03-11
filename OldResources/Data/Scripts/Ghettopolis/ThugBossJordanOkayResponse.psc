if (getSaveEntry("Okay")==1)
	{
	 setPlayerLock(1);
	 int playerx = playerXWorldPos();
	 int playery = playerYWorldPos();
	 trainerToLocation("ThugBossJordan",playerx,playery-1,1);
	 moveTrainer("ThugBossJordan",2);
	 messageBox("You can't honestly think I'd let you leave.","I'm managing this operation. No loose ends.","And they don't just hand out promotions.. I'm pretty tough.","It's just unfortunate timing for you kid.","Get ready to feel the power of a Local Thug Mid to Low level Boss!"); 
	 startTrainerBattle("Ghettopolis/ThugBossJordan.tnr");
	}