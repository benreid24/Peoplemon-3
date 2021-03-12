//Activated by entering tiles (437,184-186)
moveNPC("ThugJacky",2);
sleep(400);
messageBox("Look.");
sleep(600);
moveNPC("ThugJacob",2);
moveNPC("ThugJustin",2);
sleep(1300);
choiceBox("How'd you get here kid?","That Boat Guy", "What Boat?");
sleep(600);
messageBox("That ferry is still running?!","We're gonna put a stop to that right now!");
runScript("Ghettopolis/ThugJackyToDock.psc",1);
sleep(1500);
runScript("Ghettopolis/ThugJacobToDock.psc",1);
sleep(2500);
npcToLocation("ThugJustin",444,186,1);
moveNPC("ThugJustin",3);
setPlayerLock(0);