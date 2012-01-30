'use strict';

var async = require('async'),
  CORS = require('connect-xcors'),
  express = require('express'),
  scom = require('./lib/ServerCommands');

var init = function () {
  var app = express.createServer(CORS());

  app.use(express.bodyParser());

  app.get('/servers', function (req, res, next) {
    var master = req.query.master,
      split = master.split(':');

    if (split.length < 2) {
      return next(new Error("Invalid address."));
    }

    var address = split[0],
      port = split[1];

    scom.getAllServers(address, port, function (err, servers) {
      if (err) {
        return next(err);
      }
      res.send(servers);
    });
  });

  app.get('/info', function (req, res, next) {
    var address = req.query.address,
      port = req.query.port;

    scom.getServerInfo(address, port, function (err, info) {
      if (err) {
        return next(err);
      }
      res.send(info);
    });
  });

  app.listen(3000);

  console.log('Web server started on port %s', app.address().port);
};

init();