var SerialPort = require("serialport").SerialPort;
var port = new SerialPort("/dev/ttyAMA0", {baudRate: 9600});
var controllerAction = require("./controller-action");

var assingControllerId;
function assigncontrolleridassignmentfunction(f) {
	assingControllerId = f;
}
exports.assignControllerIdAssignmentFunction = assigncontrolleridassignmentfunction;

function action() {
		// We should have 4 bytes in the queue before we can do anything
	if(dataQueue.length >= 4) {
			// Get the first byte
		var firstE = dataQueue.shift();
		// The first byte should always be a full byte b'11111111'
		if(firstE == 255) {
				// Get the next byte
			var secondE = dataQueue.shift();
			console.log("SecondE ");
			console.log(secondE);
				var thirdE = dataQueue.shift();
				var fourthE = dataQueue.shift();
			// If the byte is 0, then the controller doesn't have a number and would like to get one assinget
			if(secondE == 0) {
//					// Get the 3. and 4. byte. 
					// These should also be 0, because the controller doesn't have anything to tell us, we just need to keep the protocol correct. 
				if(thirdE == 0 && fourthE == 0) {
						// assing an id to the controller and save it to our database
					//assingControllerId(function(c) {
							// Some debug so we know the controller has been assigned. 
					//	console.log("Assigned controller:")
					//	console.log(c);
					//});	
				}
			} else {
				// secondE is the id of a controller we sould have in our system. 	
				if(fourthE == 0) {
					controllerAction.setLight(secondE, thirdE);					
				}
			}
		}
	}	
	if(dataQueue.length >= 4) {
		action();
	}
}

function init() {
		port.on("open", function() {
				ready = true;
				if(initQueue.length) {
						write();
				}
				port.on("data", function(data) {
						console.log("Got data");
						console.log(data);
						for(var i = 0; i < data.length; i++) {
								var d = data.readUIntBE(i, 1);
								dataQueue.push(d);
						}
						action();
				});
				port.on("error", function(error) {
						console.log(error);
				});
		});	
}
init();
var ready = true;
var writing = false;
var initQueue = [];
var dataQueue = [];
var queue = function(b) {
		initQueue.push(b);
		if(ready && !writing) {
				write();
		}
}

var write = function() {
		writing = true;
		b = initQueue.shift();
		//console.log(b);
		port.write(b, function(err, bytesWritten) {
				if(err) {
						console.log(err);
				}
				port.drain(function(err) {
						//setTimeout(function() {
				//		console.log("Done writing");
				//		console.log(b);
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
				//console.log(bytesWritten);		
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

