setPlayerLock(1);
sleep(300);
moveNPC("ThugBoss2",3);
sleep(300);
messageBox("I'm trying to turn over a new leaf, so I will no longer be doing business with you.","I think we're gonna head out now.","Sorry Mr. Kid, sir. We're gonna get out of your hair.");
sleep(300);
runScript("npcToLocation(\"\ThugBoss2\"\,9,18,1); removeNPC(\"\ThugBoss2\"\);",1);
runScript("sleep(1500); npcToLocation(\"\ThugMary2\"\,8,20,1); removeNPC(\"\ThugMary2\"\);",1);
runScript("sleep(3000); npcToLocation(\"\ThugPaul2\"\,11,19,1); removeNPC(\"\ThugPaul2\"\); ",1);
runScript("sleep(4500); npcToLocation(\"\ThugPeter2\"\,13,19,1); removeNPC(\"\ThugPeter2\"\); ",1);
sleep(3000);


if( ( playerXWorldPos()/32 - npcXPos("Pastor2")/32) < 1  || ( npcXPos("Pastor2")/32 - playerXWorldPos()/32) < 1 )
{
	if(playerYWorldPos()/32 < npcYPos("Pastor2")/32)
	{
	 moveNPC("Pastor2",0);
	}
	else
	{
	 moveNPC("Pastor2",2);
	}
}
elif(playerXWorldPos()/32 < npcXPos("Pastor2")/32)
{
 moveNPC("Pastor2",3);
}
sleep(500);
messageBox("You're probably thinking it's suspicious, a Pastor talking to Thugs.","I certainly can't let the word get out.","I'm sorry, but your church membership has just expired.");
startTrainerBattle("Churchton/Pastor2.tnr");
messageBox("Okay. I'll confess.","Our town has a mandatory tithe. When people can't pay up, we take parts of their Peoplemon instead.","I sell them to the local thugs so our church can recoup the losses.","They use them for experiments, making Peopleblobs and what not, but the point is, everyone's happy.","I know it seems wrong, but how can we continue God's work if we don't have the money to buy large screen Tv's?","Just..here.","You received 3 Peoplemon Parts");
giveItem(135);
giveItem(135);
giveItem(135);
addSaveEntry("Pastor2Beat",1);
messageBox("Give them back to their original owners. Keep them. I don't care.","Let's just forget this whole thing ever happened.");
string PastorChoice = choiceBox("Okay?","Okay","No");
addSaveEntry("PastorChoice", PastorChoice);

if(PastorChoice=="Okay")
{
 messageBox("Oh wow, really? Thank the Lord, Jebus Christ.","God bless you.","Now please be on your way.");
}
else
{
 messageBox("But I'll never make it in prison!","I guess I better get out of here. Time to make my escape!");
 loadMap("Churchton/church",2);
 }
setPlayerLock(0);