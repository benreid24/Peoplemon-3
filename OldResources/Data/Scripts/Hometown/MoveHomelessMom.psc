while(getTimeHours() <=23  && (getTimeHours()) > 8) //Daytime
	{
	 setNPCLock("HomelessMom",1);
	 sleep(200);
	 npcToLocation("HomelessMom",342,243,1);
	 sleep(200);
	 setNPCLock("HomelessMom",0);
	 sleep(500);
	
	}
	
while(getTimeHours() <=8  || getTimeHours() >23 ) //NightTime
	{
	 setNPCLock("HomelessMom",1);
	 sleep(200);
	 npcToLocation("HomelessMom",349,251,1);
	 sleep(200);
	 setNPCLock("HomelessMom",0);
	 sleep(500);
	}