/*globals
jQuery: true, $: true, _: true, Backbone: true, window: true
*/
(function () {
  'use strict';

  var Q3P = window.Q3P = typeof (window.Q3P) === 'undefined' ? {} : window.Q3P;
  var jade = require('jade');

  Q3P.GameView = Backbone.View.extend({
    id: 'game-view',

    template: "div.row.settings\n\
  div.span16\n\
    div.clearfix\n\
      input#executable.xlarge(type='text', name='executable', placeholder='Enter the path to your ioquake3 executable', value=executable)\n\
      button.btn.primary(type='button') Load\n\
- if (typeof executable !== 'undefined' && executable !== '')\n\
  div.row\n\
    div.span16\n\
      object#plugin(type='application/x-q3plugin', width='800', height='600')\n\
        param(name='executable', value=executable)\n\
        - if (typeof connect !== 'undefined')\n\
          param(name='connect', value=connect)",

    events: {
      'click button': 'loadGame'
    },

    initialize: function () {
      _.bindAll(this, 'render');
      this.templateCompiled = jade.compile(this.template);
    },

    loadGame: function () {
      var self = this;

      Q3P.AppSettings.save({
        executable: $(this.el).find('#executable').val(),
      }, {
        success: function () {
          self.render();
        }
      });
    },

    render: function () {
      var params = {
        executable: Q3P.AppSettings.get('executable')
      };

      if (this.options.connect !== undefined) {
        params.connect = this.options.connect;
      }

      var html = this.templateCompiled(params);
      $(this.el).html(html);

      return this;
    }
  });
}());