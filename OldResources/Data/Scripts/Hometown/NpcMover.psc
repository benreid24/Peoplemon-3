//--------------------Homeless Mom & Son---------------------------------------------
runScript("Hometown/MoveHomelessMom.psc",1);

while(getTimeHours() <=23  && (getTimeHours()) > 8) //Daytime
	{
	 setNPCLock("HomelessSon",1);
	 sleep(200);
	 npcToLocation("HomelessSon",341,243,1);
	 sleep(200);
	 setNPCLock("HomelessSon",0);
	 sleep(500);
	
	}
	
while(getTimeHours() <=8  || getTimeHours() >23 ) //NightTime
	{
	 setNPCLock("HomelessSon",1);
	 sleep(200);
	 npcToLocation("HomelessSon",349,250,1);
	 sleep(200);
	 setNPCLock("HomelessSon",0);
	 sleep(500);
	}