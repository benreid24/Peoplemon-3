if (getSaveEntry("professorDead")==1 && getSaveEntry("boatGuy1")!=1)
	{
	 if (controlPressed(5) == 1 || controlPressed(2) == 1)
		{
		 messageBox("A voice echoed inside your head:","You don't have time!");
		 print("TownBlockingScriptRuns");
		 sleep(100);
		}
	}