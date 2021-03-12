messageBox("Let's make this quick. I gotta get back to the gym.");
runScript("Ghettopolis/ThugJayToMarsala.psc",1);
npcToLocation("ThugJimbo",421,189,1);
npcToLocation("ThugJacob",420,playerYWorldPos()/32-2,1);
messageBox("They're probably here for the professor.", "Go on ahead to the lab. I can take care of these guys.");
addSaveEntry("MarsalaFend",1);
setPlayerLock(0);