(function($){

  Drupal.behaviors.zmt = {
    attach: function(context, settings) {
      $('#zmt-group-edit-form').once('submit', function () {
        $(this).submit(function(){
          $('select[id^="edit-members"] option').each(function() {
            this.selected = true;
          });
        });
      });

      $(':checkbox[name^="data_type"]').once('click', function () {
        $(this).click(function() {
          if (this.checked) {
            switch(this.value){
              case 'group':
                $('#edit-data-type-alias').attr('checked', 'checked');
              case 'alias':
                $('#edit-data-type-account').attr('checked', 'checked');
              case 'account':
                $('#edit-data-type-domain').attr('checked', 'checked');
            }
          };
          if (!this.checked) {
            switch(this.value){
              case 'domain':
                $('#edit-data-type-account').attr('checked', '');
              case 'account':
                $('#edit-data-type-alias').attr('checked', '');
              case 'alias':
                $('#edit-data-type-group').attr('checked', '');
            }
          };
        });
      });

      $('.zmt-remove-parent').once('click', function () {
        $(this).click(function(){
          $(this).parent().remove();
          return false;
        });
      });
    }
  };

  $(function() {

    Drupal.ajax.prototype.commands.zmt_ajax_redirect = function(ajax, response, status) {
      if (response.delay > 0) {
        setTimeout(function () {
          window.location.href = response.url;
        }, response.delay);
      }
      else {
        window.location.href = response.url;
      }
    };

    Drupal.ajax.prototype.commands.zmt_ajax_reload = function(ajax, response, status) {
      window.location = window.location;
    };

  });

})(jQuery);
