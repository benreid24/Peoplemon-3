trainerToLocation("ThugJacky",428,185,1);
removeTrainer("ThugJacky");
sleep(1000);
moveNPC("ThugJacob",2);
messageBox("I guess we should go after him, huh?");
sleep(500);
moveNPC("ThugJustin",0);
messageBox("Yeah, proably. You know how emotional he gets.");
sleep(200);
runScript("Ghettopolis/ThugJustinAwayDock.psc",1);
sleep(1500);
npcToLocation("ThugJustin",436,184,1);
sleep(100);
moveNPC("ThugJustin",1);
messageBox("Thanks for being cool about all this.");
choiceBox("Oh and don't go anywhere okay?", "Okay", "Sure...");
sleep(500);
messageBox("Cool. Thanks again man.");
sleep(300);
npcToLocation("ThugJustin",428,186,1);
removeNPC("ThugJustin");
sleep(300);
addSaveEntry("DockThugsLeave",1);
setPlayerLock(0);
spawnNPC("Ghettopolis/ThugJacky3.npc",397,185,2,0);
spawnNPC("Ghettopolis/ThugJustin2.npc",398,185,3,0);
spawnNPC("Ghettopolis/ThugJoe2.npc",396,185,1,0);