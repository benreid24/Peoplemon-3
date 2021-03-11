if (getSaveEntry("professorDead")!=1 && getSaveEntry("Delivered")!=1)
{
setPlayerLock(1);
runScript("Hometown/officer1Blockb.psc",1);
}