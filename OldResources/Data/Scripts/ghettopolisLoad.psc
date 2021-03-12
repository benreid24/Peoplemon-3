spawnNPC("Ghettopolis/trashGuy.npc",419,182,0,0);

if (getSaveEntry("MarsalatoTown")!=1)
	{
	 spawnNPC("Ghettopolis/Marsala.npc",391,111,0,0);
	 spawnNPC("Ghettopolis/FoodLady.npc",414,164,3,0);
	}

if (getSaveEntry("MarsalatoTown")==1 && getSaveEntry("ThugBossJordanBeat")!=1)
	{
	 spawnNPC("Ghettopolis/Marsala2.npc",420,190,0,0);
	 if (getSaveEntry("MarsalaFend")!=1)
		{
		 spawnNPC("Ghettopolis/ThugJimbo.npc",416,181,2,0);
		 spawnNPC("Ghettopolis/ThugJay.npc",417,181,2,0);
		 spawnNPC("Ghettopolis/ThugJacob2.npc",418,181,2,0);
		}
		
	 if (getSaveEntry("MarsalaFend")==1)
		{
		 spawnNPC("Ghettopolis/ThugJimbo.npc",421,189,2,0);
		 spawnNPC("Ghettopolis/ThugJay.npc",419,189,2,0);
		 spawnNPC("Ghettopolis/ThugJacob2.npc",421,189,2,0);
		}
		
	}
//-----------------Spawning Boat Guys-------------------
spawnNPC("Ghettopolis/boatGuy3.npc",413,133,3,0);

if (getSaveEntry("boatGuy1") == 1)
	{
	 spawnNPC("Ghettopolis/boatGuy2.npc",445,184,3,0);
	}


//--------------------------Spawning Thugs---------------------	
if (getSaveEntry("ThugBossJordanBeat")!=1)
	{
	 spawnNPC("Ghettopolis/ThugJimmy.npc",435,180,2,0);//Boathouse Door
	 spawnTrainer("Ghettopolis/IT1.tnr",395,127,0,0);
	 spawnTrainer("Ghettopolis/IT2.tnr",398,128,3,0);
	 spawnTrainer("Ghettopolis/IT3.tnr",404,126,3,0);
	 spawnTrainer("Ghettopolis/IT4.tnr",404,123,1,0);
	 spawnTrainer("Ghettopolis/IT5.tnr",399,124,1,0);
	 spawnTrainer("Ghettopolis/IT6.tnr",401,122,0,0);
	}
	
if (getSaveEntry("DockTrigger")!=1)
	{
	 spawnNPC("Ghettopolis/ThugJacky.npc",435,181,0,0);//Near Boat Guy Island 1 (Leader)
	 spawnNPC("Ghettopolis/ThugJacob.npc",434,180,1,0);//Near Boat Guy Island 1
	 spawnNPC("Ghettopolis/ThugJustin.npc",436,181,3,0);//Near Boat Guy Island 1
	}

if (getSaveEntry("DockTrigger")==1 && getSaveEntry("MandP")!=1)
	{
	 spawnNPC("Ghettopolis/ThugJacob.npc",444,185,3,0);//Near Boat Guy Island 1
	 spawnNPC("Ghettopolis/ThugJustin.npc",444,186,0,0);//Near Boat Guy Island 1 
	 spawnNPC("Ghettopolis/ThugJacky.npc",443,184,1,0);//Near Boat Guy Island 1 (Leader)
	}
	
if (getSaveEntry("DockThugsLeave")!=1 && getSaveEntry("MandP")==1)
	{
	 spawnTrainer("Ghettopolis/ThugJacky.tnr",443,184,3,0);//Near Boat Guy Island 1 (Leader)
	 spawnNPC("Ghettopolis/ThugJacob.npc",444,185,3,0);//Near Boat Guy Island 1
	 spawnNPC("Ghettopolis/ThugJustin.npc",444,186,3,0);//Near Boat Guy Island 1 
	}
if (getSaveEntry("MarsalatoTown")!=1)	
	{
	 spawnNPC("Ghettopolis/ThugJerry.npc",401,172,2,0);//Lab Door
	 spawnNPC("Ghettopolis/ThugJin.npc",407,175,0);//Sandcastle thug chasing kid
	 spawnNPC("Ghettopolis/RowdyKid.npc",408,175,0);//Running away from Jin
	 spawnNPC("Ghettopolis/ThugJoe.npc",418,182,0);//Looking for trash guy
	 spawnNPC("Ghettopolis/ThugJoJo.npc",416,169,1,0);//JuiceConvo
	 spawnNPC("Ghettopolis/ThugJil.npc",417,169,3,0);//JuiceConvo
	 spawnNPC("Ghettopolis/ThugJaq.npc",421,173,0,0);//JakeCommentary
	 spawnTrainer("Ghettopolis/ThugJudas.tnr",424,189,2,0);
	 spawnTrainer("Ghettopolis/ThugJeb.tnr",428,170,2,0);
	 spawnTrainer("Ghettopolis/ThugJustina.tnr",407,179,3,0);
	 if(getSaveEntry("DockThugsLeave")==1)
		{
		 spawnNPC("Ghettopolis/ThugJacky3.npc",397,185,2,0);
		 spawnNPC("Ghettopolis/ThugJustin2.npc",398,185,3,0);
		 spawnNPC("Ghettopolis/ThugJoe2.npc",396,185,1,0);
		}
	}
	
if (getSaveEntry("MarsalatoTown")==1 && getSaveEntry("ThugBossJordanBeat")!=1)	
	spawnTrainer("Ghettopolis/ThugJerry2.tnr",401,172,2,0);//Lab Door
	
if (getSaveEntry("ThugBossJordanBeat")==1 && getSaveEntry("JustBeatM")!=1 )
	{
	 spawnNPC("Ghettopolis/BeatThugMa.npc",403,178,0,0);
	 spawnNPC("Ghettopolis/BeatThugMa.npc",396,188,2,0);
	 spawnNPC("Ghettopolis/BeatThugMb.npc",400,179,2,0);
	 spawnNPC("Ghettopolis/BeatThugMb.npc",401,191,2,0);
	 spawnNPC("Ghettopolis/BeatThugFa.npc",401,181,2,0);
	 spawnNPC("Ghettopolis/BeatThugFa.npc",407,173,0,0);
	 spawnNPC("Ghettopolis/BeatThugMi.npc",402,183,0,0);
	 spawnNPC("Ghettopolis/BeatThugMb.npc",419,174,2,0);
	 spawnNPC("Ghettopolis/BeatThugFa.npc",421,184,0,0);
	 spawnNPC("Ghettopolis/Officerg.npc",412,169,0,0);
	 spawnNPC("Ghettopolis/Officerg.npc",402,180,2,0);
	 spawnNPC("Ghettopolis/Officeri.npc",399,173,1 ,0);
	 
	 spawnNPC("Ghettopolis/FoodLady2.npc",412,168,2,0);
	 spawnNPC("Ghettopolis/RowdyKid2.npc",408,175,1,0);
	}
	
if (getSaveEntry("ThugBossJordanBeat")==1 && getSaveEntry("Jake1Removed")!=1)
	spawnNPC("Ghettopolis/MarsalaVictory.npc",419,171,1,0);
	
	if (getSaveEntry("Jake1Removed")==1)
	{
	 setCollisions(420,171,2,1,1);
	 editTile(421,171,6,0);
	}

if (getSaveEntry("JustBeatM")==1)
{
 spawnNPC("Ghettopolis/GuyBlockingBoathouse.npc",435,180,2,0);
 spawnNPC("Ghettopolis/Chef.npc",426,173,1,0);
 spawnNPC("Ghettopolis/IslandInformer.npc",427,186,2,0);
 spawnNPC("Ghettopolis/RascalMom.npc",395,185,3,0);
 spawnNPC("Ghettopolis/RascalDad.npc",395,186,3,0);
 spawnNPC("Ghettopolis/RowdyKid3.npc",389,185,3,0);
 spawnNPC("Ghettopolis/BenchLady.npc",408,174,0,0);
 spawnNPC("Ghettopolis/ScienceKid.npc",398,173,1,0);
 spawnNPC("Ghettopolis/HappyKid.npc",401,189,2,0);
 spawnNPC("Ghettopolis/DomJuiceGirl.npc",403,177,0,0);
 spawnNPC("Ghettopolis/SwimmingGirl.npc",416,176,3,0);
 spawnNPC("Ghettopolis/FanGuy.npc",409,176,1,0);
 spawnNPC("Ghettopolis/LittleKid.npc",434,182,3,0);
 
 if(getSaveEntry("ShiftMissedTalked")!=1)
  spawnNPC("Ghettopolis/StorekeeperLate.npc",384,137,0,0);
  
 if(getSaveEntry("Banned4Life")==1)
  spawnNPC("Ghettopolis/StorekeeperLate2b.npc",384,137,0,0);
  
 if(getSaveEntry("FireLateEmployee")==1)
  spawnNPC("Ghettopolis/StorekeeperLate2a.npc",386,140,2,0); 
}