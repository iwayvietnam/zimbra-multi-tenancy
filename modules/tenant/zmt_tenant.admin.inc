<?php

/**
 * List all zimbra tenants page callback.
 */
function zmt_tenant_list_all() {
  global $user;

  backdrop_set_title(t('Tenants'));
  $header = array();
  $header[] = array('data' => t('No.'));
  $header[] = array('data' => t('Full name'), 'field' => 'zt.full_name');
  $header[] = array('data' => t('User name'), 'field' => 'u.name');
  $header[] = array('data' => t('Status'), 'field' => 'u.status');
  $header[] = array('data' => t('Total domains'));
  $header[] = array('data' => t('Total accounts'));
  $header[] = array('data' => t('Server'), 'field' => 'zs.name');
  $header[] = array('data' => t('Operations'));

  $query = db_select('zmt_tenant', 'zt')->extend('PagerDefault')->extend('TableSort');
  $query->join('users', 'u', 'u.uid = zt.tenant_uid');
  $query->join('zmt_server', 'zs', 'zs.id = zt.server_id');

  if (!(user_access('administer zmt') || user_access('administer any zmt_tenant'))) {
    $query->condition('t.uid', $user->uid);
  }
  $server_id = isset($_SESSION['zmt_filter_server_id']) ? (int) $_SESSION['zmt_filter_server_id'] : 0;
  if ($server_id) {
    $query->condition('zt.server_id', $server_id);
  }

  $destination = backdrop_get_destination();
  $item_per_page = (int) config_get('zmt.settings', 'zimbra_item_per_page');
  $ids = $query
    ->fields('zt', array('id'))
    ->orderByHeader($header)
    ->limit($item_per_page)
    ->execute()
    ->fetchCol();
  $tenants = zmt_tenant_load_multiple($ids);

  $key = 1;
  $rows = array();
  foreach ($tenants as $tenant) {
    $row = array();
    $row[] = $key + (pager_find_page() * $item_per_page);
    $row[] = l($tenant->full_name, 'zmt/tenant/' . $tenant->id . '/edit', array('query' => $destination));
    $row[] = array(
      'data' => $tenant->name,
    );
    $row[] = array(
      'data' => ((int) $tenant->status > 0) ? t('Active') : t('Blocked'),
    );
    $row[] = array(
      'data' => !empty($tenant->total_domain) ? (int) $tenant->total_domain : 0,
    );
    $row[] = array(
      'data' => !empty($tenant->total_account) ? (int) $tenant->total_account : 0,
    );
    $row[] = array(
      'data' => $tenant->server,
    );
    $row[] = implode('&nbsp;&nbsp;&nbsp;', array(
      l(t('edit'), 'zmt/tenant/' . $tenant->id . '/edit', array('query' => $destination)),
      l(t('delete'), 'zmt/tenant/' . $tenant->id . '/delete', array('query' => $destination)),
    ));

    $rows[] = $row;
    $key++;
  }

  $build = array();
  if (user_access('administer zmt')) {
    $build['tenants_filter'] = backdrop_get_form('zmt_filter_by_server_form');
  }
  $build['tenants_table']  = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('There are no tenants available.') . ' ' . l(t('Add one now.'), 'zmt/tenant/add'),
  );
  $build['tenants_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Add new zimbra tenant page callback.
 */
function zmt_tenant_add_page() {
  backdrop_set_title(t('Add Tenant'));
  $tenant = zmt_tenant_new();
  return backdrop_get_form('zmt_tenant_edit_form', $tenant);
}

/**
 * Edit exists zimbra tenant page callback.
 */
function zmt_tenant_edit_page($tenant) {
  backdrop_set_title(t('Edit Tenant'));
  return backdrop_get_form('zmt_tenant_edit_form', $tenant);
}

/**
 * Tenant edit form.
 */
function zmt_tenant_edit_form($form, &$form_state, $tenant){
  $form_state['tenant'] = $tenant;
  $account = user_load($tenant->tenant_uid);
  $form_state['user'] = ($account && (int) $account->uid > 0) ? $account : backdrop_anonymous_user();
  $langcode = entity_language('user', $form_state['user']);
  field_attach_form('user', $form_state['user'], $form, $form_state, $langcode);

  $id = isset($tenant->id) ? (int) $tenant->id : 0;

  $form['full_name'] = array(
    '#title' => t('Full name'),
    '#type' => 'textfield',
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $tenant->full_name,
  );
  $form['name'] = array(
    '#title' => t('User name'),
    '#type' => 'textfield',
    '#maxlength' => USERNAME_MAX_LENGTH,
    '#attributes' => array('class' => array('username')),
    '#required' => TRUE,
    '#default_value' => ($account) ? $account->name : '',
  );
  $form['mail'] = array(
    '#title' => t('Email'),
    '#type' => 'textfield',
    '#maxlength' => EMAIL_MAX_LENGTH,
    '#required' => TRUE,
    '#default_value' => ($account) ? $account->mail : '',
  );
  $form['pass'] = array(
    '#type' => 'password_confirm',
    '#required' => ($account) ? !((bool) $account->uid) : TRUE,
    '#size' => 30,
  );
  $form['status'] = array(
    '#type' => 'radios',
    '#title' => t('Status'),
    '#options' => array(t('Blocked'), t('Active')),
    '#default_value' => ($account && (int) $account->uid > 0) ? $account->status : 1,
  );
  if (empty($tenant->server_id)) {
    $form['server_id'] = array(
      '#type' => 'zmt_server_select',
      '#default_value' => $tenant->server_id,
      '#required' => TRUE,
      '#ajax' => array(
        'callback' => 'zmt_tenant_ajax_server_select_callback',
        'wrapper' => 'zmt-cos-wrap',
      ),
    );
  }
  else {
    $form['server_id'] = array(
      '#type' => 'select',
      '#options' => array($tenant->server_id => $tenant->server),
      '#default_value' => $tenant->server_id,
    );
  }

  $server_id = isset($form_state['values']['server_id']) ? (int) $form_state['values']['server_id'] : (int) $tenant->server_id;
  $form['cos'] = array(
    '#type' => 'fieldset',
    '#title' => t('Class of Service'),
    '#prefix' => '<div id="zmt-cos-wrap">',
    '#suffix' => '</div>',
  );
  if ($server_id) {
    $options = array();
    $services = zmt_cos_load_multiple_by_server($server_id);
    foreach ($services as $service) {
      $options[$service->id] = $service->name;
    }
    $mappings = zmt_tenant_cos_load_multiple_by_tenant(array($id));
    $default_value = array();
    $max_accounts = array();
    foreach ($mappings as $tenant_cos) {
      $default_value[] = $tenant_cos->cos_id;
      $max_accounts[$tenant_cos->cos_id] = (int) $tenant_cos->max_account;
    }
    $form['cos']['cos_ids'] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => $default_value,
      '#ajax' => array(
        'callback' => 'zmt_tenant_ajax_cos_select_callback',
        'wrapper' => 'zmt-cos-wrap',
      ),
    );
    $cos_ids = !empty($form_state['values']['cos_ids']) ? $form_state['values']['cos_ids'] : $default_value;
    $ids = array();
    foreach ($cos_ids as $cos_id) {
      if($cos_id) {
        $ids[] = $cos_id;
      }
    }
    $coses = zmt_cos_load_multiple($ids);
    foreach ($coses as $cos) {
      if ($server_id === (int) $cos->server_id) {
        $max_account = isset($max_accounts[$cos->id]) ? $max_accounts[$cos->id] : (int) $cos->max_account;
        $form['cos']['max_account_' . $cos->id] = array(
          '#type' => 'textfield',
          '#title' => t('Max account number for @cos', array('@cos' => $cos->name)),
          '#default_value' => $max_account,
        );
      }
    }
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('zmt_tenant_edit_form_submit'),
    '#suffix' => l(t('Cancel'), 'zmt/tenant'),
  );

  $form['#validate'][] = 'zmt_tenant_edit_form_validate';
  return $form;
}

/**
 * Tenant ajax server selection callback.
 */
function zmt_tenant_ajax_server_select_callback($form, $form_state){
  return $form['cos'];
}

function zmt_tenant_ajax_cos_select_callback($form, $form_state) {
  return $form['cos'];
}

/**
 * Form validation handler for zmt_tenant_edit_form().
 *
 * @see zmt_tenant_edit_form()
 */
function zmt_tenant_edit_form_validate($form, &$form_state) {
  $account = $form_state['user'];
  $values = &$form_state['values'];

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
  form_set_value($form['mail'], $mail, $form_state);
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

/**
 * Tenant edit form submit callback.
 */
function zmt_tenant_edit_form_submit($form, &$form_state) {
  $tenant = $form_state['tenant'];
  $account = $form_state['user'];
  $values = &$form_state['values'];

  $edit = array(
    'name' => check_plain($values['name']),
    'mail' => check_plain($values['mail']),
    'status' => (int) $values['status'],
    'init' => 'email address',
  );
  if (isset($values['pass']) && !empty($values['pass'])) {
    $edit['pass'] = trim($values['pass']);
  }

  $tenant_role = config_get('zmt.settings', 'zmt_tenant_role');
  if ($tenant_role) {
    $edit['roles'] = backdrop_map_assoc(array($tenant_role));
  }
  $account = user_save($account, $edit);

  $tenant->full_name = check_plain($values['full_name']);
  $tenant->tenant_uid = $account->uid;
  $tenant->server_id = (int) $values['server_id'];
  zmt_tenant_save($tenant);

  $mappings = zmt_tenant_cos_load_multiple_by_tenant(array($tenant->id));
  $available_ids = array();
  $selected_ids = array_values($values['cos_ids']);

  foreach ($mappings as $tenant_cos) {
    $cos_id = (int) $tenant_cos->cos_id;
    $available_ids[] = $cos_id;
    if (!in_array($cos_id, $selected_ids)) {
      zmt_tenant_cos_delete($tenant_cos->id);
    }
  }
  foreach ($selected_ids as $cos_id) {
    if ((int) $cos_id > 0 && !in_array($cos_id, $available_ids)) {
      $tenant_cos = zmt_tenant_cos_new();
      $tenant_cos->tenant_id = $tenant->id;
      $tenant_cos->cos_id = $cos_id;
    }
    else {
      $tenant_cos = zmt_tenant_cos_load_single($tenant->id, $cos_id);
    }
    if ($tenant_cos) {
      $max_account = isset($values['max_account_' . $cos_id]) ? (int) $values['max_account_' . $cos_id] : 0;
      $tenant_cos->max_account = abs($max_account);
      zmt_tenant_cos_save($tenant_cos);
    }
  }

  $form_state['redirect'] = 'zmt/tenant';
}

/**
 * Delete exists zimbra tenant page callback.
 */
function zmt_tenant_delete_page($tenant) {
  backdrop_set_title(t('Delete Tenant'));
  return backdrop_get_form('zmt_tenant_delete_form', $tenant);
}

/**
 * Form callback: confirmation form for deleting a tenant.
 *
 * @param $tenant
 *   The tenant object to be deleted.
 *
 * @see confirm_form()
 */
function zmt_tenant_delete_form($form, &$form_state, $tenant) {
  $form_state['tenant'] = $tenant;
  $user = user_load($tenant->tenant_uid);

  $form['#submit'][] = 'zmt_tenant_delete_form_submit';
  $form['#submit'][] = 'zmt_tenant_delete_form_validate';

  $form = confirm_form($form,
    t('Are you sure you want to delete tenant %name?', array('%name' => $user->name)),
    '',
    '<p>' . t('Deleting this tenant cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate callback for zmt_tenant_delete_form().
 */
function zmt_tenant_delete_form_validate($form, &$form_state) {
  $tenant = $form_state['tenant'];
  $total_domain = !empty($tenant->total_domain) ? (int) $tenant->total_domain : 0;
  if ($total_domain) {
    form_set_error('actions', t('Please delete all domains of this tenant before delete this tenant.'));
  }
}

/**
 * Submit callback for zmt_tenant_delete_form().
 */
function zmt_tenant_delete_form_submit($form, &$form_state) {
  $tenant = $form_state['tenant'];
  $user = user_load($tenant->tenant_uid);

  zmt_tenant_delete($tenant->id);
  backdrop_set_message(t('%name has been deleted.', array('%name' => $user->name)));
  watchdog('zmt_tenant', 'Deleted zimbra tenant %name.', array('%name' => $user->name), WATCHDOG_NOTICE);
}
