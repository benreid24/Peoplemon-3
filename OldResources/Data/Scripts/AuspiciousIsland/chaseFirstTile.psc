if()==	{

 setPlayerLock(1);

 if(getSaveEntry("ChaseLineCrossed")!=1)	{
	sleep(300);
	moveTrainer("chaseThug2",0);
	messageBox("Hey! No unauthorized thugs in here.");
 }

 runScript("AuspiciousIsland/ThugChaseMessage.psc",1);
}