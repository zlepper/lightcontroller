var portio = require("./portio");
var Controller = require("./controller");

// Create a list of all the controllers our system knows about
var controllers = [];

// Adds a new controller to the list of controllers
function addController() {
		var controller = new Controller();
		controllers.push(controller);
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
		// Make sure the controller number doens't become too big. If it does it clashes with some other code on the microcontroller
		if(controller === 255) {
				return;
		}
		// Queue our start byte
		portio.queueNumber(255);
		// Queue the controller number
		portio.queueNumber(controller);
		// Avoid issues if the controller has the same id as the wanted power level
		if(controller == number) {
				portio.queueNumber(number + 1);
		} else {
				portio.queueNumber(number);
		}
		// Queue our end bit.
		portio.queueNumber(0);
}

// Export the send number function
exports.sendNumber = sendNumber;
// Export a get all function that returns a list of all the controllers in the system and their current settings
exports.getAll = function() {
		return controllers;
}
