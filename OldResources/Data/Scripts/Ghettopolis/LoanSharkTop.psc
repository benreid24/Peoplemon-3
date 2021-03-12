movePlayer(2,0,1);
moveNPC("LoanShark",0);
messageBox("Let me give you some money.");
alterMoney(2000);
messageBox("It's a small loan.","Don't worry about it. Go buy yourself something nice.");
setPlayerLock(0);
addSaveEntry(LoanSharkTalked,1);