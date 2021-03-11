if(getSaveEntry("BeatGirlTalkedGuard")==1 && getSaveEntry("ChampBeaten")!=1)
{
setPlayerLock(1);
runScript("TriliteIsland/ChampArrives2.psc",1);
}