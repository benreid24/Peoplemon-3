//This File moves all the NPCs to their rightful locations throughout the day
//,then it runs itself at the end of the day for the start of the next day.

//******************************NPC Movements Sorted by NPC Name*********************************

//------------------Homeless Mom And Son----------------

//Going to Night Positions (->@23)
runScriptAtTime("print(getTimeHours()); addSaveEntry(\"\HomelessMomAndSonInTransit\"\,1); npcToLocation(\"\HomelessSon\"\,349,250,1); sleep(1500); npcToLocation(\"\HomelessMom\"\,349,251,1); addSaveEntry(\"\HomelessMomAndSonNight\"\,1); addSaveEntry(\"\HomelessMomAndSonInTransit\"\,0);",23,1);

//Going to Day Positions (->@8)
runScriptAtTime("print(getTimeHours()); addSaveEntry(\"\HomelessMomAndSonInTransit\"\,1); npcToLocation(\"\HomelessMom\"\,342,243,1); sleep(1500); npcToLocation(\"\HomelessSon\"\,341,243,1); addSaveEntry(\"\HomelessMomAndSonNight\"\,0); addSaveEntry(\"\HomelessMomAndSonInTransit\"\,0);",8,1);

//------------------Gardener----------------------------

//Unblock Garden (->@2)
runScriptAtTime("print(getTimeHours()); npcToLocation(\"\Gardener\"\,328,285,1);,1);",2,1);

//Block Garden (->@4)
runScriptAtTime("print(getTimeHours()); npcToLocation(\"\Gardener\"\,327,287,1);,1);",4,1);










//************************************Script runs self for next day*******************************************
runScriptAtTime("Hometown/HometownNPCMover.psc",0,1);