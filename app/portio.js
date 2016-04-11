var GPIO = require("onoff").Gpio;

// Contains the queue of characters to write
var queue = [];
var actionQueue = [];
var writerInterval = null;


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
	return new GPIO(iomap[index], "out");
}

function writePin(index, value) {
	var pin = getWriteablePin(index);
	pin.write(value, function(err) {
		if(err) return console.log(err);	 
	});
}

function setPorts(bits) {
	console.log(bits);
	// Iterate over all the values
	for(var i = 0; i < 8; i++) {
		// Get the value
		var bit = bits[i];
		// Set the pin to the selected value
		writePin(i, bit);			
	}	
}

function byteString(n) {
	if (n < 0 || n > 255 || n % 1 !== 0) {
		throw new Error(n + " does not fit in a byte");
	}
	return ("000000000" + n.toString(2)).substr(-8)
}
function intToBinary(g) {
	bin = [];
	bits = byteString(g);
	for(var i = 0; i < 8; i++) {
		bin.push(Number(bits[i]));
	}
	return bin;
}

function write() {
	if(queue.length) {
		var character = queue.shift();
		var bin = intToBinary(character);
		setPorts(bin);
	}
	return;
}


// Create a new interval to write data
writerInterval = setInterval(function() {
	//console.log("TEST");
	write();
}, 1000);

function queueString(s) {
	// push the characters into the queue
	for(var i = 0; i < s.length; i++) {
		queue.push(s.charCodeAt(i));
	}
}

function queueNumber(number) {
	// If the number is too big, don't do anything
	if(number > 255) {
		return;
	}
	queue.push(number);
}

exports.queueString = queueString;
exports.queueNumber = queueNumber;
