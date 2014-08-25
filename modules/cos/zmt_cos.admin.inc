<?php

/**
 * List all zimbra COS page callback.
 */
function zmt_cos_list_all() {
  global $user;

  drupal_set_title('Class of Services');
  $header = array();
  $header[] = array('data' => t('No.'));
  $header[] = array('data' => t('Name'), 'field' => 'c.name');
  $header[] = array('data' => t('Max account number'), 'field' => 'c.account_max_number');
  $header[] = array('data' => t('Mailbox quota'), 'field' => 'c.mail_quota');
  $header[] = array('data' => t('Operations'));

  $query = db_select('zmt_cos', 'c')->extend('TableSort');
  if (!(user_access('administer zmt') || user_access('administer any zmt_cos'))) {
    $query->condition('c.uid', $user->uid);
  }

  $item_per_page = (int) variable_get('zimbra_item_per_page', 0);
  if ($item_per_page) {
    $query->extend('PagerDefault')->limit($item_per_page);
  }
  $result = $query
    ->fields('c')
    ->orderByHeader($header)
    ->execute();

  $destination = drupal_get_destination();

  $rows = array();
  foreach ($result as $key => $cos) {
    $row = array();
    $row[] = $key + 1 + (pager_find_page() * $item_per_page);
    $row[] = l($cos->name, 'zmt/cos/' . $cos->id . '/edit', array('query' => $destination));
    $row[] = array(
      'data' => $cos->account_max_number,
      'id' => $cos->account_max_number,
    );
    $row[] = array(
      'data' => $cos->mail_quota . ' MB',
      'id' => $cos->mail_quota,
    );

    $operations = l(t('edit'), 'zmt/cos/' . $cos->id . '/edit', array('query' => $destination));
    $operations .= '&nbsp;&nbsp;&nbsp;' . l(t('delete'), 'zmt/cos/' . $cos->id . '/delete', array('query' => $destination));
    $row[] = $operations;
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('There are no COS available.') . ' ' . l(t('Add one now'), 'zmt/cos/add') . '.',
        'colspan' => count($header)
      ),
    );
  }

  $build['cos_table']  = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['cos_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Add new zimbra cos page callback.
 */
function zmt_cos_add_page() {
  drupal_set_title('Add Service');
  $cos = zmt_cos_new();
  return drupal_get_form('zmt_cos_edit_form', $cos);
}

/**
 * Edit exists zimbra cos page callback.
 */
function zmt_cos_edit_page($cos) {
  drupal_set_title('Edit Service');
  return drupal_get_form('zmt_cos_edit_form', $cos);
}

/**
 * Zimbra cos edit form.
 */
function zmt_cos_edit_form($form, &$form_state, $cos){
  $form_state['#cos'] = $cos;
  $none = array('' => t('- None -'));
  $cos_id = isset($cos->id) ? (int) $cos->id : 0;

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $cos->name,
    '#required' => TRUE,
  );
  if ($cos_id) {
    $form['name']['#attributes']['readonly'] = TRUE;
  }
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $cos->description,
  );

  $servers = zmt_server_load_all();
  $options = array();
  foreach ($servers as $server) {
    $options[$server->id] = $server->name;
  }
  $cos_servers = zmt_cos_server_load_multiple_by_cos(array($cos_id));
  $values = array();
  foreach ($cos_servers as $cos_server) {
    $values[] = $cos_server->server_id;
  }
  $form['servers'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Servers'),
    '#options' => $options,
    '#default_value' => $values,
    '#required' => TRUE,
  );

  $form['account_max_number'] = array(
    '#title' => t('Max account number'),
    '#type' => 'textfield',
    '#default_value' => $cos->account_max_number,
    '#element_validate' => array(
      'element_validate_integer_positive',
    ),
  );
  $form['mail_quota'] = array(
    '#title' => t('Mailbox quota (MB)'),
    '#type' => 'textfield',
    '#default_value' => $cos->mail_quota,
    '#element_validate' => array(
      'element_validate_integer_positive',
    ),
  );
  $form['attachments_blocked'] = array(
    '#type' => 'checkbox',
    '#title' => t('Block attachments'),
    '#default_value' => $cos->attachments_blocked,
  );
  $form['password_min_length'] = array(
    '#title' => t('Password min length'),
    '#type' => 'textfield',
    '#default_value' => $cos->password_min_length,
  );
  $form['password_max_length'] = array(
    '#title' => t('Password max length'),
    '#type' => 'textfield',
    '#default_value' => $cos->password_max_length,
  );
  $form['password_min_upper'] = array(
    '#title' => t('Password min upper case characters'),
    '#type' => 'textfield',
    '#default_value' => $cos->password_min_upper,
  );
  $form['password_min_lower'] = array(
    '#title' => t('Password min lower case characters'),
    '#type' => 'textfield',
    '#default_value' => $cos->password_min_lower,
  );
  $form['password_min_numeric'] = array(
    '#title' => t('Password min numeric characters'),
    '#type' => 'textfield',
    '#default_value' => $cos->password_min_numeric,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('zmt_cos_edit_form_submit'),
    '#suffix' => l(t('Cancel'), 'zmt/cos'),
  );

  hide($form['attachments_blocked']);
  hide($form['password_min_length']);
  hide($form['password_max_length']);
  hide($form['password_min_upper']);
  hide($form['password_min_lower']);
  hide($form['password_min_numeric']);

  return $form;
}

/**
 * Zimbra cos edit form submit callback.
 */
function zmt_cos_edit_form_submit($form, &$form_state) {
  $cos = $form_state['#cos'];
  $values = $form_state['values'];

  $cos->name = check_plain($values['name']);
  $cos->description = check_plain($values['description']);
  $cos->account_max_number = (int) $values['account_max_number'];
  $cos->mail_quota = (int) $values['mail_quota'];
  $cos->attachments_blocked = (int) $values['attachments_blocked'];
  $cos->password_min_length = (int) $values['password_min_length'];
  $cos->password_max_length = (int) $values['password_max_length'];
  $cos->password_min_upper = (int) $values['password_min_upper'];
  $cos->password_min_lower = (int) $values['password_min_lower'];
  $cos->password_min_numeric = (int) $values['password_min_numeric'];
  zmt_cos_save($cos);

  $cos_servers = zmt_cos_server_load_multiple_by_cos(array($cos->id));
  $available_ids = array();
  $selected_ids = array_values($values['servers']);

  foreach ($cos_servers as $cos_server) {
    $server_id = (int) $cos_server->server_id;
    $available_ids[] = $server_id;
    if (!in_array($server_id, $selected_ids)) {
      zmt_cos_server_delete($cos_server->id);
    }
  }
  foreach ($selected_ids as $server_id) {
    if ((int) $server_id > 0 && !in_array($server_id, $available_ids)) {
      $cos_server = zmt_cos_server_new();
      $cos_server->cos_id = $cos->id;
      $cos_server->server_id = $server_id;
      zmt_cos_server_save($cos_server);
    }
  }

  $form_state['redirect'] = 'zmt/cos';
}

/**
 * Delete exists zimbra cos page callback.
 */
function zmt_cos_delete_page($cos) {
  drupal_set_title('Delete Service');
  return drupal_get_form('zmt_cos_delete_form', $cos);
}

/**
 * Form callback: confirmation form for deleting a cos.
 *
 * @param $cos
 *   The cos object to be deleted.
 *
 * @see confirm_form()
 */
function zmt_cos_delete_form($form, &$form_state, $cos) {
  $form_state['#cos'] = $cos;

  $form['#submit'][] = 'zmt_cos_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete service %name?', array('%name' => $cos->name)),
    '',
    '<p>' . t('Deleting this service cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for zmt_cos_delete_form().
 */
function zmt_cos_delete_form_submit($form, &$form_state) {
  $cos = $form_state['#cos'];
  zmt_cos_delete($cos->id);
  drupal_set_message(t('%name has been deleted.', array('%name' => $cos->name)));
  watchdog('zmt_cos', 'Deleted service %name.', array('%name' => $cos->name), WATCHDOG_NOTICE);
}
