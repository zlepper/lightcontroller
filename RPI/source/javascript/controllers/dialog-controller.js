angular.module("lightcontroller")
    .controller("DialogController", ["$scope", "$mdDialog", "LightController", function($scope, $mdDialog, lightcontroller) {
			console.log(this.locals);
        console.log("Loaded dialogController");
		this.controller = this.locals.controller;
		this.close = function() {
			$mdDialog.cancel();
		}
		this.save = function() {
			$mdDialog.hide(this.controller);	
		}
		this.addPoint = function() {
			this.controller.points.push({power: 0});
		}
    }]);
