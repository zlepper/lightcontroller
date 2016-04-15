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

var values = [];
var pins = [];


function binArrayToNumber(arr) {
		var s = arr.reverse().join('');
		return parseInt(s, 2);
}

function getBinaryValue() {
		var binArr = [];
		for(var i = 0; i < 8; i++) {
				var b = pins[i].readSync();
				binArr.push(b);		
		}
		return binArr;
}

function read() {
		var binArr = getBinaryValue();	
		var n = binArrayToNumber(binArr);
		// Do not add the same value to the array
		if(values.length > 0 && n == values[values.length-1]) {
				return;
		}
		values.push(n);
}

function init() {
		for(var i = 0; i < 8; i++) {
				var pin = new GPIO(iomap[i], "in", "rising");
				pins.push(pin);
				pin.watch(function(err, value) {
						read();			
				});
		}
}

init();
function getWriteablePin(i) {
		return pins[i];
}

function setPorts(bin) {
		console.log(bin);
		for(var i = 0; i < bin.length; i++) {
				var bit = bin[i];
				var pin = getWriteablePin(i);
				pin.setEdge("none");
				pin.setDirection("out");
				pin.writeSync(bit);
				pin.setDirection("in");
				pin.setEdge("rising");
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

// Export the functions we can use the interact with the GPIO pins
exports.queueString = queueString;
exports.queueNumber = queueNumber;
exports.getNumber = function() {
		if(values.length > 0) {
				return values.shift();	
		} else {
				return 0;
		}	
};
process.on('SIGINT', function() {
	for(var i = 0; i < 8; i++) {
		var pin = pins[i];
		pin.unexport();
	}			
});
