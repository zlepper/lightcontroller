var moment = require("moment");
var controllerAction = require("./controller-action");

function getBetweenValue(min, max, value) {
		console.log(arguments);
		var dif = max - min;
		var fill = value - min;
		var fillPercent = fill / dif;
		return fillPercent;
}

setInterval(function() {
		var controllers = controllerAction.getAllControllers();	
		for(var i = 0; i < controllers.length; i++) {
				var controller = controllers[i];
				if(!controller.isAuto) {
						continue;
				}
				var points = controller.points;
				console.log(points.length);
				for(var j = 0; j < points.length; j++) {
						console.log(moment().unix());
						var point = points[j];
						var nextPoint;
						if(j == points.length - 1) {
								if(points.length > 2) {
									nextPoint = points[0];
								} else {
									break;
								}
						} else {
								nextPoint = points[j+1];
						}
						if(!point.time) {
								continue;
						}
						var time = moment(point.time);
						var nextTime = moment(nextPoint.time);
						var nowTime = moment();
						if(time.hour() <= nowTime.hour() && nowTime.hour() <= nextTime.hour()) {
								if(time.minute() <= nowTime.minute() && nowTime.minute() <= nextTime.minute()) {

										calculate(time, nowTime, nextTime, controller, point, nextPoint);
										break;
								}
						}
				}
		}
}, 1000);

function calculate(before, now, next, controller, point, nextPoint) {

		console.log("The time is now!");
		var startSecond = before.hour() * 60 * 60 + before.minute() * 60 + before.second();
		var nextSecond = next.hour() * 60 * 60 + next.minute() * 60 + next.second();
		var currentSecond = now.hour() * 60 * 60 + now.minute() * 60 + now.second();
		var diff = getBetweenValue(startSecond, nextSecond, currentSecond);
		var startPower = point.power;
		var endPower = nextPoint.power;
		var powerDif = endPower - startPower;
		var nowPower = Math.round(startPower + powerDif * diff);
		controller.setPower(nowPower);
		console.log("The power is now " + nowPower);
}

console.log("Auto time ready");
