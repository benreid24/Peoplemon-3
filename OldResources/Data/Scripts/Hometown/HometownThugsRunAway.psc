if (getSaveEntry("professorDead")==1 && getSaveEntry("thugsRunAway")!=1)
	{
	 setPlayerLock(1);
	 runScript("Hometown/HometownThugsRunAway2.psc",1);
	}