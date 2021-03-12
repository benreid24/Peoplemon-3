if(getSaveEntry("MailReceived")==1 && getSaveEntry("Blocker4Talked")!=1)
{
spawnNPC("Hometown/Blocker4.npc",342,245,2,1);
npcToLocation("Blocker4",playerXWorldPos()/32+1,playerYWorldPos()/32,1);
sleep(500);
interact();
}

else
setPlayerLock(0);