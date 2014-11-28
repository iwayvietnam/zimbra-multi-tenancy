<?php

/**
 * List all zimbra accounts page callback.
 */
function zmt_account_list_all() {
  global $user;

  drupal_set_title('Accounts');
  $header = array();
  $header[] = array('data' => t('No.'));
  $header[] = array('data' => t('Name'), 'field' => 'a.name');
  $header[] = array('data' => t('Full name'), 'field' => 'a.full_name');
  $header[] = array('data' => t('COS'), 'field' => 'c.name');
  $header[] = array('data' => t('Status'), 'field' => 'a.status');
  $header[] = array('data' => t('Operations'));

  $query = db_select('zmt_account', 'a')->extend('PagerDefault')->extend('TableSort');
  $query->join('zmt_domain', 'd', 'd.id = a.domain_id');
  $query->leftJoin('zmt_cos', 'c', 'c.id = a.cos_id');
  $query->addField('d', 'name', 'domain');
  $query->addField('c', 'name', 'cos');

  $domain_ids = array();
  if (!(user_access('administer zmt'))) {
    if (user_access('administer any zmt_account')) {
      $domains = zmt_domain_load_multiple_by_user($user);
      $domain_ids = array_keys($domains);
      if (empty($domain_ids)) {
        $domain_ids = array(0);
      }

      $domain_id = isset($_SESSION['zmt_filter_domain_id']) ? (int) $_SESSION['zmt_filter_domain_id'] : 0;
      if ($domain_id) {
        $query->condition('a.domain_id', $domain_id);
      }
      else {
        $query->condition('a.domain_id', $domain_ids);
        $query->addTag('account_query_filter');
      }
    }
    else {
      $query->condition('a.uid', $user->uid);
    }
  }
  else {
    $domain_id = isset($_SESSION['zmt_filter_domain_id']) ? (int) $_SESSION['zmt_filter_domain_id'] : 0;
    if ($domain_id) {
      $query->condition('a.domain_id', $domain_id);
    }
  }

  $item_per_page = variable_get('zimbra_item_per_page', 20);
  $result = $query
    ->fields('a')
    ->orderByHeader($header)
    ->limit($item_per_page)
    ->execute();

  $destination = drupal_get_destination();
  $statuses = zmt_account_statuses();
  $rows = array();
  foreach ($result as $key => $account) {
    $row = array();
    $row[] = $key + 1 + (pager_find_page() * $item_per_page);
    $account_name = $account->name . '@' . $account->domain;
    $row[] = l($account_name, 'zmt/account/' . $account->id . '/edit', array('query' => $destination));
    $row[] = array(
      'data' => $account->full_name,
    );
    $row[] = array(
      'data' => $account->cos,
    );
    $row[] = array(
      'data' => isset($statuses[$account->status]) ? $statuses[$account->status] : $account->status,
    );

    $operations = l(t('edit'), 'zmt/account/' . $account->id . '/edit', array('query' => $destination));
    $operations .= '&nbsp;&nbsp;&nbsp;' . l(t('delete'), 'zmt/account/' . $account->id . '/delete', array('query' => $destination));
    $row[] = $operations;
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('There are no accounts available.') . ' ' . l(t('Add one now'), 'zmt/account/add') . '.',
        'colspan' => count($header)
      ),
    );
  }

  $build = array();
  if (user_access('administer zmt')) {
    $build['account_filter'] = drupal_get_form('zmt_filter_by_domain_form');
  }
  elseif (user_access('administer any zmt_account')) {
    if (count($domain_ids) > 1) {
      $build['account_filter'] = drupal_get_form('zmt_filter_by_domain_form');
    }
  }
  $build['account_table']  = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['account_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Add new zimbra account page callback.
 */
function zmt_account_add_page() {
  drupal_set_title('Add Account');
  $account = zmt_account_new();
  return drupal_get_form('zmt_account_edit_form', $account);
}

/**
 * Edit exists zimbra account page callback.
 */
function zmt_account_edit_page($account) {
  drupal_set_title('Edit Account');
  return drupal_get_form('zmt_account_edit_form', $account);
}

/**
 * Zimbra account edit form.
 */
function zmt_account_edit_form($form, &$form_state, $account){
  global $user;

  $form_state['account'] = $account;

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $account->name,
    '#required' => TRUE,
  );

  $options = array();
  if (isset($account->id) && (int) $account->id > 0) {
    $domain = zmt_domain_load($account->domain_id);
    if ($domain) {
      $options[$domain->id] = $domain->name;
    }
  }
  else {
    if (!(user_access('administer zmt'))) {
      $domains = zmt_domain_load_multiple_by_user($user);
    }
    else {
      $domains = zmt_domain_load_all();
    }
    if (count($domains) !== 1) {
      $options = array('' => t('- None -'));
    }
    foreach ($domains as $domain) {
      $options[$domain->id] = $domain->name;
    }
  }
  $form['domain_id'] = array(
    '#type' => 'select',
    '#title' => t('Domain'),
    '#options' => $options,
    '#default_value' => $account->domain_id,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'zmt_account_ajax_domain_select_callback',
      'wrapper' => 'zmt-cos-wrap',
    ),
  );

  $domain_id = isset($form_state['values']['domain_id']) ? (int) $form_state['values']['domain_id'] : (int) $account->domain_id;
  $options = array();
  if ($domain_id) {
    $mappings = zmt_domain_cos_load_multiple_by_domain(array($domain_id));
    foreach ($mappings as $map) {
      $options[$map->cos_id] = $map->cos;
    }
  }
  $form['cos_id'] = array(
    '#type' => 'select',
    '#title' => t('COS'),
    '#options' => $options,
    '#empty_option' => t('- None -'),
    '#default_value' => $account->cos_id,
    '#required' => TRUE,
    '#prefix' => '<div id="zmt-cos-wrap">',
    '#suffix' => '</div>',
  );

  $form['password'] = array(
    '#type' => 'password_confirm',
    '#required' => !(isset($account->id) && (int) $account->id > 0),
    '#size' => 30,
  );
  $form['full_name'] = array(
    '#title' => t('Full name'),
    '#type' => 'textfield',
    '#default_value' => $account->full_name,
    '#required' => TRUE,
  );
  $options = zmt_account_statuses();
  $form['status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#options' => $options,
    '#default_value' => $account->status,
  );
  $form['title'] = array(
    '#title' => t('Work title'),
    '#type' => 'textfield',
    '#default_value' => $account->title,
  );
  $form['telephone'] = array(
    '#title' => t('Telephone'),
    '#type' => 'textfield',
    '#default_value' => $account->telephone,
  );
  $form['mobile'] = array(
    '#title' => t('Mobile'),
    '#type' => 'textfield',
    '#default_value' => $account->mobile,
  );
  $form['company'] = array(
    '#title' => t('Company'),
    '#type' => 'textfield',
    '#default_value' => $account->company,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('zmt_account_edit_form_submit'),
    '#suffix' => l(t('Cancel'), 'zmt/account'),
  );

  $form['#validate'][] = 'zmt_account_edit_form_validate';

  return $form;
}

/**
 * Zimbra account edit form validate callback.
 */
function zmt_account_edit_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $account = $form_state['account'];

  $domain_id = (int) $values['domain_id'];
  $cos_id = (int) $values['cos_id'];
  $name = trim($values['name']);
  $id = isset($account->id) ? (int) $account->id : 0;

  $results = module_invoke_all('mailbox_exists', $domain_id, $name, $id);
  foreach ($results as $result) {
    if ($result === TRUE) {
      form_set_error('name', t('The mailbox name already exists in selected domain.'));
      break;
    }
  }

  if ($id === 0) {
    $domain_cos = zmt_domain_cos_load_single($domain_id, $cos_id);
    $max_account = !empty($domain_cos->max_account) ? (int) $domain_cos->max_account : 0;
    $total_account = zmt_account_count_by_domain($domain_id);
    if ($max_account && $total_account >= $max_account) {
      form_set_error('name', t('The domain has reached the maximum number of accounts.'));
    }
  }
}

/**
 * Zimbra account ajax domain select callback.
 */
function zmt_account_ajax_domain_select_callback($form, $form_state) {
  return $form['cos_id'];
}

/**
 * Zimbra account edit form submit callback.
 */
function zmt_account_edit_form_submit($form, &$form_state) {
  $account = $form_state['account'];
  $values = $form_state['values'];

  $account->domain_id = (int) $values['domain_id'];
  $account->cos_id = (int) $values['cos_id'];
  $account->name = check_plain(trim($values['name']));
  $account->password = trim($values['password']);
  $account->full_name = $values['full_name'];
  $account->status = check_plain($values['status']);
  $account->title = check_plain($values['title']);
  $account->telephone = check_plain($values['telephone']);
  $account->mobile = check_plain($values['mobile']);
  $account->company = check_plain($values['company']);

  zmt_account_save($account);

  $form_state['redirect'] = 'zmt/account';
}


/**
 * Import accounts page callback.
 */
function zmt_account_import_page() {
  drupal_set_title('Import Accounts');
  return drupal_get_form('zmt_account_import_form');
}

/**
 * Import accounts form
 */
function zmt_account_import_form($form, &$form_state){
  global $user;

  $form['file'] = array(
    '#type' => 'file',
    '#title' => t('Csv file'),
    '#description' => t('Csv file fields: name,password,full_name,status,title,telephone,mobile,company'),
  );

  $options = array();
  if (!(user_access('administer zmt'))) {
    $domains = zmt_domain_load_multiple_by_user($user);
  }
  else {
    $domains = zmt_domain_load_all();
  }
  if (count($domains) !== 1) {
    $options = array('' => t('- None -'));
  }
  foreach ($domains as $domain) {
    $options[$domain->id] = $domain->name;
  }

  $form['domain_id'] = array(
    '#type' => 'select',
    '#title' => t('Domain'),
    '#options' => $options,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'zmt_account_ajax_domain_select_callback',
      'wrapper' => 'zmt-cos-wrap',
    ),
  );

  $domain_id = isset($form_state['values']['domain_id']) ? (int) $form_state['values']['domain_id'] : 0;
  $options = array();
  if ($domain_id) {
    $mappings = zmt_domain_cos_load_multiple_by_domain(array($domain_id));
    foreach ($mappings as $map) {
      $options[$map->cos_id] = $map->cos;
    }
  }
  $form['cos_id'] = array(
    '#type' => 'select',
    '#title' => t('COS'),
    '#options' => $options,
    '#empty_option' => t('- None -'),
    '#required' => TRUE,
    '#prefix' => '<div id="zmt-cos-wrap">',
    '#suffix' => '</div>',
  );

  $form['actions']['import'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
    '#ajax' => array(
      'callback' => 'zmt_account_ajax_import_form_callback',
    ),
    '#suffix' => l(t('Cancel'), 'zmt/account'),
  );
  return $form;
}

/**
 * Import accounts form callback
 */
function zmt_account_ajax_import_form_callback($form, $form_state) {
  $destination = 'public://zmt/';
  if (file_prepare_directory($destination, FILE_CREATE_DIRECTORY)) {
    $file = file_save_upload(
      'file',
      array('file_validate_extensions' => array('csv')),
      $destination
    );
    if ($file) {
      $statuses = zmt_account_statuses();
      $domain_id = (int) $form_state['values']['domain_id'];
      $cos_id = (int) $form_state['values']['cos_id'];

      $handle = fopen($file->uri, 'r');
      $row = fgetcsv($handle);
      $columns = array();
      foreach ($row as $i => $header) {
        $columns[$i] = trim($header);
      }
      $count = 0;
      while ($row = fgetcsv($handle)) {
        $record = array();
        $quota = 0;
        foreach ($row as $i => $field) {
          if ($columns[$i] === 'status' && !in_array($field, $statuses)) {
            $field = 'active';
          }

          $column = $columns[$i];
          if ($column) {
            $record[$column] = $field;
          }
        }
        $account = zmt_account_prepare($record, $domain_id);
        $account->cos_id = $cos_id;
        $results = module_invoke_all('mailbox_exists', $domain_id, $account->name, 0);
        $exists = FALSE;
        foreach ($results as $result) {
          if ($result === TRUE) {
            $exists = $result;
            break;
          }
        }
        if (!$exists) {
          zmt_account_save($account);
          $count++;
        }
      }
      fclose($handle);
      drupal_set_message($count . ' accounts were updated.');
    }
  }
  $commands = array();
  $commands[] = zmt_ajax_command_redirect('zmt/account');
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Prepare account entity
 */
function zmt_account_prepare($record, $domain_id = 0) {
  $account = zmt_account_new();
  $account->domain_id = (int) $domain_id;
  foreach ($record as $field => $value) {
    $account->$field = $value;
  }
  return $account;
}

/**
 * Delete exists zimbra account page callback.
 */
function zmt_account_delete_page($account) {
  drupal_set_title('Delete Account');
  return drupal_get_form('zmt_account_delete_form', $account);
}

/**
 * Form callback: confirmation form for deleting a account.
 *
 * @param $account
 *   The account object to be deleted.
 *
 * @see confirm_form()
 */
function zmt_account_delete_form($form, &$form_state, $account) {
  $form_state['account'] = $account;

  $form['#submit'][] = 'zmt_account_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete account %name?', array('%name' => $account->name)),
    '',
    '<p>' . t('Deleting this account cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for zmt_account_delete_form().
 */
function zmt_account_delete_form_submit($form, &$form_state) {
  $account = $form_state['account'];
  zmt_account_delete($account->id);
  drupal_set_message(t('%name has been deleted.', array('%name' => $account->name)));
  watchdog('zmt_account', 'Deleted account %name.', array('%name' => $account->name), WATCHDOG_NOTICE);
}
