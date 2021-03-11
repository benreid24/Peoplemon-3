if(getSaveEntry("HomelessMomAndSonNight") != 1 && getSaveEntry("HomelessMomAndSonInTransit") != 1)
{
 sleep(500);
 moveNPC("HomelessSon",1);
 sleep(500);
 messageBox("Can we go home now mom?");
 sleep(500);
 moveNPC("HomelessMom",3);
 messageBox("Hun, you know we don't have a home.","Only the Professor and that new family have homes in this town.");
 sleep(400);
 moveNPC("HomelessMom",0);
 sleep(400);
 moveNPC("HomelessSon",0);
 setPlayerLock(0);
}

if(getSaveEntry("HomelessMomAndSonNight")==1 && getSaveEntry("HomelessMomAndSonInTransit") != 1)
{
 sleep(500);
 moveNPC("HomelessMom",0);
 messageBox("By sleeping under the side roof of someone else's home, you can avoid getting damp in poor weather.");
 sleep(400);
 moveNPC("HomelessSon",2);
 messageBox("Mom, you're so smart.");
 sleep(400);
 moveNPC("HomelessMom",1);
 sleep(500);
 moveNPC("HomelessSon",1);
 setPlayerLock(0);
}

if(getSaveEntry("HomelessMomAndSonInTransit") == 1)
{
 messageBox("Being homeless is hard.");
 setPlayerLock(0);
}
