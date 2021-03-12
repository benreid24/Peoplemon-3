if (getSaveEntry("DockTrigger")!=1)
	{
	 setPlayerLock(1);
	 runScript("Ghettopolis/BoatBlockingThugs.psc",1);
	 addSaveEntry("DockTrigger",1);
	}