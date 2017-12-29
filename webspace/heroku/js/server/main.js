"use strict";

var Server = require('./server').Server;

var io = require('socket.io').listen(1337);

var server = new Server();

server.initListeners(io);

