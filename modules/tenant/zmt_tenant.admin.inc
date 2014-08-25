<?php

/**
 * List all zimbra tenants page callback.
 */
function zmt_tenant_list_all() {
  global $user;

  drupal_set_title('Tenants');
  $header = array();
  $header[] = array('data' => t('No.'));
  $header[] = array('data' => t('Name'), 'field' => 'u.name');
  $header[] = array('data' => t('Status'), 'field' => 'u.status');
  if (module_exists('zmt_domain')) {
    $header[] = array('data' => t('Total domains'), 'field' => 'd.total_domain');
  }
  if (module_exists('zmt_account')) {
    $header[] = array('data' => t('Total accounts'), 'field' => 'a.total_account');
  }
  $header[] = array('data' => t('Server'), 'field' => 's.name');
  $header[] = array('data' => t('COS'), 'field' => 'c.name');

  $header[] = array('data' => t('Operations'));
  $query = db_select('zmt_tenant', 't')->extend('TableSort');
  $query->leftJoin('users', 'u', 'u.uid = t.tenant_uid');
  $query->leftJoin('zmt_server', 's', 's.id = t.server_id');
  $query->leftJoin('zmt_cos', 'c', 'c.id = t.cos_id');

  $query->addTag('tenant_count_account')
    ->addTag('tenant_count_domain');

  $query->addField('u', 'name', 'name');
  $query->addField('u', 'status', 'status');
  $query->addField('s', 'name', 'server');
  $query->addField('c', 'name', 'cos');

  if (!(user_access('administer zmt') || user_access('administer any zmt_tenant'))) {
    $query->condition('t.uid', $user->uid);
  }
  $server_id = isset($_SESSION['zmt_filter_server_id']) ? (int) $_SESSION['zmt_filter_server_id'] : 0;
  if ($server_id) {
    $query->condition('t.server_id', $server_id);
  }

  $item_per_page = (int) variable_get('zimbra_item_per_page', 0);
  if ($item_per_page) {
    $query->extend('PagerDefault')->limit($item_per_page);
  }
  $result = $query
    ->fields('t', array('id'))
    ->orderByHeader($header)
    ->execute();

  $destination = drupal_get_destination();

  $rows = array();
  foreach ($result as $key => $tenant) {
    $row = array();
    $row[] = $key + 1 + (pager_find_page() * $item_per_page);
    $row[] = l($tenant->name, 'zmt/tenant/' . $tenant->id . '/edit', array('query' => $destination));
    $row[] = array(
      'data' => ((int) $tenant->status > 0) ? t('Active') : t('Blocked'),
      'id' => $tenant->status,
    );
    if (module_exists('zmt_domain')) {
      $total_domain = isset($tenant->total_domain) ? $tenant->total_domain : 0;
      $row[] = array(
        'data' => $total_domain,
        'id' => $total_domain,
      );
    }
    if (module_exists('zmt_account')) {
      $total_account = isset($tenant->total_account) ? $tenant->total_account : 0;
      $row[] = array(
        'data' => $total_account,
        'id' => $total_account,
      );
    }
    $row[] = array(
      'data' => $tenant->server,
      'id' => $tenant->server,
    );
    $row[] = array(
      'data' => $tenant->cos,
      'id' => $tenant->cos,
    );

    $operations = l(t('edit'), 'zmt/tenant/' . $tenant->id . '/edit', array('query' => $destination));
    $operations .= '&nbsp;&nbsp;&nbsp;' . l(t('delete'), 'zmt/tenant/' . $tenant->id . '/delete', array('query' => $destination));
    $row[] = $operations;
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('There are no tenants available.') . ' ' . l(t('Add one now'), 'zmt/tenant/add') . '.',
        'colspan' => count($header)
      ),
    );
  }

  $build = array();
  if (user_access('administer zmt')) {
    $build['tenants_filter'] = drupal_get_form('zmt_filter_by_server_form');
  }
  $build['tenants_table']  = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['tenants_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Add new zimbra tenant page callback.
 */
function zmt_tenant_add_page() {
  drupal_set_title('Add Tenant');
  $tenant = zmt_tenant_new();
  return drupal_get_form('zmt_tenant_edit_form', $tenant);
}

/**
 * Edit exists zimbra tenant page callback.
 */
function zmt_tenant_edit_page($tenant) {
  drupal_set_title('Edit Tenant');
  return drupal_get_form('zmt_tenant_edit_form', $tenant);
}

/**
 * Tenant edit form.
 */
function zmt_tenant_edit_form($form, &$form_state, $tenant){
  $form_state['#tenant'] = $tenant;
  $account = user_load($tenant->tenant_uid);
  $form_state['#user'] = ($account && (int) $account->uid > 0) ? $account : drupal_anonymous_user();
  $langcode = entity_language('user', $form_state['#user']);
  field_attach_form('user', $form_state['#user'], $form, $form_state, $langcode);

  $id = isset($tenant->id) ? (int) $tenant->id : 0;

  $form['name'] = array(
    '#title' => t('Tenant Name'),
    '#type' => 'textfield',
    '#maxlength' => USERNAME_MAX_LENGTH,
    '#attributes' => array('class' => array('username')),
    '#required' => TRUE,
    '#default_value' => ($account) ? $account->name : '',
  );
  $form['mail'] = array(
    '#title' => t('Tenant Email'),
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

  $form['server_id'] = array(
    '#type' => 'zmt_server_select',
    '#default_value' => $tenant->server_id,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'ajax_tenant_server_select_callback',
      'wrapper' => 'zmt-cos-dropdown-wrap',
    ),
  );

  $server_id = isset($form_state['values']['server_id']) ? $form_state['values']['server_id'] : $tenant->server_id;
  $form['cos_id'] = array(
    '#type' => 'zmt_cos_select',
    '#server' => $server_id,
    '#default_value' => $tenant->cos_id,
    '#required' => TRUE,
    '#prefix' => '<div id="zmt-cos-dropdown-wrap">',
    '#suffix' => '</div>',
  );

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
function ajax_tenant_server_select_callback($form, $form_state){
  return $form['cos_id'];
}

/**
 * Form validation handler for zmt_tenant_edit_form().
 *
 * @see zmt_tenant_edit_form()
 */
function zmt_tenant_edit_form_validate($form, &$form_state) {
  $account = $form_state['#user'];
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
  $tenant = $form_state['#tenant'];
  $account = $form_state['#user'];
  $values = &$form_state['values'];

  $edit = array(
    'name' => $values['name'],
    'mail' => $values['mail'],
    'status' => (int) $values['status'],
    'init' => 'email address',
  );
  if (isset($values['pass']) && !empty($values['pass'])) {
    $edit['pass'] = trim($values['pass']);
  }
  $role_id = (int) variable_get('zmt_tenant_role_id', 0);
  if ($role_id) {
    $edit['roles'] = drupal_map_assoc(array($role_id));
  }
  $account = user_save($account, $edit);

  $tenant->tenant_uid = $account->uid;
  $tenant->server_id = (int) $values['server_id'];
  $tenant->cos_id = (int) $values['cos_id'];
  zmt_tenant_save($tenant);

  $form_state['redirect'] = 'zmt/tenant';
}

/**
 * Delete exists zimbra tenant page callback.
 */
function zmt_tenant_delete_page($tenant) {
  drupal_set_title('Delete Tenant');
  return drupal_get_form('zmt_tenant_delete_form', $tenant);
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
  $form_state['#tenant'] = $tenant;
  $user = user_load($tenant->tenant_uid);

  $form['#submit'][] = 'zmt_tenant_delete_form_submit';

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
 * Submit callback for zmt_tenant_delete_form().
 */
function zmt_tenant_delete_form_submit($form, &$form_state) {
  $tenant = $form_state['#tenant'];
  $user = user_load($tenant->tenant_uid);

  zmt_tenant_delete($tenant->id);
  drupal_set_message(t('%name has been deleted.', array('%name' => $user->name)));
  watchdog('zmt_tenant', 'Deleted zimbra tenant %name.', array('%name' => $user->name), WATCHDOG_NOTICE);
}

/**
 * Tenant attach form to user.
 */
function zmt_tenant_attach_form(&$form, &$form_state) {
  form_load_include($form_state, 'inc', 'zmt_tenant', 'zmt_tenant.admin');
  $tenant_rid = (int) variable_get('zmt_tenant_role_id', 0);
  $form['account']['roles']['#ajax'] = array(
    'callback' => 'ajax_account_role_callback',
    'wrapper' => 'zmt-tenant-wrap',
  );

  $none = array('' => t('- None -'));

  $form['tenant'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tenant'),
    '#tree' => TRUE,
    '#prefix' => '<div id="zmt-tenant-wrap">',
    '#suffix' => '</div>',
  );
  $roles = isset($form_state['values']['roles']) ? array_values($form_state['values']['roles']) : array();
  if ($tenant_rid && in_array((string) $tenant_rid, $roles, TRUE)) {
    $form['tenant']['server_id'] = array(
      '#type' => 'zmt_server_select',
      '#required' => TRUE,
      '#ajax' => array(
        'callback' => 'ajax_tenant_server_callback',
        'wrapper' => 'zmt-cos-dropdown-wrap',
      ),
    );

    $server_id = isset($form_state['values']['tenant']['server_id']) ? $form_state['values']['tenant']['server_id'] : 0;
    $form['tenant']['cos_id'] = array(
      '#type' => 'zmt_cos_select',
      '#server' => $server_id,
      '#required' => TRUE,
      '#prefix' => '<div id="zmt-cos-dropdown-wrap">',
      '#suffix' => '</div>',
    );
  }
  $form['#submit'][] = 'zmt_tenant_form_submit_callback';
}

/**
 * Tenant ajax account role selection callback.
 */
function ajax_account_role_callback($form, $form_state) {
  return $form['tenant'];
}

/**
 * Tenant ajax server selection callback.
 */
function ajax_tenant_server_callback($form, $form_state){
  return $form['tenant']['cos_id'];
}

/**
 * Submit callback for zmt_tenant_attach_form().
 */
function zmt_tenant_form_submit_callback($form, &$form_state) {
  $values = $form_state['values'];
  $server_id = isset($values['tenant']['server_id']) ? (int) $values['tenant']['server_id'] : 0;
  $cos_id = isset($values['tenant']['cos_id']) ? (int) $values['tenant']['cos_id'] : 0;

  if ($server_id && $cos_id) {
    $tenant = zmt_tenant_new();
    $tenant->tenant_uid = $values['uid'];
    $tenant->server_id = isset($values['tenant']['server_id']) ? (int) $values['tenant']['server_id'] : 0;
    $tenant->cos_id = isset($values['tenant']['cos_id']) ? (int) $values['tenant']['cos_id'] : 0;
    zmt_tenant_save($tenant);
  }
}
