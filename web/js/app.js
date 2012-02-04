/*globals
jQuery: true, $: true, _: true, Backbone: true, window: true, localStorage: true, document: true
*/
(function () {
  'use strict';

  var Q3P = window.Q3P = typeof (window.Q3P) === 'undefined' ? {} : window.Q3P;

  Q3P.bootstrap = function () {
    var router = new Q3P.AppRouter();

    var finish = function () {
      Backbone.history.start({ root: '/index.html' });
    };

    Q3P.AppSettings.fetch({ success: finish, error: finish }, { silent: true });
  };

  // Replace q3-style colored strings with the appropriate HTML.
  Q3P.colorize = function (text) {
    // White isn't outputted full white since we're on a white background.
    var colors = ['#000', '#ff0000', '#00ff00', '#ffff00', '#0000ff', '#00ffff', '#ff00ff', '#ccc'],
      replace = function (str, m1, m2) {
        var index = parseInt(m1, 10);

        if (isNaN(index) || index > 7) {
          index = 7;
        }

        return '<span style="color: ' + colors[index] + '">' + m2 + '</span>';
      };

    return text.replace(/\^(.)(.*?)(?=\^|$)/g, replace);
  };

  Q3P.getPlugin = function () {
    return document.getElementById('plugin');
  };

  Q3P.AppSettings = new (Backbone.Model.extend({
    defaults: {
      masterserver: 'master.ioquake3.org:27950'
    },
    sync: function (method, model, options) {
      var name = 'Q3P.AppSettings';
      var resp = model;

      switch (method) {
      case 'read':
        resp = JSON.parse(localStorage.getItem(name));
        break;
      case 'create':
        localStorage.setItem(name, JSON.stringify(model));
        break;
      case 'update':
        localStorage.setItem(name, JSON.stringify(model));
        break;
      case 'delete':
        localStorage.removeItem(name);
        break;
      }

      if (resp) {
        options.success(resp);
      } else {
        options.error('Record not found');
      }
    }
  }))();

  Q3P.AppRouter = Backbone.Router.extend({
    routes: {
      'game?connect=:server': 'game',
      'servers': 'servers',
      '': 'game'
    },

    servers: function () {
      var browserView = new Q3P.BrowserView();
      $('#content').html(browserView.render().el);
    },

    game: function (server) {
      var gameView = new Q3P.GameView({
        connect: server
      });
      $('#content').html(gameView.render().el);
    }
  });
}());