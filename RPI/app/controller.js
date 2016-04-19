var controllerAction = require("./controller-action");
var portio = require("./portio");


function Controller() {
	// Create an id for the controller and increment the id counter
	this.id = Controller.nextId++;
	this.state = false;
	this.power = 255;
	this.name = "Controller " + this.id;
	this.awaitingResponse = false;
}

Controller.nextId = 1;

Controller.prototype.setState = function() {
	controllerAction.sendNumber(this.id, this.state ? this.power : 0);
}

Controller.prototype.turnOn = function() {
	this.state = true;
	this.setState();
}

Controller.prototype.turnOff = function() {
	this.state = false;
	this.setState();
}

Controller.prototype.toggle = function() {
	this.state = !this.state;
	this.setState();
}

Controller.prototype.setPower = function(power) {
	this.power = power;
	this.setState();
}

module.exports = Controller;


