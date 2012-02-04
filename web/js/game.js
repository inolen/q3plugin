/*globals
jQuery: true, $: true, _: true, Backbone: true, window: true
*/
(function () {
  'use strict';

  var Q3P = window.Q3P = typeof (window.Q3P) === 'undefined' ? {} : window.Q3P;
  var jade = require('jade');

  Q3P.GameView = Backbone.View.extend({
    id: 'game-view',

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
      $(this.el).html('');

      if (this.options.connect !== undefined) {
        Q3P.plugin.connect(this.options.connect);
      }

      return this;
    }
  });
}());