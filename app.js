var express = require("express");
var path = require("path");
var app = express();
var portIO = require("./app/controller-action");
var bodyParser = require("body-parser");
app.use(express.static("public"));

var indexFile = path.join(__dirname, "public", "index.html");

// Enable parsing of requests
app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());

app.post("/api/toggle", function(req, res) {
	var controller = req.body.controller;
	portIO.toggle(controller, function(err, value) {
		res.json({value: value});	
	});
});

app.get('/*', function (req, res) {
    res.sendFile(indexFile);
});

app.listen(80, function() {
	console.log("Listering for requests");
});

n = 0;
setTimeout(function() {
	portIO.sendNumber(1, n);
	console.log(n);
	n++;
}, 1000);
