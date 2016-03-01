angular.module("lightcontroller", ["ngMaterial", "ui.router", "ngAnimate", "ngMessages"]).controller("TestController", ["LightController", function(lightController) {
	var controller = this;
	this.lightController = lightController;
	this.hey = function() {
		console.log("HEllo sir!");
	}
	console.log("TestController loaded!");
	this.toggle = function() {
		var togglePromise = lightController.toggle(0);
		togglePromise.then(function(response) {
			controller.state = response.data.value;
		});
	};
	this.state = false;
}]);
