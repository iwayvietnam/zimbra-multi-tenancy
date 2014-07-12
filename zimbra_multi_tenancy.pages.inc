<?php

/**
 * Form builder function for module settings.
 */
function zimbra_multi_tenancy_settings() {
  $form['zimbra_enable_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable debug'),
    '#default_value' => variable_get('zimbra_enable_debug', 0),
    '#weight' => -6,
  );
  $roles = user_roles();
  $form['zimbra_zimbra_role_id'] = array(
    '#type' => 'select',
    '#title' => t('Role manage Zimbra multi tenancy'),
    '#default_value' => variable_get('zimbra_zimbra_role_id', 4),
    '#options' => $roles,
    '#weight' => -5,
  );
  $form['zimbra_tenant_role_id'] = array(
    '#type' => 'select',
    '#title' => t('Role for Tenant domain base'),
    '#default_value' => variable_get('zimbra_tenant_role_id', 5),
    '#options' => $roles,
    '#weight' => -4,
  );
  $form['zimbra_tenant_group_role_id'] = array(
    '#type' => 'select',
    '#title' => t('Role for Tenant group base'),
    '#default_value' => variable_get('zimbra_tenant_group_role_id', 6),
    '#options' => $roles,
    '#weight' => -3,
  );
  $form['zimbra_domain_role_id'] = array(
    '#type' => 'select',
    '#title' => t('Role for Domain'),
    '#default_value' => variable_get('zimbra_domain_role_id', 7),
    '#options' => $roles,
    '#weight' => -2,
  );  
  $form['zimbra_group_role_id'] = array(
    '#type' => 'select',
    '#title' => t('Role for Group'),
    '#default_value' => variable_get('zimbra_group_role_id', 8),
    '#options' => $roles,
    '#weight' => -1,
  );  
  
  $form = system_settings_form($form);
  return $form;
}

/**
 * Zimbra add content page
 */
function zimbra_page_addcontent() {
  $type = arg(3);
  
  if (empty($type)) {
    $item = menu_get_item();
    $content = system_admin_menu_block($item);

    if (count($content) == 1) {
      $item = array_shift($content);
      drupal_goto($item['href']);
    }
    return theme('node_add_list', array('content' => $content));
  }
  else {
    drupal_goto('node/add/' . $type);
    return '';
  }
}

/**
 * Zimbra content page
 */
function zimbra_page_content() {
  $output = '';
  $additem = 0;
  if(arg(0) == 'admin') {
    $additem = 1;
  }
  $output = page_content_general($additem);

  return $output;
}

/**
 * General content page
 */
function page_content_general($add_item = 1) {
  global $user;
  $output = '';
  $menu_items = array();
  if (user_access('create server content')) {
    $menu_items[] = l(t('Zimbra @type', array('@type' => t('servers'))), 'zimbra/server');
  }
  if (user_access('create tenant content')) {
    $menu_items[] = l(t('Zimbra @type', array('@type' => t('tenants'))), 'zimbra/tenant');
  }
  if (user_access('create domain content')) {
    $menu_items[] = l(t('Zimbra @type', array('@type' => t('domains'))), 'zimbra/domain');
  }
  if (user_access('create group content')) {
    $menu_items[] = l(t('Zimbra @type', array('@type' => t('groups'))), 'zimbra/group');
  }
  if (user_access('create mailbox content')) {
    $menu_items[] = l(t('Zimbra @type', array('@type' => t('mailboxes'))), 'zimbra/mailbox');
  }
  if (user_access('create alias content')) {
    $menu_items[] = l(t('Zimbra @type', array('@type' => t('aliases'))), 'zimbra/alias');
  }
  
  if ($add_item == 1) {
    if(user_access('create server content')) {
      $menu_items[] = l(t('Add new Server'), 'node/add/server', array('query' => array('destination' => 'zimbra/server')));
    }
    if(user_access('create tenant content')) {
      $menu_items[] = l(t('Add new Tenant'), 'node/add/tenant', array('query' => array('destination' => 'zimbra/tenant')));
    }
    if(user_access('create domain content')) {
      $menu_items[] = l(t('Add new Domain'), 'node/add/domain', array('query' => array('destination' => 'zimbra/domain')));
    }
    if(user_access('create group content')) {
      $menu_items[] = l(t('Add new Group'), 'node/add/group', array('query' => array('destination' => 'zimbra/group')));
    }
    if(user_access('create mailbox content')) {
      $menu_items[] = l(t('Add new Mailbox'),'node/add/mailbox', array('query' => array('destination' => 'zimbra/mailbox')));
    }
    if(user_access('create alias content')) {
      $menu_items[] = l(t('Add new Alias'), 'node/add/alias', array('query' => array('destination' => 'zimbra/alias')));
    }
  }   

  foreach ($menu_items as $key => $value) {
    $output .= '<li class="item mnu-item-' . $key . '">' . $value . '</li>';
  }   
  if($output !='') {
    $output = '<ul class="zimbra-general-page">' . $output . '</ul>';
  }
  return $output; 
}
