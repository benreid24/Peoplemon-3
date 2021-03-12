string padZeros(int time) {
	string padded = intToString(time);
	if (time<10)
		padded = "0" + padded;
	return padded;
}

string minutes = padZeros(getTimeMinutes());
string hours = padZeros(getTimeHours());
setPlayerLock(1);
if (getTimeHours()>10 || getTimeHours()<8)
	messageBox("The church has a daily service from 8:00 until 11:00.", "It's currently " + hours + ":" + minutes);
else
	messageBox("Welcome to our Morning service", "It's currently " + hours + ":" + minutes);
setPlayerLock(0);