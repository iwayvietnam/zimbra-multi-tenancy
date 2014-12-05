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
  $header[] = array('data' => t('Code'), 'field' => 'a.code');
  $header[] = array('data' => t('Category'), 'field' => 'a.category');
  $header[] = array('data' => t('Status'), 'field' => 'a.status');
  $header[] = array('data' => t('Created'), 'field' => 'a.created');
  $header[] = array('data' => t('Creator'), 'field' => 'a.creator');
  $header[] = array('data' => t('Op'));

  $query = db_select('zmt_account', 'a')->extend('TableSort');
  $query->leftJoin('zmt_domain', 'd', 'd.id = a.domain_id');
  $query->addField('d', 'name', 'domain');
  $query->leftJoin('users', 'u', 'u.uid = a.uid');
  $query->addField('u', 'name', 'creator');

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

  $item_per_page = (int) variable_get('zimbra_item_per_page', 0);
  if ($item_per_page) {
    $query->extend('PagerDefault')->limit($item_per_page);
  }
  $result = $query
    ->fields('a')
    ->orderByHeader($header)
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
      'id' => $account->full_name,
    );
    $row[] = array(
      'data' => $account->code,
      'id' => $account->code,
    );
    $row[] = array(
      'data' => $account->category,
      'id' => $account->category,
    );
    $row[] = array(
      'data' => isset($statuses[$account->status]) ? $statuses[$account->status] : $account->status,
      'id' => $account->status,
    );
    $row[] = array(
      'data' => date('d-m-y', $account->created) . '<br>' . date('H:i:s', $account->created),
      'id' => $account->created,
    );
    $row[] = array(
      'data' => $account->creator,
      'id' => $account->creator,
    );

	$operations = '<ul class="links" style="list-style: none;">';
    $operations .= '<li class="edit first">' . l(t('edit'), 'zmt/account/' . $account->id . '/edit', array('query' => $destination)) . '</li>';
    $operations .= '<li class="edit last">' . l(t('delete'), 'zmt/account/' . $account->id . '/delete', array('query' => $destination)) . '</li>';
    $operations .= '</ul>';
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

  $form_state['#account'] = $account;

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
  );

  $form['password'] = array(
    '#type' => 'password_confirm',
    '#required' => !(isset($account->id) && (int) $account->id > 0),
    '#size' => 30,
  );
  $form['code'] = array(
    '#title' => t('Code'),
    '#type' => 'textfield',
    '#default_value' => $account->code,
    '#required' => TRUE,
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
    '#required' => TRUE,
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
    '#required' => TRUE,
  );
  $form['category'] = array(
    '#title' => t('Category'),
    '#type' => 'select',
    '#default_value' => $account->category,
    '#options' => array('' => t('- None -')) + zmt_account_categories(),
    '#required' => TRUE,
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
  $account = $form_state['#account'];

  $domain_id = (int) $values['domain_id'];
  $name = trim($values['name']);
  $id = isset($account->id) ? (int) $account->id : 0;
  if(!preg_match('!^[a-zA-Z0-9_.]+$!', $name))
    form_set_error('name', t('Please enter a valid account name.'));

  $results = module_invoke_all('mailbox_exists', $domain_id, $name, $id);
  foreach ($results as $result) {
    if ($result === TRUE) {
      form_set_error('name', t('The mailbox name already exists in selected domain.'));
      break;
    }
  }

  if ($id === 0) {
    $domain = zmt_domain_load($domain_id);
    if ($domain) {
      $account_max_number = (int) $domain->account_max_number;
      $total_account = zmt_account_count_by_domain($domain_id);
      if ($total_account >= $account_max_number) {
        form_set_error('name', t('The domain has reached the maximum number of accounts.'));
      }
    }
  }
}

/**
 * Zimbra account edit form submit callback.
 */
function zmt_account_edit_form_submit($form, &$form_state) {
  $account = $form_state['#account'];
  $values = $form_state['values'];

  $account->domain_id = (int) $values['domain_id'];
  $account->name = check_plain(strtolower($values['name']));
  $account->password = trim($values['password']);
  $account->full_name = check_plain($values['full_name']);
  $account->code = check_plain($values['code']);
  $account->status = check_plain($values['status']);
  $account->title = check_plain($values['title']);
  $account->telephone = check_plain($values['telephone']);
  $account->mobile = check_plain($values['mobile']);
  $account->category = check_plain($values['category']);

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
    '#description' => t('Csv file fields: name,password,code,full_name,status,title,telephone,mobile,category,group'),
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
  );

  $form['button_import'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
    '#submit' => array('zmt_account_import_form_callback'),
    '#suffix' => l(t('Cancel'), 'zmt/account'),
  );
  return $form;
}

/**
 * Import accounts form callback
 */
function zmt_account_import_form_callback($form, &$form_state) {
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

      $handle = fopen($file->uri, 'r');
      $row = fgetcsv($handle);
      $columns = array();
      foreach ($row as $i => $header) {
        $columns[$i] = trim($header);
      }
      $row_num = 1;
      $col_num = count($columns);
      $count = 0;
      $failed = 0;
      while ($row = fgetcsv($handle)) {
        $row_num++;
        if (count($row) !== $col_num) {
          drupal_set_message(t('Column number of row @row_num is invalid.', array('@row_num' => $row_num)));
          continue;
        }
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
        if (!empty($account->name) && preg_match('!^[a-zA-Z0-9_.]+$!', $account->name)) {
          $account->name =strtolower($account->name);
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
            module_invoke_all('update_account_group', $domain_id, $account);
            $count++;
          }
          else {
            watchdog('zimbra', 'Account ' . $account->name . ' already existed.');
            $failed++;
          }
        }
        else {
          drupal_set_message(t('Format of row @row_num is invalid.', array('@row_num' => $row_num)));
        }
      }
      fclose($handle);
      drupal_set_message(t('@count accounts were imported.', array('@count' => $count)));
      drupal_set_message(t('@failed accounts were not imported, because they already existed.', array('@failed' => $failed)));
    }
  }
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
  $form_state['#account'] = $account;

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
  $account = $form_state['#account'];
  zmt_account_delete($account->id);
  drupal_set_message(t('%name has been deleted.', array('%name' => $account->name)));
  watchdog('zmt_account', 'Deleted account %name.', array('%name' => $account->name), WATCHDOG_NOTICE);
}
