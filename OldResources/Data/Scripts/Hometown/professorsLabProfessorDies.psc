//----------------------------------------Professor talks
	sleep(1200);
   	moveNPC("professor3",2);
	sleep(200);
	messageBox("You're back already?");
	sleep(600);
	moveNPC("professor3",1);
	sleep(800);
	moveNPC("professor3",3);
	sleep(800);
	moveNPC("professor3",2);
	sleep(600);
	messageBox("I don't see Aster.");
	sleep(150);
	string ProfResp1 = choiceBox("Where is he?","Coming now","No idea","About 3 o'Clock") ; 	//Fails here
	
	if (ProfResp1 == "Coming now")
		{
		 sleep(150);
		 messageBox("Oh good.");
		}
	
	if (ProfResp1 == "No idea")
		{
		 sleep(150);
		 messageBox("Oh well. I'm sure he'll turn up soon", "I'll see him then.");
		}
	
	if (ProfResp1 == "About 3 o'Clock")
		{
		 sleep(150);
		 messageBox("What? Were you...", "Well.. nevermind.");
		} 
	 
	sleep(800);
	messageBox("...", "I actually have another favor to ask of you.");
	sleep(200);
	npcToLocation("professor3",3,4,1);
	sleep(1500);
	moveNPC("professor3",2);
	messageBox("Well..", "Come on over.");
	moveNPC("professor3",3);
	setPlayerLock(0);
	playerToLocation(3,5,1);
	setPlayerLock(1);
	sleep(500);
	moveNPC("professor3",2);
	messageBox("I want you to take the other two peoplemon from the counter over here and take care of them for me.", "They deserve to be let out every once in a while and I never seem to have the time.","Just don't forget to feed them!","Speaking of which, I'm going to make myself a sandwich.");
	sleep(200);
	npcToLocation("professor3",17,4,1);
	moveNPC("professor3",0);
	sleep(1000);
	 moveNPC("professor3",3);
	string ProfResp2 =choiceBox("Do you want anything?","Yes","What do you have?","No Thanks");
	 
	if (ProfResp2 == "Yes")
		{
		 sleep(150);
		 messageBox("Okay, I'll see what I have.");.
		}
	
	if (ProfResp2 == "What do you have?")
		{
		 sleep(150);
		 messageBox("Actually I'm not sure..", "I'll check.");
		}
	
	if (ProfResp2 == "No Thanks")
		{
		 sleep(150);
		 messageBox("Okay. More for me then.");
		}
	 
	 sleep(500);
	 moveNPC("professor3",0);
	 messageBox("I'm opening the door now...");

//--------------------------------------Explosion!

	drawAnim("explosion.anim",545,64,0);
	sleep(500);
	editTile(16,1,2,0);
	removeNPC("professor3");
	
//------------------------------Thug Appearance!!
	sleep(4000);
	spawnNPC("Hometown/profLabThug1.npc",10,13,1);
	sleep(900);
	npcToLocation("profLabThug1",11,13,1);
	sleep(1500);
	spawnNPC("Hometown/profLabThug2.npc",10,13,1);
	sleep(900);
	moveNPC("profLabThug1",3);
	messageBox("I guess it worked.");
	sleep(1000);
	messageBox("It worked?!","The stars are in perfect alignment for the Local Thugs!");
	sleep(600);
	messageBox("The stars? I thought it was the fridge bomb we placed that killed him.");
	sleep(600);
	messageBox("It was...","You know sometimes I wonder about you.","Just please watch the door. I'm going to get these rare peoplemon and then we can high-tail it out of here.");
	npcToLocation("profLabThug1",9,13,1);
	moveNPC("profLabThug1",2);
	sleep(200);
//---------------------------Thugs Take Peopleballs

	npcToLocation("profLabThug2",2,5,1);
	sleep(500);
	moveNPC("profLabThug2",3);
	sleep(500);
	editTile(1,6,3,0);
	editTile(1,5,3,0);
	editTile(1,4,3,0);

//-----------------------------------------Thugs Bounce!
	npcToLocation("profLabThug2",9,12,1);
	print("walks");
	moveNPC("profLabThug2",2);
	sleep(200);
	messageBox("Hey");
	sleep(500);
	moveNPC("profLabThug1",0);
	messageBox("I got them. Let's go meet up with the others.");
	sleep(500);
	removeNPC("profLabThug1");
	npcToLocation("profLabThug2",10,13,1);
	sleep(200);
	removeNPC("profLabThug2");
	setPlayerLock(0);
	addSaveEntry("professorDead",1);
