/*globals
jQuery: true, $: true, _: true, Backbone: true, window: true
*/
(function () {
  'use strict';

  var Q3P = window.Q3P = typeof (window.Q3P) === 'undefined' ? {} : window.Q3P;

  Q3P.Server = Backbone.Model.extend({
    defaults: function () {
      return {
        game: 'N/A',
        hostname: 'N/A',
        mapname: 'N/A',
        clients: 0,
        maxclients: 0
      };
    },
    initialize: function () {
      var self = this,
        address = this.get('address'),
        port = this.get('port');

      Q3P.getPlugin().getServerInfo(address, port, function (err, info) {
        if (!err) {
          self.set(info);
        }
      });
    },
  });

  Q3P.ServerList = Backbone.Collection.extend({
    model: Q3P.Server,

    sync: function (method, model, options) {
      var self = this;

      if (method === 'read') {
        var masterserver = Q3P.AppSettings.get('masterserver'),
          split = masterserver.split(':'),
          address = split[0],
          port = split[1];

        Q3P.getPlugin().getAllServers(address, port, function (err, servers) {
          if (err) {
            return options.error(err);
          }

          options.success(servers);
        });
      }
    }
  });

  Q3P.ServerView = Backbone.View.extend({
    tagName: 'tr',

    events: {
      'click td': 'connect'
    },

    initialize: function () {
      this.model.bind('change', this.render, this);
    },

    connect: function () {
      var address = this.model.get('address'),
        port = this.model.get('port');

      Backbone.history.navigate('game?connect=' + address + ':' + port, true);
    },

    render: function () {
      $(this.el)
        .empty()
        .append('<td>' + Q3P.colorize(this.model.get('hostname')) + '</td>')
        .append('<td>' + this.model.get('mapname') + '</td>')
        .append('<td>' + this.model.get('clients') + '/' + this.model.get('maxclients') + '</td>')
        .append('<td>' + this.model.get('game') + '</td>');

      this.trigger('change');

      return this;
    }
  });

  Q3P.BrowserView = Backbone.View.extend({
    id: 'browser-view',

    template: '<div class="padded">' +
        '<div class="settings">' +
          '<h2>Browse servers</h2>' +
          '<div class="clearfix">' +
            '<input id="masterserver" class="xlarge" type="text" name="masterserver" placeholder="Enter the address of a master server" value="<%= masterserver %>" />' +
            '<button class="btn primary" type="button">Refresh</button>' +
          '</div>' +
        '</div>' +
        '<table id="servers" class="table zebra-striped">' +
          '<thead>' +
            '<tr>' +
              '<th>Name</th>' +
              '<th>Map</th>' +
              '<th>Players</th>' +
              '<th>Game</th>' +
            '</tr>' +
          '</thead>' +
          '<tbody></tbody>' +
        '</table>' +
      '</div>',

    events: {
      'click button': 'refresh'
    },

    initialize: function () {
      this.servers = new Q3P.ServerList();
      this.servers.bind('add', this.add, this);
      this.servers.bind('reset', this.reset, this);
      this.servers.fetch();
    },

    refresh: function () {
      var self = this;

      Q3P.AppSettings.save({
        masterserver: $(this.el).find('#masterserver').val(),
      }, {
        success: function () {
          self.servers.fetch();
        }
      });
    },

    add: function (server) {
      var self = this,
        view = new Q3P.ServerView({model: server});

      // Trigger update event for tablesorter.
      view.bind('change', function () {
        $(self.el).find('#servers').trigger('update');
      });

      $(this.el).find('#servers tbody').append(view.render().el);
    },

    reset: function (servers) {
      $(this.el).find('#servers tbody').empty();
      servers.each(this.add, this);
    },

    render: function () {
      var html = _.template(this.template, {
        masterserver: Q3P.AppSettings.get('masterserver')
      });

      $(Q3P.getPlugin()).css({width: 0, height: 0});
      $(this.el).html(html).find('#servers').tablesorter();

      return this;
    }
  });
}());