angular.module("lightcontroller")
    .controller("MainController", ["LightController", "$mdDialog", "$mdMedia", function (lightController, $mdDialog, $mdMedia) {
			Number.prototype.map = function (in_min, in_max, out_min, out_max) {
					  return (this - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
			}
			this.minPower = 5;
			this.maxPower = 13;
        var cont = this;
		Number.prototype.powerMap = function() {
			return Math.round(this.map(cont.minPower, cont.maxPower, 0, 255));
		}
        var customFullscreen = $mdMedia('xs') || $mdMedia('sm');
        this.controllers = [];
        this.toggle = function (controller) {
            resProm = lightController.toggle(controller.id);
            controller.awaitingResponse = true;
            controller.state = !controller.state;
            resProm.then(function () {
                console.log("Done toggling " + controller.id);
                controller.awaitingResponse = false;
            });
        }
        this.on = function (controller) {
            resProm = lightController.on(controller.id);
            controller.awaitingResponse = true;
            controller.state = true;
            resProm.then(function () {
                controller.awaitingResponse = false;
                console.log("Done turning on " + controller.id);
            });
        }
        this.off = function (controller) {
            resProm = lightController.off(controller.id);
            controller.awaitingResponse = true;
            controller.state = false;
            resProm.then(function () {
                controller.awaitingResponse = false;
                console.log("Done turning off " + controller.id);
            });
        }
        this.setState = function (controller) {
            if (controller.state) {
                this.off(controller);
            } else {
                this.on(controller);
            }
            controller.state = !controller.state;
        }
        this.setPower = function (controller, evn) {
				//evn.stopPropagation();
            resProm = lightController.setPower(controller.id, controller.power);
            controller.awaitingResponse = true;
            resProm.then(function () {
                controller.awaitingResponse = false;
                console.log("Done setting power of controller " + controller.id + " to power level " + controller.power);
            });
        }

        this.getControllers = function () {
            resProm = lightController.getSwitches();
            resProm.then(function (result) {
                console.log("got \n" + result);
            });
        }
        this.getLightColor = function (controller) {
            var color = "rgb(" + controller.power.powerMap() + "," + controller.power.powerMap() + ",0)";
            return color;
        }
        var fakeId = 1;
        this.createFakeController = function () {
            var c = {
                id: fakeId++,
                state: false,
                power: 255,
                name: "test" + fakeId,
                awaitingResponse: false
            }
            this.controllers.push(c);
        }
        this.getControllers = function () {
            var p = lightController.getSwitches();
            //console.log(this.controllers);
            p.then(function (r) {
                //console.log(r);
                //console.log(cont.controllers);
                cont.controllers = r.data;
            }, function (err) {
                console.log(err);
            })
        }
        this.connectNewControllers = function () {
				console.log("HEre");
            var p = lightController.connectNewControllers();
            p.then(function (r) {
                cont.getControllers();
            });
        }
        this.showOptions = function ($ev, controller) {
            var useFullScreen = ($mdMedia('sm') || $mdMedia('xs'))  && customFullscreen;
            $mdDialog.show({
                controller: "DialogController",
                controllerAs: "dialog",
                templateUrl: "/templates/dialog.html",
                bindToController: true,
                locals: {
					max: cont.maxPower,
					min: cont.minPower,
                    controller: controller
                },
                targetEvent: $ev,
                fullscreen: useFullScreen
            }).then(function(controller) {
				lightController.save(controller);
			});
        }
        this.getControllers();
    }])
