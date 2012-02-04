/*globals
jQuery: true, $: true, _: true, Backbone: true, window: true
*/
(function () {
  'use strict';

  var Q3P = window.Q3P = typeof (window.Q3P) === 'undefined' ? {} : window.Q3P;

  Q3P.GameView = Backbone.View.extend({
    id: 'game-view',

    defaultTemplate: '<header class="jumbotron subhead">' +
        '<h1>Doh.</h1>' +
        '<p class="lead">We couldn\'t find the plugin to be loaded.</p>' +
        '<p class="download-info">' +
          '<a class="btn btn-primary btn-large" href="https://github.com/inolen/q3plugin">View project on GitHub</a>' +
          '<a class="btn btn-large" href="assets/bootstrap.zip">Download q3plugin</a>' +
        '</p>' +
      '</header>',

    events: {
      'click button': 'loadGame'
    },

    initialize: function () {
      _.bindAll(this, 'render');
    },

    loadGame: function () {
      var self = this;
      self.render();
    },

    render: function () {
      var html = _.template(this.defaultTemplate);

      $(Q3P.getPlugin()).width(1170).height(731);
      $(this.el).empty();

      if (this.options.connect !== undefined) {
        var split = this.options.connect.split(':'),
          address = split[0],
          port = split[1];

        console.log(address);
        console.log(port);

        setTimeout(function () {
          Q3P.getPlugin().connect(address, port);
        }, 1000);
      }

      return this;
    }
  });
}());