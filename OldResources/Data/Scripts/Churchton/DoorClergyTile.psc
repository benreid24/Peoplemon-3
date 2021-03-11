if (getDaytime() > 660 || (getDaytime()) < 480)                               //after 11am
	{
	 runScript("npcToLocation(\"\DoorClergy\"\,249,301,1); setNPCLock(\"\DoorClergy\"\,0);",1);
	 runScript("npcToLocation(\"\DoorClergy2\"\,250,301,1); setNPCLock(\"\DoorClergy2\"\,0);",1);
	}
sleep(400);
setPlayerLock(0);