angular.module("lightcontroller", ["ngMaterial", "ui.router", "ngAnimate", "ngMessages"]).controller("MainController", ["LightController", function(lightController) {
	this.controllers = [];
	this.toggle = function(controller) {
		resProm = lightController.toggle(controller.id);
		controller.awaitingResponse = true;
		controller.state = !controller.state;
		resProm.then(function() {
			console.log("Done toggling " + controller.id);
			controller.awaitingResponse = false;
		});
	}
	this.on = function(controller) {
		resProm = lightController.on(controller.id);
		controller.awaitingResponse = true;
		controller.state = true;
		resProm.then(function() {
			controller.awaitingResponse = false;
			console.log("Done turning on " + controller.id); 
		});
	}
	this.off = function(controller) {
		resProm = lightController.off(controller.id);
		controller.awaitingResponse = true;
		controller.state = false;
		resProm.then(function() {
			controller.awaitingResponse = false;
			console.log("Done turning off " + controller.id);	
		});
	}
	this.setState = function(controller) {
		if(controller.state) {
			this.off(controller);
		} else {
			this.on(controller);
		}
		controller.state = !controller.state;
	}
	this.setPower = function(controller) {
		resProm = lightController.setPower(controller.id, controller.power);
		controller.awaitingResponse = true;
		resProm.then(function() {
			controller.awaitingResponse = false;
			console.log("Done setting power of controller " + controller.id + " to power level " + controller.power);
		});
	}

	this.getControllers = function() {
		resProm = lightController.getSwitches();
		resProm.then(function(result) {
			console.log("got \n" + result);  
		});
	}
	var fakeId = 1;
	this.createFakeController = function() {
		var c = {
			id: fakeId++,
			state: false,
			power: 255,
			name: "test" + fakeId,
			awaitingResponse: false
		}
		this.controllers.push(c);
	}
}]).directive('dragEnd', function() {
	return {
		restrict: 'A',
		scope: false,
		link: function(scope, element, attrs) {
			function end() {
				scope.$apply(attrs.dragEnd);
			}
			element.on('$md.dragend', end)
			.on("click", end);
		}
	}
});
