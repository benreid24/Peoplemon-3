runScript("hometownCurveForestLoad.psc",1);
runScript("ghettopolisLoad.psc",1);
runScript("churchtonLoad.psc",1);
runScript("conflictTownLoad.psc",1);
runScript("orangeHairvilleLoad.psc",1);
runScript("aridityTownLoad.psc",1);
runScript("vincingtonLoad.psc",1);
runScript("WellEntrance.psc",1);
runScript("NativeEncampmentLoad.psc",1);
runScript("triliteIslandLoad.psc",1);
runScript("AuspiciousIslandLoad.psc",1);
runScript("AlphabetCaveLoad.psc",1);

if(getSaveEntry("MagicRock1")!=1)
 spawnNPC("MagicRock1.npc",119,226,2,0);
else
 editTile(120,227,4,0);

if(getSaveEntry("JoeTalked")!=1 || getSaveEntry("Pastor2Beat")!=1) 
 spawnNPC("RockManiac.npc",119,227,0,0);