'use strict';

var dgram = require('dgram');

var ConnectionlessPacket = (function () {
  var itself = {},
    header = '\xff\xff\xff\xff';

  itself.send = function (data, address, port, callback) {
    // Prefix with header and type.
    var message = header + data,
    // Convert to a buffer.
      buffer = new Buffer(message, 'ascii'),
    // Create UDP socket and send.
      client = dgram.createSocket('udp4'),
      timer;

    // Handle response.
    client.on('message', function (msg, rinfo) {
      clearTimeout(timer);
      client.close();

      // Strip header.
      msg = msg.slice(header.length);

      callback(null, msg);
    });

    client.on('error', function (err) {
      clearTimeout(timer);
      client.close();
      callback(err);
    });

    client.send(buffer, 0, buffer.length, port, address, function () {
      timer = setTimeout(function () {
        client.close();
        callback(new Error('Request timed out'));
      }, 1000);
    });
  };

  return itself;
}());

var ServerCommands = (function () {
  var itself = {};

  itself.getAllServers = function (address, port, callback) {
    var parseResponse = function (buffer) {
      var servers = [];

      // Skip past message type.
      var i = 18;

      console.log(buffer.toString('ascii'));

      // Servers come in the format: /byte,byte,byte,byte,short
      while (i < buffer.length) {
        var c = String.fromCharCode(buffer[i]);

        i = i + 1;

        if (c === '\\') {
          // If we have enough data to read.
          if (buffer.length - i >= 6) {
            var address = buffer.readUInt8(i) + '.' + buffer.readUInt8(i + 1) + '.' + buffer.readUInt8(i + 2) + '.' + buffer.readUInt8(i + 3),
              port = buffer.readUInt16BE(i + 4);

            servers.push({ address: address, port: port });

            i = i + 6;
          }
        }
      }

      return servers;
    };

    ConnectionlessPacket.send(
      'getservers Quake3Arena 68 empty full',
      address,
      port,
      function (err, buffer) {
        if (err) {
          return callback(err);
        }
        var servers = parseResponse(buffer);
        callback(null, servers);
      }
    );
  };

  itself.getServerInfo = function (address, port, callback) {
    var parseResponse = function (buffer) {
      var info = {}, split, i;

      // Skip past message type, newline and first \
      buffer = buffer.slice(14);

      // Split message up by \
      split = buffer.toString('ascii').split('\\');

      for (i = 0; i < split.length; i = i + 2) {
        info[split[i]] = split[i + 1];
      }

      return info;
    };

    ConnectionlessPacket.send(
      'getinfo xxx',
      address,
      port,
      function (err, buffer) {
        if (err) {
          return callback(err);
        }
        var info = parseResponse(buffer);
        callback(null, info);
      }
    );
  };

  return itself;
}());

module.exports = ServerCommands;