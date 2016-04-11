var portio = require("./portio");
var Controller = require("./controller");

var controllers = [];

function addController() {
	var controller = new Controller();
	controllers.push(controller);
}

exports.addController = addController;

function getController(id) {
	for(var i = 0; i < controllers.length; i++) {
		var c = controllers[i];
		if(c.id == id) {
			return c;
		}
	}
	return null;
}

function get(id) {
	return getController(id);
}

exports.get = get;

function on(id) {
	var controller = getController(id);
	controller.turnOn();	
}

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

function sendNumber(controller, number) {
	portio.queueNumber(255);
	portio.queueNumber(controller);
	portio.queueNumber(number);
	portio.queueNumber(0);
}

exports.sendNumber = sendNumber;
exports.getAll = function() {
	return controllers;
}
