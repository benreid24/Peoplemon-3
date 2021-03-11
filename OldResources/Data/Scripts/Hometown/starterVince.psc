//Concurrent
if (getSaveEntry("gotStarter")!=1 && playerDir()==3 && controlPressed(5)==1)
{
	setPlayerLock(1);
	messageBox("Vince, the Lazy Peoplemon.");
	print("poop");
	string c = choiceBox("Is this the Peoplemon for you?","Take Vince","Keep Looking");
    
	if (c=="Take Vince")
	{
		givePeoplemon("starterVince.ppl");
		addSaveEntry("gotStarter",1);
		addSaveEntry("Vince",1);
		editTile(1,6,3,0);
	}
	setPlayerLock(0);
	sleep(250);
}