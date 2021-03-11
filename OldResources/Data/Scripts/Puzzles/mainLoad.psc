//check saved strings and remove npcs/tiles
if (getSaveEntry("puzzles:educationOpened") == 1)
	runScript("Puzzles/removeEducation.psc", 0);
	
print(getSaveEntry("puzzles:secondDone"));
	
if (getSaveEntry("puzzles:strong1Opened") == 1)
	runScript("Puzzles/removeStrong1.psc", 0);