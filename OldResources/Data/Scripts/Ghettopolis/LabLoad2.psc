	 print("LabLoad2");
     playerToLocation(8,11,1);
	 print("BeforeSleeps");
		sleep(2000);
		messageBox("For the last time, GIVE ME THE RECIPE OLD MAN!");
		sleep(200);
		messageBox("Over my old body!");
		sleep(1000);
		moveTrainer("ThugBossJordan",2);
		messageBox("How'd you get in here?");
		print("BeforeChoicebox");
		string answer = choiceBox("Whatever. Just scram before you get hurt kid.","No","Okay");
		if (answer == "No")
		{
		 sleep(1000);
		 playerToLocation(10,9,1);
		 playerToLocation(10,8,1);
		 sleep(200);
		 messageBox("Okay kid but it's your funeral");
		 startTrainerBattle("Ghettopolis/ThugBossJordan.tnr");
		 int playerx = playerXWorldPos();
		 int playery = playerYWorldPos();
		}
		if (answer == "Okay")
		{
		 movePlayer(2,0,1);
		 movePlayer(2,0,1);
		 sleep(1000);
		 int playerx = playerXWorldPos();
		 int playery = playerYWorldPos();
		 trainerToLocation("ThugBossJordan",playerx/32,(playery/32)-1,1);
		 sleep(500);
		 moveTrainer("ThugBossJordan",2);
		 sleep(500);
		 movePlayer(0,0,1);
		 sleep(500); 
		 messageBox("I can't have any witnesses","Did you really think I'd let you leave?");
		 sleep(200);
		 startTrainerBattle("Ghettopolis/ThugBossJordan.tnr",1);
		}
		
		 addSaveEntry("ThugBossJordanBeat",1);
		 sleep(1000);
		 messageBox("You haven't heard the last of the Local Thugs!","I'm gonna get my cronies and we're all gonna come back in and beat you up.");
		 trainerToLocation("ThugBossJordan",9,14,1);
		 removeTrainer("ThugBossJordan");
		 sleep(2000);
		 npcToLocation("GProf",playerx/32,(playery/32)-1,1);
		 moveNPC("GProf",2);
		 sleep(200);
		 messageBox("Thanks a lot. I was in a real pickle there.", "With all that time I lost from that intended mugging, I don't have much time until my report is due.","What I'm trying to say is I don't want to stand here all day..","..monologuing..","..trying to answer every question you can think of..","..or every question you might think of..","..honestly I don't know what I'm saying anymore..","..so...yeah","..much too busy.");
		 sleep(2000);
		 messageBox("Oh here..");
		 giveItem(58);
		 messageBox("You were given a Speed Juice");
		 messageBox("Just a little something to say thank you for your help.","That's all.");
		 setPlayerLock(0);
		 print("Unlocked");