var portio = require("./portio");
var Controller = require("./controller");
var fs = require("fs");
// Create a list of all the controllers our system knows about
var controllers = [];

// Adds a new controller to the list of controllers
function addController() {
		var controller = new Controller();
		controllers.push(controller);
		return controller;
}

// Export the add controller function
exports.addController = addController;

// Fetches a controller with a specific id from the list of controllers
function getController(id) {
		// Iterate over all the controllers
		for(var i = 0; i < controllers.length; i++) {
				// Fetch the controller at the current index
				var c = controllers[i];
				// Check if the id matches
				if(c.id == id) {
						// If the id matches, we should just return the controller to whoever wants it. 
						return c;
				}
		}
		return null;
}

// Fetches a controller with a specific id
function get(id) {
		// Call the function above		
		return getController(id);
}

// Export the get function
exports.get = get;

// Turns on a controller
function on(id) {
		// Fetches the specified controller from the list
		var controller = getController(id);
		// Turn on the controller
		controller.turnOn();	
}

// Export the on function
exports.on = on;

function off(id) {
		var controller = getController(id);
		controller.turnOff();
}

exports.off = off;

function toggle(id) {
		var controller = getController(id);
		controller.toggle();
}

exports.toggle = toggle;

function power(id, power) {
		var controlller = getController(id);
		controller.power(power);
}

exports.power = power;

// Sends a specific number to the specified controller
function sendNumber(controller, number) {
		// Queue our start byte
		portio.queueNumber(255);
		// Queue the controller number
		portio.queueNumber(controller);
		// Avoid issues if the controller has the same id as the wanted power level
		portio.queueNumber(number);
		// Queue our end bit.
		portio.queueNumber(0);
}

var assignController = function(callback) {

		var controller = addController();
		sendNumber(0, controller.id);
		callback(controller);
}

portio.assignControllerIdAssignmentFunction(assignController);
// Export the send number function
exports.sendNumber = sendNumber;
// Export a get all function that returns a list of all the controllers in the system and their current settings
exports.getAll = function() {
		return controllers;
}

function setLight(controllerId, level) {
	var controller = getController(controllerId);
	if(controller) {
		controller.level = level;
	}
}
exports.setLight = setLight

function resync() {
	assignController(function(controller) {
		
	});	
}


function saveControllers() {
	var cs = JSON.stringify(controllers, null, "  ");
	fs.writeFileSync("controllers.json", cs, "utf8");
	console.log("Controllers written to file");
}

function loadControllers() {
	var cs = fs.readFileSync("controllers.json", "utf8");
	var cse = JSON.parse(cs);
	var biggestId = -1;
	for(var i = 0; i < cse.length; i++) {
		var c = cse[i];
		var ctrl = addController();
		ctrl.id = c.id;
		ctrl.level = c.level;
		ctrl.name = c.name;
		ctrl.state = c.state;
		if(c.id > biggestId) {
			biggestId = c.id;
		}
	}
	Controller.nextId = biggestId + 1;
	console.log("Controllers loaded");
}

process.on('exit', function () {
	saveControllers();	
});
