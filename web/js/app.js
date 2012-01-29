/*globals
jQuery: true, $: true, _: true, Backbone: true, window: true
*/
(function () {
  'use strict';

  var Q3P = window.Q3P = typeof (window.Q3P) === 'undefined' ? {} : window.Q3P;

  Q3P.bootstrap = function () {
    Q3P.appView = new Q3P.AppView({el: 'body'});
  };

  // Replace q3-style colored strings with the appropriate HTML.
  Q3P.colorize = function (text) {
    // White isn't outputted full white since we're on a white background.
    var colors = ['#000', '#ff0000', '#00ff00', '#ffff00', '#0000ff', '#00ffff', '#ff00ff', '#ccc'],
      replace = function (str, m1, m2) {
        var index = parseInt(m1);

        if (index === NaN || index > 7) {
          index = 7;
        }

        return '<span style="color: ' + colors[index] + '">' + m2 + '</span>';
      };

    return text.replace(/\^(.)(.*?)(?=\^|$)/g, replace);
  };

  Q3P.Server = Backbone.Model.extend({
    defaults: function () {
      return {
        hostname: 'N/A',
        mapname: 'N/A',
        clients: 0,
        sv_maxclients: 0,
        game: 'N/A'
      };
    },
    initialize: function () {
      var self = this;

      $.get(Q3P.api + '/server', {
        address: this.get('address'),
        port: this.get('port')
      }, function (data) {
        self.set(data);
      }, 'json');
    },
  });

  Q3P.ServerList = Backbone.Collection.extend({
    model: Q3P.Server,

    url: function () {
      return Q3P.api + '/servers';
    }
  });

  Q3P.ServerView = Backbone.View.extend({
    tagName: 'tr',

    initialize: function () {
      this.model.bind('change', this.render, this);
    },

    render: function () {
      $(this.el)
        .empty()
        .append('<td>' + Q3P.colorize(this.model.get('hostname')) + '</td>')
        .append('<td>' + this.model.get('mapname') + '</td>')
        .append('<td>' + this.model.get('clients') + '/' + this.model.get('sv_maxclients') + '</td>')
        .append('<td>' + this.model.get('game') + '</td>');

      this.trigger('change');

      return this;
    },

    remove: function () {
      $(this.el).remove();
    },

    clear: function () {
      this.model.destroy();
    }
  });

  Q3P.ServerBrowserView = Backbone.View.extend({
    tagName: 'table',

    initialize: function () {
      _.bindAll(this, 'render');

      this.servers = new Q3P.ServerList();
      this.servers.bind('add', this.add, this);
      this.servers.bind('reset', this.addAll, this);

      this.servers.fetch();
    },

    add: function (server) {
      var view = new Q3P.ServerView({model: server});

      // Trigger update event for tablesorter.
      view.bind('change', function () {
        $(this.el).trigger('update');
      });

      $(this.el).find('tbody').append(view.render().el);
    },

    addAll: function(servers) {
      servers.each(this.add, this);
    },

    render: function () {
      $(this.el).append('<thead><tr><th>Name</th><th>Map</th><th>Players</th><th>Game</th></tr></thead>');
      $(this.el).append('<tbody>');
      $(this.el).tablesorter();
      return this;
    }
  });

  Q3P.GameView = Backbone.View.extend({
    render: function () {
      var $object = $('<object>', {
        id: 'plugin0',
        type: 'application/x-q3plugin',
        width: 800,
        height: 600
      })
      //append( param(name='onload', value='pluginLoaded') )
      $(this.el).append($object);
      return this;
    }
  });

  Q3P.AppView = Backbone.View.extend({
    initialize: function () {
      _.bindAll(this, 'render');
      this.render();
    },

    render: function () {
      var browserView = new Q3P.ServerBrowserView();
      $('#content').append(browserView.render().el);

      var gameView = new Q3P.GameView();
      $('#content').append(gameView.render().el);
    }
  });
}());