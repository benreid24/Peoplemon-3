if (hasItem(137) && playerDir() == 2) {
	int r = random(0,1000);
	if (r < 200) {
		messageBox("A Peoplemon noticed you had an Old Fishing Rod and threw itself at you!");
	}
	elif (r < 400) {
		messageBox("You can see all kinds of Peoplemon in the water but they seem to be uninterested in your rod");
		messageBox("Maybe it's not big enough?");
	}
	elif (r < 600) {
		messageBox("Nothing is biting. There must be something wrong with your stance");
	}
	elif (r < 800) {
		messageBox("You feel something tugging at you!");
		messageBox("It's not a Peoplemon...");
		messageBox("Turns out it's your constant sense of worthlessness. You should have done the dishes before you left home");
	}
	else {
		messageBox("Something's on the line!");
		messageBox("Hey wait a minute");
		giveItem(44, playerName() + " got a log!");
	}
}
sleep(300);
setPlayerLock(0);