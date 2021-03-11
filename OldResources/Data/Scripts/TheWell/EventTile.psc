if (getSaveEntry("VincingtonBeat")==1 && getSaveEntry("Crap")!=1) {
setPlayerLock(1);
runScript("TheWell/WellPostVEvent.psc",1);
}