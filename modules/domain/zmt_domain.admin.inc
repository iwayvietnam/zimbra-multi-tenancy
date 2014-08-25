<?php

/**
 * List all zimbra domains page callback.
 */
function zmt_domain_list_all() {
  global $user;

  drupal_set_title('Domains');
  $header = array();
  $header[] = array('data' => t('No.'));
  $header[] = array('data' => t('Name'), 'field' => 'd.name');
  $header[] = array('data' => t('Max account number'), 'field' => 'd.account_max_number');
  if (module_exists('zmt_account')) {
    $header[] = array('data' => t('Total account'), 'field' => 'a.total_account');
  }
  $header[] = array('data' => t('Mailbox quota'), 'field' => 'c.mail_quota');
  $header[] = array('data' => t('Tenant'), 'field' => 'u.name');

  $header[] = array('data' => t('Operations'));
  $query = db_select('zmt_domain', 'd')->extend('PagerDefault')->extend('TableSort');
  $query->leftJoin('zmt_cos', 'c', 'c.id = d.cos_id');
  $query->leftJoin('zmt_tenant', 't', 't.id = d.tenant_id');
  $query->leftJoin('users', 'u', 'u.uid = t.tenant_uid');

  $query->addTag('domain_count_account');

  $query->addField('c', 'mail_quota', 'mail_quota');
  $query->addField('u', 'name', 'tenant');

  if (!(user_access('administer zmt'))) {
    if (user_access('administer any zmt_domain')) {
      $query->condition('u.uid', $user->uid);
    }
    else {
      $query->condition('d.uid', $user->uid);
    }
  }
  $server_id = isset($_SESSION['zmt_filter_server_id']) ? (int) $_SESSION['zmt_filter_server_id'] : 0;
  if ($server_id) {
    $query->condition('d.server_id', $server_id);
  }

  $item_per_page = variable_get('zimbra_item_per_page', 20);
  $result = $query
    ->fields('d')
    ->orderByHeader($header)
    ->limit($item_per_page)
    ->execute();

  $destination = drupal_get_destination();

  $rows = array();
  foreach ($result as $key => $domain) {
    $row = array();
    $row[] = $key + 1 + (pager_find_page() * $item_per_page);
    $row[] = l($domain->name, 'zmt/domain/' . $domain->id . '/edit', array('query' => $destination));
    $row[] = array(
      'data' => $domain->account_max_number,
      'id' => $domain->account_max_number,
    );
    if (module_exists('zmt_account')) {
      $total_account = isset($domain->total_account) ? $domain->total_account : 0;
      $row[] = array(
        'data' => $total_account,
        'id' => $total_account,
      );
    }
    $row[] = array(
      'data' => $domain->mail_quota,
      'id' => $domain->mail_quota,
    );
    $row[] = array(
      'data' => $domain->tenant,
      'id' => $domain->tenant,
    );

    $operations = l(t('edit'), 'zmt/domain/' . $domain->id . '/edit', array('query' => $destination));
    $operations .= '&nbsp;&nbsp;&nbsp;' . l(t('delete'), 'zmt/domain/' . $domain->id . '/delete', array('query' => $destination));
    $row[] = $operations;
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('There are no domains available.') . ' ' . l(t('Add one now'), 'zmt/domain/add') . '.',
        'colspan' => count($header)
      ),
    );
  }

  $build = array();
  if (user_access('administer zmt')) {
    $build['domains_filter'] = drupal_get_form('zmt_filter_by_server_form');
  }
  $build['domains_table']  = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['domains_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Add new zimbra domain page callback.
 */
function zmt_domain_add_page() {
  drupal_set_title('Add Domain');
  $domain = zmt_domain_new();
  return drupal_get_form('zmt_domain_edit_form', $domain);
}

/**
 * Edit exists zimbra domain page callback.
 */
function zmt_domain_edit_page($domain) {
  drupal_set_title('Edit Domain');
  return drupal_get_form('zmt_domain_edit_form', $domain);
}

/**
 * Zimbra domain edit form.
 */
function zmt_domain_edit_form($form, &$form_state, $domain){
  global $user;

  $form_state['#domain'] = $domain;
  $account = user_load($domain->domain_uid);
  $form_state['#user'] = ($account && (int) $account->uid > 0) ? $account : drupal_anonymous_user();
  $values = &$form_state['values'];
  $domain_id = isset($domain->id) ? (int) $domain->id : 0;

  $none = array('' => t('- None -'));

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

  $tenant = zmt_tenant_load_by_user($user);
  if ((int) $domain->server_id == 0 && !$tenant) {
    $form['server_id'] = array(
      '#type' => 'zmt_server_select',
      '#default_value' => $domain->server_id,
      '#ajax' => array(
        'callback' => 'ajax_domain_server_select_callback',
        'wrapper' => 'zmt-tenant-dropdown-wrap',
      ),
    );
  }

  $options = array();
  if (!$tenant) {
    $server_id = isset($values['server_id']) ? (int) $values['server_id'] : (int) $domain->server_id;

    $tenants = zmt_tenant_load_multiple_by_server($server_id);
    foreach ($tenants as $tenant) {
      $options[$tenant->id] = $tenant->name;
    }
    if ((int) $domain->tenant_id == 0) {
      $options = $none + $options;
    }
  }
  else {
    $options[$tenant->id] = $user->name;
  }
  $form['tenant_id'] = array(
    '#type' => 'select',
    '#title' => t('Tenant'),
    '#options' => $options,
    '#default_value' => $domain->tenant_id,
    '#required' => TRUE,
    '#prefix' => '<div id="zmt-tenant-dropdown-wrap">',
    '#suffix' => '</div>',
  );

  $form['account_max_number'] = array(
    '#title' => t('Max account number'),
    '#type' => 'textfield',
    '#default_value' => $domain->account_max_number,
    '#element_validate' => array(
      'element_validate_integer_positive',
      'zmt_domain_account_max_number_element_validate',
    ),
  );

  $have_user = (int) ($account && (int) $account->uid > 0);
  if (!$have_user) {
    $form['create_user'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create user'),
      '#ajax' => array(
        'callback' => 'ajax_create_user_checked',
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
function ajax_create_user_checked($form, &$form_state) {
  return $form['domain_user'];
}

/**
 * Domain ajax server selection callback.
 */
function ajax_domain_server_select_callback($form, $form_state){
  return $form['tenant_id'];
}

/**
 * Form validation handler for zmt_domain_edit_form().
 *
 * @see zmt_domain_edit_form()
 */
function zmt_domain_edit_form_validate($form, &$form_state) {
  $account = $form_state['#user'];
  $domain = $form_state['#domain'];
  $values = &$form_state['values'];

  $server_id = isset($values['server_id']) ? (int) $values['server_id'] : 0;
  $name = trim($values['domain_name']) ;
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
function zmt_domain_account_max_number_element_validate($element, &$form_state, $form) {
  $domain = $form_state['#domain'];
  $id = isset($domain->id) ? (int) $domain->id : 0;
  $value = abs((int) $element['#value']);

  if ($id) {
    $total_account = zmt_domain_count_accounts($id);
    if ($value < $total_account) {
      form_error($element, t('Max account number must be greater than total account number.'));
    }
  }

  $tenant_id = (int) $form_state['values']['tenant_id'];
  $tenant = zmt_tenant_load($tenant_id);
  if ($tenant) {
    $remain_number = zmt_tenant_account_number_remain($tenant_id, $id);
    if ($value > $remain_number) {
      form_error($element, t('The domain has reached the maximum number of accounts.'));
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
  $domain = $form_state['#domain'];
  $account = $form_state['#user'];
  $values = &$form_state['values'];

  $create_user = isset($values['create_user']) ? (int) $values['create_user'] : 0;
  if ($create_user) {
    $edit = array(
      'name' => trim($values['name']),
      'mail' => trim($values['mail']),
      'status' => (int) $values['status'],
      'init' => 'email address',
    );
    if (isset($values['pass']) && !empty($values['pass'])) {
      $edit['pass'] = trim($values['pass']);
    }
    $role_id = (int) variable_get('zmt_domain_role_id', 0);
    if ($role_id) {
      $edit['roles'] = drupal_map_assoc(array($role_id));
    }
    $account = user_save($account, $edit);

    $domain->domain_uid = $account->uid;
  }

  $domain->name = $values['domain_name'];
  $domain->account_max_number = abs((int) $values['account_max_number']);
  $tenant_id = (int) $values['tenant_id'];
  $tenant = zmt_tenant_load($tenant_id);
  if ($tenant) {
    $domain->tenant_id = (int) $tenant->id;
    $domain->server_id = (int) $tenant->server_id;
    $domain->cos_id = (int) $tenant->cos_id;
    $tenant_user = user_load($tenant->tenant_uid);
  }

  zmt_domain_save($domain);

  $form_state['redirect'] = 'zmt/domain';
}

/**
 * Delete exists zimbra domain page callback.
 */
function zmt_domain_delete_page($domain) {
  drupal_set_title('Delete Domain');
  return drupal_get_form('zmt_domain_delete_form', $domain);
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
  $form_state['#domain'] = $domain;

  $form['#submit'][] = 'zmt_domain_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete domain %name?', array('%name' => $domain->name)),
    '',
    '<p>' . t('Deleting this domain cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for zmt_domain_delete_form().
 */
function zmt_domain_delete_form_submit($form, &$form_state) {
  $domain = $form_state['#domain'];
  zmt_domain_delete($domain->id);
  drupal_set_message(t('%name has been deleted.', array('%name' => $domain->name)));
  watchdog('zmt_domain', 'Deleted domain %name.', array('%name' => $domain->name), WATCHDOG_NOTICE);
}
