// Load a few libraries
var express = require("express");
var path = require("path");
var app = express();
var bodyParser = require("body-parser");
var portIO = require("./app/controller-action");
var routes = require("./app/routes/toggles");
// Configure the public directory to be server as static files whenever something is requested from there
app.use(express.static("public"));

// Calculate the path to the index file of the frontend
var indexFile = path.join(__dirname, "public", "index.html");

// Enable parsing of requests
app.use(bodyParser.urlencoded({extended: true}));
app.use(bodyParser.json());

// Testing route that toggles a controller whenever a post request comes in
app.post("/api/toggle", function(req, res) {
		var controller = req.body.controller;
		portIO.toggle(controller, function(err, value) {
				res.json({value: value});	
		});
});

// Define the api routes we can request against
app.use("/api", routes);

// Make sure our app runs in HTML5 mode. 
app.get('/*', function (req, res) {
		res.sendFile(indexFile);
});

// Listen for webrequests on port 80, which is the default webport
app.listen(80, function() {
		// Tell us that the appliction is ready for requests
		console.log("Listering for requests");
});

// Some debug code
n = 0;
// Create a fake controller, which we are using during development while waiting for the pic code to be done. 
//portIO.addController();
// Send a new signal every few seconds, again debugging
/*setInterval(function() {
		portIO.sendNumber(1, n);
		console.log(n);
		n++;
		if(n >= 255) {
				n = 0;
		} 

}, 5000);*/
