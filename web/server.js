'use strict';

var async = require('async'),
  CORS = require('connect-xcors'),
  express = require('express'),
  scom = require('./lib/ServerCommands');

var masterServer = {
  address: 'master.ioquake3.org',
  port: 27950
};

var getServers = function (callback) {
  var i = 0;

  async.waterfall([
    function (next) {
      scom.getAllServers(masterServer.address, masterServer.port, function (err, servers) {
        if (err) {
          return next(err);
        }

        next(null, servers);
      });
    },
    function (servers, next) {
      var map = function (server, cb) {
        scom.getServerInfo(server.address, server.port, function (err, info) {
          if (err) {
            return cb(err);
          }

          i = i + 1;

          console.log('got ' + i + ' of ' + servers.length);

          cb(null, info);
        });
      };

      async.map(servers, map, function (err, results) {
        if (err) {
          return next(err);
        }

        callback(null, results);
      });
    }
  ], function (err) {
    callback(err);
  });
};

var init = function () {
  var app = express.createServer(CORS());

  app.use(express.bodyParser());

  app.get('/servers', function (req, res, next) {
    scom.getAllServers(masterServer.address, masterServer.port, function (err, servers) {
      if (err) {
        return next(err);
      }
      res.send(servers);
    });
  });
  app.get('/server', function (req, res, next) {
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