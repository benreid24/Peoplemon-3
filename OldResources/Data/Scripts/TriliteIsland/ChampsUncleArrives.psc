if(getSaveEntry("ChampBeaten")==1 && getSaveEntry("AllChampsBeaten")!=1)
{
setPlayerLock(1);
runScript("TriliteIsland/ChampsUncleArrives2.psc",1);
}