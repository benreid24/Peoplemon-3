//check x saves and remove tiles/collisions as needed

int t = 1;
int allDone = 1;

while (t<6) {
	string entry = "puzzles:first:x-" + t;
	string data = getSaveEntry(entry);
	if (data+"" != "pressed") {
		allDone = 0;
	}
	else {
		editTile(28+t-1, 2, 2, 0);
		editTile(28+t-1, 3, 2, 0);
	}
	t = t + 1;
}

if (allDone) {
	int x = 0;
	while (x<5) {
		editTile(28+x, 2, 1, 0);
		editTile(28+x, 3, 1, 0);
		x = x + 1;
	}
	setCollisions(28,2,5,2,1);
	//play sound?
}