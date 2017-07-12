<?php

/**
 * List all zimbra domains page callback.
 */
function zmt_domain_list_all() {
  global $user;

  backdrop_set_title(t('Domains'));
  $header = array();
  $header[] = array('data' => t('No.'));
  $header[] = array('data' => t('Name'), 'field' => 'zd.name');
  $header[] = array('data' => t('Status'), 'field' => 'zd.status');
  $header[] = array('data' => t('Total account'));
  $header[] = array('data' => t('Tenant'), 'field' => 'u.name');

  $header[] = array('data' => t('Operations'));
  $query = db_select('zmt_domain', 'zd')->extend('PagerDefault')->extend('TableSort');
  $query->leftJoin('zmt_tenant', 'zt', 'zt.id = zd.tenant_id');
  $query->leftJoin('users', 'u', 'u.uid = zt.tenant_uid');

  if (!(user_access('administer zmt'))) {
    if (user_access('administer any zmt_domain')) {
      $query->condition('u.uid', $user->uid);
    }
    else {
      $query->condition('zd.uid', $user->uid);
    }
  }
  $server_id = isset($_SESSION['zmt_filter_server_id']) ? (int) $_SESSION['zmt_filter_server_id'] : 0;
  if ($server_id) {
    $query->condition('zd.server_id', $server_id);
  }

  $destination = backdrop_get_destination();
  $statuses = zmt_domain_statuses();
  $item_per_page = (int) config_get('zmt.settings', 'zimbra_item_per_page');
  $ids = $query
    ->fields('zd', array('id'))
    ->orderByHeader($header)
    ->limit($item_per_page)
    ->execute()
    ->fetchCol();
  $domains = zmt_domain_load_multiple($ids);

  $key = 1;
  $rows = array();
  foreach ($domains as $domain) {
    $row = array();
    $row[] = $key + (pager_find_page() * $item_per_page);
    $row[] = l($domain->name, 'zmt/domain/' . $domain->id . '/edit', array('query' => $destination));
    $row[] = array(
      'data' => !empty($statuses[$domain->status]) ? $statuses[$domain->status] : $domain->status,
    );
    $row[] = array(
      'data' => !empty($domain->total_account) ? (int) $domain->total_account : 0,
    );
    $row[] = array(
      'data' => $domain->tenant,
    );
    $row[] = implode('&nbsp;&nbsp;&nbsp;', array(
      l(t('edit'), 'zmt/domain/' . $domain->id . '/edit', array('query' => $destination)),
      l(t('delete'), 'zmt/domain/' . $domain->id . '/delete', array('query' => $destination)),
    ));

    $rows[] = $row;
    $key++;
  }

  $build = array();
  if (user_access('administer zmt')) {
    $build['domains_filter'] = backdrop_get_form('zmt_filter_by_server_form');
  }
  $build['domains_table']  = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('There are no domains available.') . ' ' . l(t('Add one now.'), 'zmt/domain/add'),
  );
  $build['domains_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Add new zimbra domain page callback.
 */
function zmt_domain_add_page() {
  backdrop_set_title(t('Add Domain'));
  $domain = zmt_domain_new();
  return backdrop_get_form('zmt_domain_edit_form', $domain);
}

/**
 * Edit exists zimbra domain page callback.
 */
function zmt_domain_edit_page($domain) {
  backdrop_set_title(t('Edit Domain'));
  return backdrop_get_form('zmt_domain_edit_form', $domain);
}

/**
 * Zimbra domain edit form.
 */
function zmt_domain_edit_form($form, &$form_state, $domain) {
  global $user;

  $form_state['domain'] = $domain;
  $account = user_load($domain->domain_uid);
  $form_state['user'] = ($account && (int) $account->uid > 0) ? $account : entity_create('user', array());
  $values = &$form_state['values'];
  $domain_id = isset($domain->id) ? (int) $domain->id : 0;

  $form['domain_name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $domain->name,
    '#required' => TRUE,
    '#element_validate' => array('zmt_domain_name_element_validate'),
  );
  if ($domain_id) {
    $form['domain_name']['#attributes']['readonly'] = TRUE;
  }
  $options = zmt_domain_statuses();
  $form['domain_status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#options' => $options,
    '#default_value' => $domain->status,
  );

  $tenant = zmt_tenant_load_by_user($user);
  if ((int) $domain->server_id == 0 && !$tenant) {
    $form['server_id'] = array(
      '#type' => 'zmt_server_select',
      '#default_value' => $domain->server_id,
      '#required' => TRUE,
      '#ajax' => array(
        'callback' => 'zmt_domain_ajax_server_select_callback',
        'wrapper' => 'zmt-tenant-wrap',
      ),
    );
  }

  $server_id = isset($values['server_id']) ? (int) $values['server_id'] : (int) $domain->server_id;
  $options = array();
  if ($tenant) {
    $domain->tenant_id = $tenant->id;
    $options[$tenant->id] = $tenant->name;
  }
  else {
    $tenants = zmt_tenant_load_multiple_by_server($server_id);
    foreach ($tenants as $tenant) {
      $options[$tenant->id] = $tenant->name;
    }
  }
  $form['tenant'] = array(
    '#type' => 'container',
    '#prefix' => '<div id="zmt-tenant-wrap">',
    '#suffix' => '</div>',
  );
  $form['tenant']['tenant_id'] = array(
    '#type' => 'select',
    '#title' => t('Tenant'),
    '#options' => $options,
    '#empty_option' => t('- None -'),
    '#default_value' => $domain->tenant_id,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'zmt_domain_ajax_tenant_select_callback',
      'wrapper' => 'zmt-cos-wrap',
    ),
    '#prefix' => '<div id="zmt-tenant-dropdown-wrap">',
    '#suffix' => '</div>',
  );
  $tenant_id = isset($values['tenant_id']) ? (int) $values['tenant_id'] : (int) $domain->tenant_id;
  $form['tenant']['cos'] = array(
    '#type' => 'fieldset',
    '#title' => t('Class of Service'),
    '#prefix' => '<div id="zmt-cos-wrap">',
    '#suffix' => '</div>',
  );
  if ($tenant_id) {
    $options = array();
    $default_value = array();
    $max_accounts = array();

    $services = zmt_tenant_cos_load_multiple_by_tenant(array($tenant_id));
    foreach ($services as $map) {
      $options[$tenant_id . '|' . $map->cos_id] = $map->cos;
      $max_accounts[$tenant_id . '|' . $map->cos_id] = (int) $map->max_account;
    }
    $mappings = zmt_domain_cos_load_multiple_by_domain(array($domain_id));
    foreach ($mappings as $map) {
      $default_value[] = $tenant_id . '|' . $map->cos_id;
      $max_accounts[$tenant_id . '|' . $map->cos_id] = (int) $map->max_account;
    }
    $form['tenant']['cos']['tenant_cos_ids'] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => $default_value,
      '#ajax' => array(
        'callback' => 'zmt_domain_ajax_cos_select_callback',
        'wrapper' => 'zmt-cos-wrap',
      ),
    );
    $tenant_cos_ids = !empty($form_state['values']['tenant_cos_ids']) ? $form_state['values']['tenant_cos_ids'] : $default_value;
    $cos_ids = array();
    foreach ($tenant_cos_ids as $tenant_cos_id) {
      if($tenant_cos_id) {
        $arr = explode('|', $tenant_cos_id);
        $tid = isset($arr[0]) ? (int) $arr[0] : 0;
        if ($tenant_id === $tid) {
          $cos_ids[] = isset($arr[1]) ? (int) $arr[1] : 0;
        }
      }
    }
    $services = zmt_cos_load_multiple($cos_ids);
    foreach ($services as $cos) {
      $max_account = isset($max_accounts[$tenant_id . '|' . $cos->id]) ? $max_accounts[$tenant_id . '|' . $cos->id] : (int) $cos->max_account;
      $form['tenant']['cos']['max_account_' . $cos->id] = array(
        '#type' => 'textfield',
        '#title' => t('Max account number for @cos', array('@cos' => $cos->name)),
        '#default_value' => $max_account,
        '#element_validate' => array(
          'zmt_domain_max_account_number_element_validate',
        ),
      );
    }
  }

  $have_user = (int) ($account && (int) $account->uid > 0);
  if (!$have_user) {
    $form['create_user'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create user'),
      '#ajax' => array(
        'callback' => 'zmt_domain_ajax_create_user_checked',
        'wrapper' => 'zmt-user-wrap',
      ),
    );
  }

  $form['domain_user'] = array(
    '#type' => 'container',
    '#prefix' => '<div id="zmt-user-wrap">',
    '#suffix' => '</div>',
  );
  $create_user = isset($values['create_user']) ? (int) $values['create_user'] : $have_user;
  if ($create_user) {
    $account_name = isset($values['domain_name']) ? $values['domain_name'] : $account->name;
    $form['domain_user']['name'] = array(
      '#title' => t('User Name'),
      '#type' => 'textfield',
      '#default_value' => $account_name,
      '#required' => TRUE,
    );
    $form['domain_user']['mail'] = array(
      '#title' => t('User Email'),
      '#type' => 'textfield',
      '#default_value' => $account->mail,
      '#required' => TRUE,
      '#default_value' => ($account) ? $account->mail : '',
    );
    $form['domain_user']['pass'] = array(
      '#type' => 'password_confirm',
      '#required' => ($account) ? !((bool) $account->uid) : TRUE,
      '#size' => 30,
    );
    $form['domain_user']['status'] = array(
      '#type' => 'radios',
      '#title' => t('Status'),
      '#options' => array(t('Blocked'), t('Active')),
      '#default_value' => ($account && (int) $account->uid > 0) ? $account->status : 1,
    );
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('zmt_domain_edit_form_submit'),
    '#suffix' => l(t('Cancel'), 'zmt/domain'),
  );

  $form['#validate'][] = 'zmt_domain_edit_form_validate';
  return $form;
}

/**
 * Domain ajax create account check callback.
 */
function zmt_domain_ajax_create_user_checked($form, $form_state) {
  return $form['domain_user'];
}

/**
 * Domain ajax server selection callback.
 */
function zmt_domain_ajax_server_select_callback($form, $form_state) {
  return $form['tenant'];
}

function zmt_domain_ajax_tenant_select_callback($form, $form_state) {
  return $form['tenant']['cos'];
}

function zmt_domain_ajax_cos_select_callback($form, $form_state) {
  return $form['tenant']['cos'];
}

/**
 * Form validation handler for zmt_domain_edit_form().
 *
 * @see zmt_domain_edit_form()
 */
function zmt_domain_edit_form_validate($form, &$form_state) {
  $account = $form_state['user'];
  $domain = $form_state['domain'];
  $values = &$form_state['values'];

  $server_id = isset($values['server_id']) ? (int) $values['server_id'] : 0;
  $name = trim(check_plain($values['domain_name'])) ;
  $id = isset($domain->id) ? (int) $domain->id : 0;

  $exists = zmt_domain_exists($name, $id);
  if ($exists) {
    form_set_error('domain_name', t('The domain already exists in selected server.'));
  }

  $tenant_id = (int) $values['tenant_id'];
  $tenant = zmt_tenant_load($tenant_id);
  if (!$tenant) {
    form_set_error('tenant_id', t('The tenant you selected does not exist.'));
  }

  $create_user = isset($values['create_user']) ? (int) $values['create_user'] : 0;
  if ($create_user && !zmt_is_ajax_request()) {
    // Validate new or changing username.
    if ($error = user_validate_name($values['name'])) {
      form_set_error('name', $error);
    }
    else {
      $exists = db_select('users')
        ->fields('users', array('uid'))
        ->condition('uid', $account->uid, '<>')
        ->condition('name', db_like($values['name']), 'LIKE')
        ->range(0, 1)->execute()->fetchField();
      if ((bool) $exists) {
        form_set_error('name', t('The name %name is already taken.', array('%name' => $values['name'])));
      }
    }

    $mail = trim($values['mail']);
    form_set_value($form['domain_user']['mail'], $mail, $form_state);
    // Validate the e-mail address, and check if it is taken by an existing user.
    if ($error = user_validate_mail($values['mail'])) {
      form_set_error('mail', $error);
    }
    else {
      $exists = db_select('users')
        ->fields('users', array('uid'))
        ->condition('uid', $account->uid, '<>')
        ->condition('mail', db_like($values['mail']), 'LIKE')
        ->range(0, 1)->execute()->fetchField();
      if ((bool) $exists) {
        // Format error message dependent on whether the user is logged in or not.
        if ($GLOBALS['user']->uid) {
          form_set_error('mail', t('The e-mail address %email is already taken.', array('%email' => $values['mail'])));
        }
        else {
          form_set_error('mail', t('The e-mail address %email is already registered. <a href="@password">Have you forgotten your password?</a>', array('%email' => $values['mail'], '@password' => url('user/password'))));
        }
      }
    }
  }
}

/**
 * Account max number validate callback.
 */
function zmt_domain_max_account_number_element_validate($element, &$form_state, $form) {
  $domain = $form_state['domain'];
  $id = isset($domain->id) ? (int) $domain->id : 0;
  $value = abs((int) $element['#value']);
  $title = $element['#title'];

  if ($id) {
    $total_account = !empty($domain->total_account) ? (int) $domain->total_account : 0;
    if ($value < $total_account) {
      form_error($element, t('Max account number must be greater than total account number.'));
    }
  }

  $tenant_id = (int) $form_state['values']['tenant_id'];
  $cos_id = (int) ltrim($element['#name'], 'max_account_');
  $cos = zmt_cos_load($cos_id);
  $tc = zmt_tenant_cos_load_single($tenant_id, $cos_id);
  $max_account = 0;
  if ($tc) {
    $max_account = (int) $tc->max_account;
  }
  elseif ($cos) {
    $max_account = (int) $cos->max_account;
  }
  if ($max_account) {
    if ($value == 0 || $value > $max_account) {
      form_error($element, t('@title must be between @min and @max.', array(
        '@title' => $title,
        '@min' => 1,
        '@max' => $max_account,
      )));
    }
  }
}

/**
 * Domain name validate callback.
 */
function zmt_domain_name_element_validate($element, &$form_state, $form) {
  $value = trim($element['#value']);
  $pattern = '/^(?:[a-zA-Z0-9]+(?:\-*[a-zA-Z0-9])*\.)+[a-zA-Z]{2,6}$/ix';
  if (!empty($value) && !preg_match($pattern, $value)) {
    form_error($element, t('Please enter a valid domain name.'));
  }  
}

/**
 * Zimbra domain edit form submit callback.
 */
function zmt_domain_edit_form_submit($form, &$form_state) {
  $domain = $form_state['domain'];
  $account = $form_state['user'];
  $values = &$form_state['values'];

  $create_user = isset($values['create_user']) ? (int) $values['create_user'] : 0;
  if ($create_user) {
    $account->name = check_plain(trim($values['name']));
    $account->mail = $account->init = check_plain(trim($values['mail']));
    $account->status = (int) $values['status'];
    if (isset($values['pass']) && !empty($values['pass'])) {
      $account->pass = trim($values['pass']);
    }
    $domain_role = config_get('zmt.settings', 'zmt_domain_role');
    if ($domain_role) {
      $account->roles = backdrop_map_assoc(array($domain_role));
    }
    $account->save();
    $domain->domain_uid = $account->uid;
  }

  $domain->name = check_plain($values['domain_name']);
  $domain->status = check_plain($values['domain_status']);
  $tenant_id = (int) $values['tenant_id'];
  $tenant = zmt_tenant_load($tenant_id);
  if ($tenant) {
    $domain->tenant_id = (int) $tenant->id;
    $domain->server_id = (int) $tenant->server_id;
  }
  zmt_domain_save($domain);

  $mappings = zmt_domain_cos_load_multiple_by_domain(array($domain->id));
  $available_ids = array();
  $selected_ids = array();
  $tenant_cos_ids = array_values($values['tenant_cos_ids']);
  foreach ($tenant_cos_ids as $tenant_cos_id) {
    if($tenant_cos_id) {
      $arr = explode('|', $tenant_cos_id);
      $tid = isset($arr[0]) ? (int) $arr[0] : 0;
      if ($tenant_id === $tid) {
        $selected_ids[] = isset($arr[1]) ? (int) $arr[1] : 0;
      }
    }
  }

  foreach ($mappings as $domain_cos) {
    $cos_id = (int) $domain_cos->cos_id;
    $available_ids[] = $cos_id;
    if (!in_array($cos_id, $selected_ids)) {
      zmt_domain_cos_delete($domain_cos->id);
    }
  }
  foreach ($selected_ids as $cos_id) {
    if ((int) $cos_id > 0 && !in_array($cos_id, $available_ids)) {
      $domain_cos = zmt_domain_cos_new();
      $domain_cos->domain_id = $domain->id;
      $domain_cos->cos_id = $cos_id;
    }
    else {
      $domain_cos = zmt_domain_cos_load_single($domain->id, $cos_id);
    }
    if ($domain_cos) {
      $max_account = isset($values['max_account_' . $cos_id]) ? (int) $values['max_account_' . $cos_id] : 0;
      $domain_cos->max_account = abs($max_account);
      zmt_domain_cos_save($domain_cos);
    }
  }

  $form_state['redirect'] = 'zmt/domain';
}

/**
 * Delete exists zimbra domain page callback.
 */
function zmt_domain_delete_page($domain) {
  backdrop_set_title(t('Delete Domain'));
  return backdrop_get_form('zmt_domain_delete_form', $domain);
}

/**
 * Form callback: confirmation form for deleting a domain.
 *
 * @param $domain
 *   The domain object to be deleted.
 *
 * @see confirm_form()
 */
function zmt_domain_delete_form($form, &$form_state, $domain) {
  $form_state['domain'] = $domain;

  $form['#submit'][] = 'zmt_domain_delete_form_submit';
  $form['#validate'][] = 'zmt_domain_delete_form_validate';

  $description = '';
  if ((int) $domain->delete_domain > 0 && !empty($domain->zimbra_id)) {
    $description .= t('Delete this domain also delete it in zimbra server. ');
  }
  $description .= t('This action cannot be undone.');

  $form = confirm_form($form,
    t('Are you sure you want to delete domain %name?', array('%name' => $domain->name)),
    '',
    '<p>' . $description . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate callback for zmt_domain_delete_form().
 */
function zmt_domain_delete_form_validate($form, &$form_state) {
  $domain = $form_state['domain'];
  $total_mailbox = 0;
  if (!empty($domain->total_account)) {
    $total_mailbox += (int) $domain->total_account;
  }
  if (!empty($domain->total_alias)) {
    $total_mailbox += (int) $domain->total_alias;
  }
  if (!empty($domain->total_group)) {
    $total_mailbox += (int) $domain->total_group;
  }
  if ($total_mailbox) {
    form_set_error('actions', t('Please delete all accounts, aliases, group of this domain before delete this domain.'));
  }
}

/**
 * Submit callback for zmt_domain_delete_form().
 */
function zmt_domain_delete_form_submit($form, &$form_state) {
  $domain = $form_state['domain'];
  zmt_domain_delete($domain->id);
  backdrop_set_message(t('%name has been deleted.', array('%name' => $domain->name)));
  watchdog('zmt_domain', 'Deleted domain %name.', array('%name' => $domain->name), WATCHDOG_NOTICE);
}
