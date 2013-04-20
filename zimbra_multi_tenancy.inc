<?php

/**
 * Form builder function for module settings.
 */
function zimbra_multi_tenancy_settings(){
  $form['zimbra_domain_server'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain for automatic create new user'),
    '#default_value' => variable_get('zimbra_domain_server', 'iwayvietnam.com'),
    '#description' => t('Domain for automatic create new user. Do not input "@" character.'),
	'#require' => 1,
	'#weight' => -9,
  );
  
  $roles = user_roles();
  $form['zimbra_tenant_role_id'] = array(
  	'#type' => 'select',
    '#title' => t('Role manage Tenant'),
    '#default_value' => variable_get('zimbra_tenant_role_id', 2),
  	'#options' => $roles,
    '#weight' => -8,
  );
  $form['zimbra_domain_role_id'] = array(
  	'#type' => 'select',
    '#title' => t('Role manage Domain'),
    '#default_value' => variable_get('zimbra_domain_role_id', 2),
  	'#options' => $roles,
    '#weight' => -7,
  );  
  
  $form = system_settings_form($form);
  return $form;
}

function zimbra_multi_tenancy_settings_validate($form, &$form_state){
	//Check invalid of Tenant name
  $values =$form_state['values'];
  $zimbra_domain_server =strtolower($form_state['values']['zimbra_domain_server']);
  if(!preg_match('!^[a-z0-9_.-]+$!', $zimbra_domain_server)){
	form_set_error('zimbra_domain_server', t('The @fieldname must contain only lowercase letters, numbers, and underscores.', array('@fieldname' =>$form['zimbra_domain_server']['#title'])));
  }
  //print_r($form_state['values']);exit;
}


function zimbra_page_addcontent(){
	$type =arg(3);
	
	if($type ==''){
	  $item = menu_get_item();
	  $content = system_admin_menu_block($item);
	  
	  if (count($content) == 1) {
		$item = array_shift($content);
		drupal_goto($item['href']);
	  }
	  return theme('node_add_list', array('content' => $content));
	}else{
		drupal_goto('node/add/' .$type);
		return '';
	}
}

function zimbra_page_content(){
/*
	  $item = menu_get_item();
	  $content = system_admin_menu_block($item);
	  
	  if (count($content) == 1) {
		$item = array_shift($content);
		drupal_goto($item['href']);
	  }
	  return theme('node_add_list', array('content' => $content));
*/
	$output ='';
	
	$menu_items =array();
	if(user_access('create tenant content')) $menu_items[] =l('List of Tenant','admin/zimbra/tenant');
	if(user_access('create domain content')) $menu_items[] =l('List of Domain','admin/zimbra/domain');
	if(user_access('create group content')) $menu_items[] =l('List of Group','admin/zimbra/group');
	if(user_access('create alias content')) $menu_items[] =l('List of Alias','admin/zimbra/alias');
	if(user_access('create mailbox content')) $menu_items[] =l('List of Mailbox','admin/zimbra/mailbox');
	
	if(user_access('create tenant content')) $menu_items[] =l('Add Tenant','node/add/tenant');
	if(user_access('create domain content')) $menu_items[] =l('Add Domain','node/add/domain');
	if(user_access('create group content')) $menu_items[] =l('Add Group','node/add/group');
	if(user_access('create alias content')) $menu_items[] =l('Add Alias','node/add/alias');
	if(user_access('create mailbox content')) $menu_items[] =l('Add Mailbox','node/add/mailbox');
		
	$i =0;
	foreach($menu_items as $value){
		$i++;
		$output .='<li class="item mnu-item-' .$i .'">' .$value .'</li>';
	}	
		
	if($output !='') $output ='<ul class="zimbra-general-page">' .$output .'</ul>';
	return $output;
}




