var express = require("express");
var path = require("path");
var app = express();
var portIO = require("./app/portio");
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

//var n = 1;
//////var intervaller = setInterval(function() {
	//portIO.set(0, n, function(err, value) {
		//if(err) return console.log(err, value);
	//});
	//if(n)
		//n = 0;
	//else
		//n = 1;

//}, 2000); 
