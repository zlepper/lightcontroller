var GPIO = require("onoff").Gpio;

// Contains the queue of characters to write
var queue = [];
var actionQueue = [];
var writerInteval = null;


// Dictionary that descripes what pin maps to what bit
var iomap = {
	0: 2,
	1: 3,
	2: 4,
	3: 14,
	4: 15,
	5: 18,
	6: 17,
	7: 27
}

function getWriteablePin(index) {
	return new GPIO(iomap[index], "in");
}

function writePin(index, value, cb) {
	var pin = getWriteablePin(index);
	pin.write(value, cb);
}

function setPorts(bits) {
	// Iterate over all the values
	for(var i = 0; i < bits.length; i++) {
		// Get the value
		var bit = bits[i];
		// Set the pin to the selected value
		writePin(i, bit);			
	}	
}

function intToBinary(g) {
	// Avoid writing numbers bigger than 8 bit
	if(g > 255) {
		g = 255;
	}
	// Convert the number into a binary list
	var temp = g.toString(2);
	var bin = [];
	for(var i = 0; i < temp.length; i++) {
		bin.push(Number(temp[i]));		
	}
	while(bin.length < 8) {
		bin.unshift(0);	
	}
	return bin;
}

function write() {
	var character = queue.shift();
	var bin = intToBinary(character);		
	setPorts(bin);
	// If there is no more in the queue, then just stop sending stuff
	if(queue.length === 0) {
		clearInterval(writerInterval);
	}	
}

function startWriting() {
	// Only create a new interval if it doesn't already exist
	if(!writerInterval && queue.length) {
		// Create a new interval to write data
		writerInterval = setInterval(write, 1);
	}
}

function queueString(s) {
	// push the characters into the queue
	for(var i = 0; i < s.length; i++) {
		queue.push(s.charCodeAt(i));
	}
	// Start the queue if it is not started already
	startWriting();
}

function queueNumber(number) {
	// If the number is too big, don't do anything
	if(number > 254) {
		return;
	}
	queue.push(number);
}

exports.queueString = queueString;
exports.queueNumber = queueNumber;
