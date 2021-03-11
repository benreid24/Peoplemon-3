sleep(300);
int RockRoll = 2700; //random(1,9999);
print(RockRoll);
print("# made");

if (RockRoll<= 250) {
messageBox("You found a Fancy Fossil.");
giveItem(103);
print("1");
}

elif (RockRoll <=500) {
messageBox("You found a Party Fossil.");
giveItem(104);
print("2");
}

elif (RockRoll <=600) {
messageBox("You found a Friendship Stone.");
giveItem(28);
print("3");
}

elif (RockRoll <=700) {
messageBox("You found a Supah Cool Stone.");
giveItem(29);
print("4");
}

elif (RockRoll <=800) {
messageBox("You found an Oo you Fancy Stone.");
giveItem(30);
print("5");
}

elif (RockRoll <=900) {
messageBox("You found a Supah Funny Stone.");
giveItem(31);
print("6");
}

elif (RockRoll <=1000) {
messageBox("You found a Party Stone.");
giveItem(32);
print("7");
}

elif (RockRoll <=1100) {
messageBox("You found a PP Raiser.","..ew....");
giveItem(38);
print("8");
}

elif (RockRoll <=1200) {
messageBox("You found Peoplemon Parts.","You should probably call the cops or something..");
giveItem(135);
print("9");
}

elif (RockRoll <=3000) {
startBattle("HorseshoeCave/Default.ppl","Battle.plst","battleBgnd2.png",0,1);
print("10");
}

else {
messageBox("You find nothing. It's just a rock.");
print("11");
}
print("Done");
setPlayerLock(0);
