/*globals
jQuery: true, $: true, _: true, Backbone: true, window: true
*/
(function () {
  'use strict';

  var Q3P = window.Q3P = typeof (window.Q3P) === 'undefined' ? {} : window.Q3P;

  Q3P.GameView = Backbone.View.extend({
    id: 'game-view',

    initialize: function () {
      $(Q3P.getPlugin()).width(1170).height(731);

      if (this.options.connect !== undefined) {
        var split = this.options.connect.split(':'),
          address = split[0],
          port = split[1];

        Q3P.getPlugin().connect(address, port);
      }
    }
  });
}());