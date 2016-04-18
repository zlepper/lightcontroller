var SerialPort = require("serialport").SerialPort;
var port = new SerialPort("/dev/ttyAMA0", {baudRate: 9600});

function init() {
		port.on("open", function() {
				ready = true;
				if(initQueue.length) {
				write();
				}
				port.on("data", function(data) {
					console.log("Got data");
					console.log(data);
				});
				port.on("error", function(error) {
					console.log(error);
				});
		});	
}
init();
ready = true;
writing = false;
initQueue = [];
var queue = function(b) {
		initQueue.push(b);
		if(ready && !writing) {
				write();
		}
}

var write = function() {
		writing = true;
		b = initQueue.shift();
		console.log(b);
		port.write(b, function(err, bytesWritten) {
				if(err) {
					console.log(err);
				}
				port.drain(function(err) {
						//setTimeout(function() {
						console.log("Done writing");
						console.log(b);
						if(err) {
								console.log(err);
						}			
						if(initQueue.length) {
								write();
						} else {
								writing = false;
						}
						//}, 1);
				});
				console.log(bytesWritten);		
		});
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

