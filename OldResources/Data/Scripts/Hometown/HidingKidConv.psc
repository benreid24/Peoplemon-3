//---------------Storing what location hidingspot you just found
if(getSaveEntry("HidingKidSpot")==1 || getSaveEntry("HidingKidSpot")==2) //--Hiding in Garbage Near PC
{
addSaveEntry("HidingKidFound1",1);
messageBox("Hiding is fun, but I guess this isn't a good enough hiding spot after all.");
}

if(getSaveEntry("HidingKidSpot")==3 || getSaveEntry("HidingKidSpot")==4) //--
{
addSaveEntry("HidingKidFound2",1);
messageBox("I guess this isn't a good enough hiding spot after all.");
}

if(getSaveEntry("HidingKidSpot")==5 || getSaveEntry("HidingKidSpot")==6) //--
{
addSaveEntry("HidingKidFound3",1);
messageBox("I guess this isn't a good enough hiding spot after all.");
}

if(getSaveEntry("HidingKidSpot")==5 || getSaveEntry("HidingKidSpot")==6) //--
{
addSaveEntry("HidingKidFound4",1);
messageBox("I guess this isn't a good enough hiding spot after all.");
}

//--------------------Deciding response based on if you've talked before or if you've found them at all locations------
if(getSaveEntry("HidingKidTalkedTo")!=1)
{
 messageBox("You found my hiding spot!","Well you won't find me next time!");
 addSaveEntry("HidingKidTalkedTo",1);
 setPlayerLock(0);
}

if(getSaveEntry("HidingPrizeGiven")!=1 && getSaveEntry("HidingKidFound1")==1 && getSaveEntry("HidingKidFound2")==1 && getSaveEntry("HidingKidFound3")==1 && getSaveEntry("HidingKidFound4")==1)
{
 messageBox("I guess there just aren't any good hiding spots in this town.","You win, so here's your prize.");
 giveItem(20);
 messageBox("Thanks for playing with me!");
 addSaveEntry("HidingPrizeGiven",1);
 setPlayerLock(0);
}

if(getSaveEntry("HidingKidFound1")!=1 || getSaveEntry("HidingKidFound2")!=1 || getSaveEntry("HidingKidFound3")!=1 || getSaveEntry("HidingKidFound4")!=1)
{
 messageBox("Just wait. I'll find ");
 setPlayerLock(0);
}