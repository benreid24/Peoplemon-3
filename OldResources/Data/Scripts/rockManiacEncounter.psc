if(getSaveEntry("JoeTalked")==1 && getSaveEntry("Pastor2Beat")==1) 
{
 setPlayerLock(1);
 playMapAnim(120,227);
 removeNPC("MagicRock1");
 sleep(1000);
 editTile(120,227,4,0);
 sleep(1000);
 spawnTrainer("RockManiac.tnr",119,238,1,1);
 trainerToLocation("RockManiac",119,228,1);
 sleep(300);
 movePlayer(2,0,1);
 sleep(200);
 messageBox("My.. my Rock!!");
 startTrainerBattle("RockManiac.tnr");
 messageBox("*hmpf*");
 trainerToLocation("RockManiac",119,238,1);
 removeTrainer("RockManiac");
 addSaveEntry("MagicRock1",1);
 setPlayerLock(0);
}
