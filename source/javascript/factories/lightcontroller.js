(function() {
	var switchBase = "/api/switches";
	function LightController($http) {
		this.toggle = function(controllerNumber) {
			var postPromise = $http.put(switchBase + "/" + controllerNumber + "/toggle", {});
			return postPromise;
		}

		this.on = function(controllerNumber) {
			var postPromise = $http.put(switchBase  + "/"+ controllerNumber + "/on", {});
			return postPromise;
		}

		this.off = function(controllerNumber) {
			return $http.put(switchBase  + "/"+ controllerNumber + "/off", {});
		}
		this.setPower = function(controllerNumber, power) {
			return $http.put(switchBase  + "/"+ controllerNumber + "/" + power, {});
		}
		this.getSwitches = function() {
			return $http.get(switchBase, {}); 
		}
	};

	function LightControllerFactory($http) {
		return new LightController($http);
	}

	angular.module("lightcontroller").factory("LightController", ["$http", LightControllerFactory]);
})();
