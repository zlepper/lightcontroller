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

function doStuff() {
	console.log("Port 7 values changed. This means that microcontroller would like to tell us something. So we should probably listen to that");
}

var inputWatch = new GPIO(iomap[7], "in");
inputWatch.watch(function(err, value) {
	if(value) {
		doStuff();
	}
});

exports.set = function(controller, value, cb) {
	var ctrl = new GPIO(iomap[0], "out");
	ctrl.write(value, function(err) {
		cb(err, value);
	});	
}
