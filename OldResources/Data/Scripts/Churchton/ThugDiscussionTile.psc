if (getSaveEntry("GotDivineBadge") !=1)
{
moveNPC("Thug2",1);
sleep(2000);
messageBox("Stay ready everyone.","We'll be heading over to set up our new base soon.");
sleep(500);
moveNPC("Thug2",3);
sleep(500);
messageBox("That Well has to be around here somewhere...");
addSaveEntry("TalkedThugsChurchton",1);
}
setPlayerLock(0);