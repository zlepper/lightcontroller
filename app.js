var express = require("express");
var path = require("path");
var app = express();
var portIO = require("./app/controller-action");
var bodyParser = require("body-parser");
var routes = require("./app/routes/toggles");
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

app.use("/api", routes);

app.get('/*', function (req, res) {
    res.sendFile(indexFile);
});

app.listen(80, function() {
	console.log("Listering for requests");
});

n = 0;
portIO.addController();
/*setInterval(function() {
	portIO.sendNumber(1, n);
	console.log(n);
	n++;
	if(n >= 255) {
		n = 0;
	} 

}, 10000);*/
//portIO.sendNumber(1,1);
