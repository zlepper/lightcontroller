var SerialPort = require("serialport").SerialPort
var serialPort = new SerialPort("/dev/ttyAMA0", {
	  baudrate: 9200
}, false);
var GPIO = require("onoff").Gpio;
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

var controllers = {
	0: 1
}

var lightStates = {
	0: false
}

function doStuff() {
	console.log("Port 7 values changed. This means that microcontroller would like to tell us something. So we should probably listen to that");
}

var inputWatch = new GPIO(iomap[7], "in");
inputWatch.watch(function(err, value) {
	if(value) {
		doStuff();
	}
});

function set(controller, value, cb) {
	var ctrl = new GPIO(iomap[0], "out");
	ctrl.write(value, function(err) {
		cb(err, value);
	});	
}

serialPort.open(function(error) {
	if(error) {
		return console.log(error);
	}
	console.log("Serial port open");
	serialPort.on("data", function(data) {
		console.log("Data recieved: " + data);
	});
	serialPort.write(255, function(err, result) {
		console.log("err " + err);
		console.log("results " + result);
	});
});

exports.set = set;
	
exports.toggle = function(controller, cb) {
	var newState = lightStates[controller];
	set(controller, newState ? 1 : 0, cb);
	lightStates[controller] = !newState;
}
