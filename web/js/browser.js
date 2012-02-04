/*globals
jQuery: true, $: true, _: true, Backbone: true, window: true
*/
(function () {
  'use strict';

  var Q3P = window.Q3P = typeof (window.Q3P) === 'undefined' ? {} : window.Q3P;
  var jade = require('jade');

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

      Q3P.plugin.getServerInfo(address, port, function (err, info) {
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
        Q3P.plugin.getAllServers("master.ioquake3.org", 27950, function (err, servers) {
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

    template: "div.row.settings\n\
  div.span16\n\
    div.clearfix\n\
      input#masterserver.xlarge(type='text', name='masterserver', placeholder='Enter the address of a master server', value=masterserver)\n\
      button.btn.primary(type='button') Refresh\n\
div.row\n\
  div.span16\n\
    table.zebra-striped#servers\n\
      thead\n\
        tr\n\
          th Name\n\
          th Map\n\
          th Players\n\
          th Game\n\
      tbody",

    events: {
      'click button': 'refresh'
    },

    initialize: function () {
      this.templateCompiled = jade.compile(this.template);

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
      var html = this.templateCompiled({
        masterserver: Q3P.AppSettings.get('masterserver')
      });

      $(this.el)
        .html(html)
        .find('#servers')
        .tablesorter();

      return this;
    }
  });
}());