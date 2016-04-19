angular.module("lightcontroller", ["ngMaterial", "ui.router", "ngAnimate", "ngMessages"]).directive('dragEnd', function () {
	return {
		restrict: 'A',
		scope: false,
		link: function (scope, element, attrs) {
			function end() {
				scope.$apply(attrs.dragEnd);
			}
			element.on('$md.dragend', end)
				.on("click", end);
		}
	}
});
