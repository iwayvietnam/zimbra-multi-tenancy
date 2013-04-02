(function($){

Drupal.behaviors.groupform_action = {
  attach: function(context, settings) {
     $('#members_add').click(function(e) {
        AddRemoveOptions( 'edit-field-group-account-list', 'edit-field-group-members-list');
     });
     $('#members_remove').click(function(e) {
        AddRemoveOptions( 'edit-field-group-members-list', 'edit-field-group-account-list');
     });
     
     $('#edit-field-group-account-list').dblclick(function(e) {
        AddRemoveOptions( 'edit-field-group-account-list', 'edit-field-group-members-list');
     });
     $('#edit-field-group-members-list').dblclick(function(e) {
        AddRemoveOptions( 'edit-field-group-members-list', 'edit-field-group-account-list');
     });     
  }
};

Drupal.behaviors.tenant_form_action = {
  attach: function(context, settings) {
  
    //Disable 2 textbox username and password when value of tenant user is null
    if($('#edit-field-tenant-user-und').val() && $('#edit-field-tenant-user-und').val()!='_none'){
        $('#edit-field-tenant-user-name').css("display","none");
        $('#edit-field-tenant-user-password').css("display","none");
    }
    $('#edit-field-tenant-user-und').change(function(){
        if($('#edit-field-tenant-user-und').val()!='_none'){
            $('#edit-field-tenant-user-name').css("display","none");
            $('#edit-field-tenant-user-password').css("display","none");
        }else{
            $('#edit-field-tenant-user-name').css("display","");
            $('#edit-field-tenant-user-password').css("display","");
        }
    });
    
    //Disable 2 textbox username and password when value of domain user is null
    if($('#edit-field-domain-user-und').val() && $('#edit-field-domain-user-und').val()!='_none'){
        $('#edit-field-domain-user-name').css("display","none");
        $('#edit-field-domain-user-password').css("display","none");
    }
    $('#edit-field-domain-user-und').change(function(){
        if($('#edit-field-domain-user-und').val()!='_none'){
            $('#edit-field-domain-user-name').css("display","none");
            $('#edit-field-domain-user-password').css("display","none");
        }else{
            $('#edit-field-domain-user-name').css("display","");
            $('#edit-field-domain-user-password').css("display","");
        }
    });
    
    //alert('sss');
  }
};

})(jQuery);


function group_before_validate(){
    
    combRemoveID ='edit-field-group-account-list';
    combAddID ='edit-field-group-members-list';
    comboRemove =GetE(combRemoveID);
    comboAdd =GetE(combAddID); 
    
    //Unselected item
    comboRemove.selectedIndex =-1;
    comboAdd.selectedIndex =-1;
    
	// Insert value into item Members
	members_list ="";
	var oOptions = comboAdd.options ;
	
	for ( var i =0; i <oOptions.length; i++ ){
	    if(members_list !='')members_list +="|";
		members_list += oOptions[i].value;
	}
    GetE('edit-field-group-members-und-0-value').value =members_list;
}


function AddRemoveOptions( combRemoveID, combAddID){
	
	comboRemove =GetE(combRemoveID);
	comboAdd =GetE(combAddID);

	// Save the selected index
	var iSelectedIndex = comboRemove.selectedIndex ;
	var oOptions = comboRemove.options ;

	// Remove all selected options
	for ( var i =0; i < oOptions.length; i++ ){
		if (oOptions[i].selected){ 
			iText =oOptions[i].innerHTML;
			iValue =oOptions[i].value;
			AddItemOption( combAddID, iText, iValue);
		}
	}
	for ( var i = oOptions.length - 1 ; i >= 0 ; i-- ){
		if (oOptions[i].selected){ 
			comboRemove.remove(i) ;
		}
	}

	// Reset the selection based on the original selected index
	if ( comboRemove.options.length > 0 ){
		if ( iSelectedIndex >= comboRemove.options.length ) 
			iSelectedIndex = comboRemove.options.length - 1 ;
		comboRemove.selectedIndex = iSelectedIndex ;
	}
	if (comboAdd.selectedIndex == -1 && comboAdd.options.length >0) comboAdd.selectedIndex=0;
}


function AddItemOption(comboID, optionText, optionValue, documentObject, index ){	
	combo=GetE(comboID);
	var oOption ;

	if ( documentObject )
		oOption = documentObject.createElement("OPTION") ;
	else
		oOption = document.createElement("OPTION") ;
	if ( index != null )
		combo.options.add( oOption, index ) ;
	else
		combo.options.add( oOption ) ;

	oOption.innerHTML = optionText.length > 0 ? optionText : "&nbsp;" ;
	oOption.value     = optionValue ;
	return oOption;
}

function GetE(obj){
	if(document.all)
		return document.all[obj];
	else
		return document.getElementById(obj);
}


