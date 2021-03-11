if(getSaveEntry("WellThugsBeat")!=1)
{
 spawnTrainer("TheWell/ThugPaul.tnr",16,39,2,0);
 spawnTrainer("TheWell/ThugMary.tnr",18,18,0,0);
 if (trainerDefeated("ThugPeter")!=1)
  {
   spawnTrainer("TheWell/ThugPeter.tnr",32,5,2,0);
  }
 else
  spawnTrainer("TheWell/ThugPeter.tnr",32,7,2,0);
}

if(getSaveEntry("BeatIce")==1 && getSaveEntry("VincingtonBeat")!=1) {
 spawnTrainer("TheWell/InvThug1.tnr",6,40,1,0);
 spawnTrainer("TheWell/InvThug2.tnr",16,41,0,0);
 spawnTrainer("TheWell/InvThug6.tnr",3,8,2,0);
 spawnTrainer("TheWell/InvThug3.tnr",25,27,2,0);
 spawnTrainer("TheWell/InvThug4.tnr",34,7,1,0);
 spawnTrainer("TheWell/InvThug5b.tnr",40,27,3,0);
 spawnTrainer("TheWell/InvThug5a.tnr",40,25,1,0);
 spawnTrainer("TheWell/InvThug5.tnr",40,26,0,0);
 spawnTrainer("TheWell/InvThug7.tnr",59,26,2,0);
 spawnTrainer("TheWell/InvThug8.tnr",60,32,0,0);
 spawnTrainer("TheWell/InvThug9.tnr",34,48,1,0);

 spawnTrainer("TheWell/ThugGrunt.tnr",1,27,1,0);
 spawnTrainer("TheWell/ThugGrunt.tnr",2,27,3,0);
 spawnTrainer("TheWell/ThugGrunt.tnr",1,26,2,0);
 spawnTrainer("TheWell/ThugGrunt.tnr",11,39,1,0);
 spawnTrainer("TheWell/ThugGrunt.tnr",12,39,3,0);
 spawnTrainer("TheWell/ThugGruntC.tnr",11,25,0,0);
 spawnTrainer("TheWell/ThugGruntC.tnr",9,27,3,0);
 spawnTrainer("TheWell/ThugGruntC.tnr",13,27,1,0);
 spawnTrainer("TheWell/ThugGruntC.tnr",15,25,2,0);
 spawnTrainer("TheWell/ThugGruntC.tnr",17,27,3,0);
 spawnTrainer("TheWell/ThugGrunt.tnr",1,16,3,0);
 spawnTrainer("TheWell/ThugGrunt.tnr",3,16,1,0);
 spawnTrainer("TheWell/ThugGrunt.tnr",2,18,1,0);
 spawnTrainer("TheWell/ThugGrunt.tnr",2,20,3,0);
 spawnTrainer("TheWell/ThugGruntCC.tnr",17,7,0,0);
 spawnTrainer("TheWell/ThugGruntC.tnr",57,9,1,0);
 spawnTrainer("TheWell/ThugGruntC.tnr",50,9,3,0);
 spawnTrainer("TheWell/ThugGruntUD.tnr",9,8,2,0);
 spawnTrainer("TheWell/ThugGruntUD.tnr",10,18,2,0);
 spawnTrainer("TheWell/ThugGruntUD.tnr",39,13,2,0);
 
}
if(getSaveEntry("VincingtonBeat")==1 && getSaveEntry("Aholes")!=1) {
 spawnNPC("TheWell/LAThugM.npc",39,27,1,0);
 spawnNPC("TheWell/LAThugF.npc",39,26,1,0);
 spawnNPC("TheWell/LAThugM.npc",39,25,1,0);
 spawnNPC("TheWell/LAThugM.npc",40,27,1,0);
 spawnNPC("TheWell/LAThugM.npc",40,26,1,0);
 spawnNPC("TheWell/LAThugM.npc",40,25,1,0);
 spawnNPC("TheWell/LAThugM.npc",41,27,1,0);
 spawnNPC("TheWell/LAThugF.npc",41,26,1,0);
 spawnNPC("TheWell/LAThugF.npc",41,25,1,0);
 spawnNPC("TheWell/LAThugM.npc",42,27,1,0);
 spawnNPC("TheWell/LAThugF.npc",42,26,1,0);
 spawnNPC("TheWell/LAThugM.npc",42,25,1,0);
 spawnNPC("TheWell/LAThug3.npc",43,27,1,0);
 spawnNPC("TheWell/LAThug2.npc",43,26,1,0);
 spawnNPC("TheWell/LAThug1.npc",43,25,1,0);
 spawnNPC("TheWell/LAThugL.npc",44,26,1,0);
}

if(getSaveEntry("VincingtonBeat")==1 && getSaveEntry("Aholes")!=1) {
 spawnNPC("TheWell/LThugL.npc",48,26,1,0);
 spawnNPC("TheWell/LAThugM.npc",49,25,1,0);
 spawnNPC("TheWell/LAThugF.npc",50,25,2,0);
 spawnNPC("TheWell/LAThugFS.npc",49,27,1,0);
 spawnNPC("TheWell/LAThugM.npc",50,26,0,0);
 spawnNPC("TheWell/LAThugM.npc",50,27,0,0);
 spawnNPC("TheWell/LAThugF.npc",51,25,3,0); 
}

if(getSaveEntry("Aholes")==1 && getSaveEntry("Crap")!=1) {
 spawnNPC("TheWell/LThugL.npc",48,26,3,0);
 spawnNPC("TheWell/LAThugM.npc",49,25,3,0);
 spawnNPC("TheWell/LAThugF.npc",50,25,3,0);
 spawnNPC("TheWell/LAThugFS.npc",49,27,3,0);
 spawnNPC("TheWell/LAThugM.npc",50,26,3,0);
 spawnNPC("TheWell/LAThugM.npc",50,27,3,0);
 spawnNPC("TheWell/LAThugF.npc",51,25,3,0); 

if(getSaveEntry("Aholes")==1 && getSaveEntry("Crap")!=1)
{
setPlayerLock(1);
runScript("TheWell/Fractured.psc",1);
}