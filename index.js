const express = require('express');
const app     = express();
const router  = express.Router();
const path    = require('path');
const server  = require('http').createServer(app);


app.use('/', express.static(path.join(__dirname + '/html')));
app.use('/css', express.static(path.join(__dirname + '/css')));
app.use('/js', express.static(path.join(__dirname + '/js')));

app.use('/', router);
const port = process.env.port || 2337;
server.listen(port);
console.log("Server running at http:://localhost:%d", port);