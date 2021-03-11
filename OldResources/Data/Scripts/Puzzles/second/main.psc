int main() {
	int px = playerXWorldPos()/32;
	int py = playerYWorldPos()/32 - 1;

	sleep(1200);
	movePlayer(1, 0, 1);

	if (getSaveEntry("puzzles:secondDone") == 1) {
		messageBox("What are you doing back here kid?", "We're waiting for our next potential victim, get lost");
		return 0;
	}
	else {
		messageBox("Hello, my name is Juan", "This puzzle requires that you stand very still");
		messageBox("Don't worry about the temptation of moving, I've already frozen your legs in place");
		messageBox("You will be asked a series of questions by my associates here");
		messageBox("Answer them all correctly and you will gifted enough strength to lift exactly one dumbbell. No more no less");
		messageBox("Answer wrong... and POW! You're gone");
		messageBox("Don't worry though, you can come back and try again");
		messageBox("Now get ready. Question 1 Asker, approach!");
		sleep(600);
		
		int nx = npcXPos("Q1");
		int ny = npcYPos("Q1");
		npcToLocation("Q1", px, py, 1);
		sleep(600);
		moveNPC("Q1", 2);
		movePlayer(0, 0, 1);
		
		messageBox("Question 1");
		if (choiceBox("What town do you live in?", "Littleroot", "HomeTown", "AwayTown", "Ghettopolis", "Iceburg") != "HomeTown") {
			messageBox("FALSE! You're done for kid");
			return 0;
		}
		messageBox("Correct. Next question!");
		npcToLocation("Q1", nx, ny, 1);
		sleep(200);
		
		
		nx = npcXPos("Q2");
		ny = npcYPos("Q2");
		npcToLocation("Q2", px, py, 1);
		sleep(600);
		moveNPC("Q2", 2);
		
		messageBox("Question 2");
		if (choiceBox("What are you trying to get from this room?", "Wealth", "Health Insurance", "Strength to lift a dumbbell", "Snacks") != "Strength to lift a dumbbell") {
			messageBox("FALSE! You're done for kid");
			return 0;
		}
		messageBox("Correct. Next question!");
		npcToLocation("Q2", nx, ny, 1);
		sleep(200);
		
		
		nx = npcXPos("Q3");
		ny = npcYPos("Q3");
		npcToLocation("Q3", px, py, 1);
		sleep(600);
		moveNPC("Q3", 2);
		
		messageBox("Question 3");
		if (choiceBox("Which of these Peopleballs is best?", "Terriball", "Great Peopleball", "Pair of Peopleballs", "Master Peopleball", "Gulliball") != "Master Peopleball") {
			messageBox("FALSE! You're done for kid");
			return 0;
		}
		messageBox("Correct. Next question!");
		npcToLocation("Q3", nx, ny, 1);
		sleep(200);
		
		
		nx = npcXPos("Q4");
		ny = npcYPos("Q4");
		npcToLocation("Q4", px, py, 1);
		sleep(600);
		moveNPC("Q4", 2);
		
		messageBox("Question 4");
		if (choiceBox("There is a city that is all ghetto. What is it called?", "OrangehairVille", "HomeTown", "Ghettopolis", "Iceburg", "Aridity") != "Ghettopolis") {
			messageBox("FALSE! You're done for kid");
			return 0;
		}
		messageBox("Correct. Next question!");
		npcToLocation("Q4", nx, ny, 1);
		sleep(200);
		
		
		nx = npcXPos("Q5");
		ny = npcYPos("Q5");
		npcToLocation("Q5", px, py, 1);
		sleep(600);
		moveNPC("Q5", 2);
		
		messageBox("Question 5");
		if (choiceBox("Will you hold my Peopleballs?", "Yes", "No") != "Yes") {
			messageBox("Well then get out!");
			return 0;
		}
		giveItem(9, "Recieved a Pair of Peopleballs... They are uncomfortably warm");
		npcToLocation("Q5", nx, ny, 1);
		sleep(200);
		
		
		nx = npcXPos("Q6");
		ny = npcYPos("Q6");
		npcToLocation("Q6", px, py, 1);
		sleep(600);
		moveNPC("Q6", 2);
		
		messageBox("Question 6");
		if (choiceBox("70% of the time it works ________", "Everytime", "70% of the time", "Never", "Enough of you old ladies") != "Everytime") {
			messageBox("FALSE! You're done for kid");
			return 0;
		}
		messageBox("Correct. Next question!");
		npcToLocation("Q6", nx, ny, 1);
		sleep(200);
		
		
		nx = npcXPos("Q7");
		ny = npcYPos("Q7");
		npcToLocation("Q7", px, py, 1);
		sleep(600);
		
		messageBox("Question 7");
		choiceBox("Have you seen my glasses", "Yes but I'm not helping you", "...", "No");
		messageBox("WHAT? I CAN'T HEAR YOU I'M HARD OF HEARING", "HAVE A NICE DAY YOUNG WHIPPERSNAPPER");
		npcToLocation("Q7", nx, ny, 1);
		sleep(200);
		
		
		nx = npcXPos("Q8");
		ny = npcYPos("Q8");
		npcToLocation("Q8", px, py, 1);
		sleep(600);
		moveNPC("Q8", 2);
		
		messageBox("Question 8");
		if (choiceBox("If it's a Tuesday in January of next year which of my medications am I supposed to take?", "All of it", "The ones in the Tuesday box", "Please let me leave") != "The ones in the Tuesday box") {
			messageBox("That doesn't sound right, you better leave");
			return 0;
		}
		messageBox("Ok I believe that", "You came here to get the strength to lift one dumbbell");
		messageBox("Well stop being a baby and just move it");
		messageBox("Seriously, that thing out there weighs like 8 pounds");
		sleep(700);
		messageBox(playerName()+" gained the power of Confidence!");
		npcToLocation("Q8", nx, ny, 1);
		sleep(200);
		
		addSaveEntry("puzzles:secondDone", 1);
	}
	
	return 0;
}

main();
setPlayerLock(0);
loadMap("Puzzles/mainRoom", 3);