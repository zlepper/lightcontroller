var SerialPort = require("serialport").SerialPort;
var port = new SerialPort("/dev/ttyAMA0", {baudrate: 9600});

function init() {
	port.on("open", function() {
		queue = function(b) {
			port.write(b);
		}
		while(initQueue.length) {
			queue(initQueue.shift);
		}
	});	
}
init();

initQueue = [];
var queue = function(b) {
	initQueue.push(b);
}

function queueString(s) {
		// push the characters into the queue
		for(var i = 0; i < s.length; i++) {
				queue(s.charCodeAt(i));
		}
}

function queueNumber(number) {
		// If the number is too big, don't do anything
		if(number > 255) {
				return;
		}
		queue(new Buffer([number]));
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

