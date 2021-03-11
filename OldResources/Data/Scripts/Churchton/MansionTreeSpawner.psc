int showTrees() {
	editTile(252,329,5,1);
	editTile(252,331,5,1);
	editTile(255,329,5,1);
	editTile(255,331,5,1);
	editTile(258,330,5,1);
	editTile(261,330,5,33);
	editTile(261,328,5,33);
	editTile(263,328,5,33);
	editTile(261,332,5,33);
	editTile(263,330,5,33);
	setCollisions(253,330,1,4,0);
	setCollisions(264,329,1,5,0);
	return 0;
}

int hideTrees() {
	editTile(252,329,5,0);
	editTile(252,331,5,0);
	editTile(255,329,5,0);
	editTile(255,331,5,0);
	editTile(258,330,5,0);
	editTile(261,330,5,0);
	editTile(261,328,5,0);
	editTile(263,328,5,0);
	editTile(261,332,5,0);
	editTile(263,330,5,0);
	setCollisions(253,330,1,4,1);
	setCollisions(264,329,1,5,1);
	
	return 0;
}

int flashOff() {
	int delay = 20;
	
	while (delay<850) {
		showTrees();
		sleep(delay/2);
		hideTrees();
		sleep(delay/2);
		delay = delay * 1.25;
	}
	hideTrees();
	
	return 0;
}

int flashOn() {
	int delay = 850;
	
	while (delay>20) {
		showTrees();
		sleep(delay/2);
		hideTrees();
		sleep(delay/2);
		delay = delay * 0.75;
	}
	showTrees();
	
	return 0;
}

if (getDaytime() < 1380 && getDaytime() > 240 ) {  //Between 4am and 11pm
	showTrees();
	sleepToTime(23,0);
}
	
	
while(1) {
	flashOff();
	sleepToTime(4,0);

	flashOn();
	sleepToTime(23,0);
}