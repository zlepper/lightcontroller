var express = require("express");
var router = express.Router();
var controller = require("../controller-action.js");


router.get("/switches", function(req, res) {
	var switches = controller.getAll();
	res.json(switches);	
});

router.put("/switches/:id/on", function(req, res) {
	var id = req.params.id;
	var c = controller.get(id);
	c.turnOn();
	res.sendStatus(200);
});

router.put("/switches/:id/off", function(req, res) {
	var id = req.params.id;
	var c = controller.get(id);
	c.turnOff();
	res.sendStatus(200);
});

router.put("/switches/:id/toggle", function(req, res) {
	var id = req.params.id;
	var c = controller.get(id);
	c.toggle();
	res.sendStatus(200);
});

router.put("/switches/:id/:level", function(req, res) {
	console.log(req.params);
	var id = Number(req.params.id);
	var level = Number(req.params.level);
	var c = controller.get(id);
	c.setPower(level);
	res.sendStatus(200);
});

module.exports = router;
