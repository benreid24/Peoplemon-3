//remove dumbell npc and tile
int x = npcXPos("Strong1 Blocker");
int y = npcYPos("Strong1 Blocker");
removeNPC("Strong1 Blocker");
addSaveEntry("puzzles:strong1Opened", 1);
editTile(x, y, 2, 0);