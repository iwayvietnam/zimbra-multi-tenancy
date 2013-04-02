<?php

/**
 * Form builder function for module settings.
 */
function zimbra_tenancy_settings(){
  $form['zimbra_domain_server'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain for automatic create new user'),
    '#default_value' => variable_get('zimbra_domain_server', 'iwayvietnam.com'),
    '#description' => t('Domain for automatic create new user. Do not input "@" character.'),
	'#require' => 1,
	'#weight' => -9,
  );
  $form = system_settings_form($form);

  return $form;
}

function zimbra_tenancy_settings_validate($form, &$form_state){
	//Check invalid of Tenant name
  $values =$form_state['values'];
  $zimbra_domain_server =strtolower($form_state['values']['zimbra_domain_server']);
  if(!preg_match('!^[a-z0-9_.-]+$!', $zimbra_domain_server)){
	form_set_error('zimbra_domain_server', t('The @fieldname must contain only lowercase letters, numbers, and underscores.', array('@fieldname' =>$form['zimbra_domain_server']['#title'])));
  }
  //print_r($form_state['values']);exit;
}
