(function($){
    Drupal.behaviors.zimbra_multi_tenancy = {
        attach: function(context, settings) {
            $('#members_add:not(.click-processed)').click(function(e) {
                move_options('.group_account_list', '.group_members_list');
            }).addClass('click-processed');
            $('#members_remove:not(.click-processed)').click(function(e) {
                move_options('.group_members_list', '.group_account_list');
            }).addClass('click-processed');

            $('.group_account_list:not(.dblclick-processed)').dblclick(function(e) {
                move_options('.group_account_list', '.group_members_list');
            }).addClass('dblclick-processed');
            $('.group_members_list:not(.dblclick-processed)').dblclick(function(e) {
                move_options('.group_members_list', '.group_account_list');
            }).addClass('dblclick-processed');

            $('#group-node-form:not(.submit-processed)').submit(function(){
                before_sumbit();
            }).addClass('submit-processed');

            for (ajax_el in settings.ajax){
                if (Drupal.ajax[ajax_el].element.form){
                    var form = Drupal.ajax[ajax_el].element.form;
                    if (form.id === 'group-node-form' && !$(form).hasClass('before-serialize-processed')){
                        $(form).addClass('before-serialize-processed');
                        Drupal.ajax[ajax_el].beforeSerialize = function($form, options){
                            before_sumbit();
                        }
                    }
                }
            }

            $(':checkbox[name^="data_type"]').click(function(){
                if(this.value === 'domain' && !this.checked)
                {
                    $(':checkbox[name^="data_type"]').attr('checked', '');
                }
                if(this.value === 'alias' && this.checked)
                {
                    $(':checkbox[name^="data_type"]').attr('checked', 'checked');
                }
                if(this.value !== 'domain' && this.checked)
                {
                    $('#edit-data-type-domain').attr('checked', 'checked');
                }
            });
        }
    };
})(jQuery);

function before_sumbit(){
    var members_list = '';
    jQuery('.group_account_list option:selected').each(function(){
        this.selected = false;
    });
    jQuery('.group_members_list option:selected').each(function(){
        this.selected = false;
    });
    jQuery('.group_members_list option').each(function(){
        if(members_list.length === 0){
            members_list = this.value;
        }else{
            members_list += '|' + this.value;
        }
    });
    jQuery('#edit-field-group-members-und-0-value').val(members_list);
}

function move_options(from_selector, to_selector){
    var from_select = jQuery(from_selector).get(0);
    var to_select = jQuery(to_selector).get(0);

    // Save the selected index
    var selectedIndex = from_select.selectedIndex;
    var options = from_select.options;

    // Remove all selected options
    for (var i =0; i < options.length; i++){
        if (options[i].selected){ 
            var text = options[i].innerHTML;
            var value = options[i].value;
            add_option_item(to_select, text, value);
        }
    }
    for (var i = options.length - 1 ; i >= 0 ; i--){
        if (options[i].selected){ 
            from_select.remove(i) ;
        }
    }

    // Reset the selection based on the original selected index
    if (from_select.options.length > 0){
        if ( selectedIndex >= from_select.options.length ){
            selectedIndex = from_select.options.length - 1;
        } 
        from_select.selectedIndex = selectedIndex ;
    }
    if (to_select.selectedIndex == -1 && to_select.options.length > 0){
        to_select.selectedIndex = 0;
    }
}

function add_option_item(select_object, option_text, option_value, document_object, index){ 
    var option;

    if (document_object)
        option = document_object.createElement("option");
    else
        option = document.createElement("option") ;
    if (index != null)
        select_object.options.add(option, index);
    else
        select_object.options.add(option);

    option.innerHTML = option_text.length > 0 ? option_text : "&nbsp;";
    option.value     = option_value ;
    return option;
}
