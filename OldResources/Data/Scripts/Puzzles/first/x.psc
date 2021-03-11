//get player position
//check save
//create save
//remove collisions/tiles

/*
 * First X:  (42,47)
 * Second X: (46,5)
 * Third X:  (25,47)
 * Fourth X: (19,47)
 * Fifth X:  (38,15)
 *
 * Rocks: (28,2) -> (32,3)
*/

int x = playerXWorldPos()/32;
int y = playerYWorldPos()/32;
int xnum = 1;

if (abs(x-42)<3 && abs(y-47)<3) {
	xnum = 1;
}
if (abs(x-46)<3 && abs(y-5)<3) {
	xnum = 2;
}
if (abs(x-25)<3 && abs(y-47)<3) {
	xnum = 3;
}
if (abs(x-19)<3 && abs(y-47)<3) {
	xnum = 4;
}
if (abs(x-38)<3 && abs(y-15)<3) {
	xnum = 5;
}

string xsave = "puzzles:first:x-"+xnum;

if (getSaveEntry(xsave)+"" != "pressed") {
	addSaveEntry(xsave, "pressed");
	editTile(28+xnum-1, 2, 2, 0);
	editTile(28+xnum-1, 3, 2, 0);
	//play sound?
}

int t = 1;
int allDone = 1;

while (t<6) {
	string entry = "puzzles:first:x-" + t;
	string data = getSaveEntry(entry);
	if (data+"" != "pressed") {
		allDone = 0;
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
	