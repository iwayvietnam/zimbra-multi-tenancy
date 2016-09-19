<?php

/**
 * List all zimbra servers page callback.
 */
function zmt_server_list_all() {
  global $user;

  backdrop_set_title(t('Zimbra Servers'));
  $header = array();
  $header[] = array('data' => t('No.'));
  $header[] = array('data' => t('Name'), 'field' => 'zs.name');
  $header[] = array('data' => t('Service location'), 'field' => 'zs.service_location');
  $header[] = array('data' => t('Admin user'), 'field' => 'zs.admin_user');
  $header[] = array('data' => t('Operations'));

  $query = db_select('zmt_server', 'zs')->extend('PagerDefault')->extend('TableSort');
  if (!(user_access('administer zmt') || user_access('administer any zmt_server'))) {
    $query->condition('zs.uid', $user->uid);
  }

  $item_per_page = variable_get('zimbra_item_per_page', 20);
  $result = $query
    ->fields('zs')
    ->orderByHeader($header)
    ->limit($item_per_page)
    ->execute();

  $destination = backdrop_get_destination();

  $rows = array();
  foreach ($result as $key => $server) {
    $row = array();
    $row[] = $key + 1 + (pager_find_page() * $item_per_page);
    $row[] = l($server->name, 'zmt/server/' . $server->id . '/edit', array('query' => $destination));
    $row[] = array(
      'data' => $server->service_location,
    );
    $row[] = array(
      'data' => $server->admin_user,
    );

    $operations = l(t('edit'), 'zmt/server/' . $server->id . '/edit', array('query' => $destination));
    $operations .= '&nbsp;&nbsp;&nbsp;' . l(t('delete'), 'zmt/server/' . $server->id . '/delete', array('query' => $destination));
    $row[] = $operations;
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('There are no servers available.') . ' ' . l(t('Add one now'), 'zmt/server/add') . '.',
        'colspan' => count($header)
      ),
    );
  }

  $build['servers_table']  = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['servers_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Add new zimbra server page callback.
 */
function zmt_server_add_page() {
  backdrop_set_title(t('Add Zimbra Server'));
  $server = zmt_server_new();
  return backdrop_get_form('zmt_server_edit_form', $server);
}

/**
 * Edit exists zimbra server page callback.
 */
function zmt_server_edit_page($server) {
  backdrop_set_title(t('Edit Zimbra Server'));
  return backdrop_get_form('zmt_server_edit_form', $server);
}

/**
 * Zimbra server edit form.
 */
function zmt_server_edit_form($form, &$form_state, $server){
  $form_state['server'] = $server;

  $form['name'] = array(
    '#title' => t('Server name'),
    '#type' => 'textfield',
    '#default_value' => $server->name,
    '#required' => TRUE,
  );
  $form['service_location'] = array(
    '#title' => t('Service location'),
    '#type' => 'textfield',
    '#default_value' => $server->service_location,
    '#required' => TRUE,
  );
  $form['admin_user'] = array(
    '#title' => t('Admin user'),
    '#type' => 'textfield',
    '#default_value' => $server->admin_user,
    '#required' => TRUE,
    '#size' => 30,
  );
  $form['admin_password'] = array(
    '#title' => t('Admin password'),
    '#type' => 'password',
    '#default_value' => $server->admin_password,
    '#required' => !isset($server->id) || (int) $server->id == 0,
    '#size' => 30,
  );
  if (empty($server->exclude_mailbox)) {
    $server->exclude_mailbox = implode(PHP_EOL, zmt_server_exclude_mailbox());
  }
  $form['exclude_mailbox'] = array(
    '#title' => t('Exclude mailboxes'),
    '#type' => 'textarea',
    '#default_value' => $server->exclude_mailbox,
  );
  $form['delete_domain'] = array(
    '#title' => t('Allow delete domain on this server'),
    '#type' => 'checkbox',
    '#default_value' => $server->delete_domain,
  );
  $form['delete_dl'] = array(
    '#title' => t('Allow delete DL on this server'),
    '#type' => 'checkbox',
    '#default_value' => $server->delete_dl,
  );
  $form['delete_account'] = array(
    '#title' => t('Allow delete account on this server'),
    '#type' => 'checkbox',
    '#default_value' => $server->delete_account,
  );
  $form['delete_alias'] = array(
    '#title' => t('Allow delete alias on this server'),
    '#type' => 'checkbox',
    '#default_value' => $server->delete_alias,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('zmt_server_edit_form_submit'),
    '#suffix' => l(t('Cancel'), 'zmt/server'),
  );

  $form['#validate'][] = 'zmt_server_edit_form_validate';

  return $form;
}

/**
 * Form validation handler for zmt_server_edit_form().
 *
 * @see zmt_server_edit_form()
 */
function zmt_server_edit_form_validate($form, &$form_state) {
  $values = &$form_state['values'];
  if (!valid_url($values['service_location'], TRUE)) {
    form_set_error('service_location', t('The service location is an invalid url.'));
  }
}

/**
 * Zimbra server edit form submit callback.
 */
function zmt_server_edit_form_submit($form, &$form_state) {
  $server = $form_state['server'];
  $values = $form_state['values'];

  $server->name = check_plain($values['name']);
  $server->service_location = check_plain($values['service_location']);
  $server->admin_user = check_plain($values['admin_user']);
  $password = trim($values['admin_password']);
  if (!empty($password)) {
    $server->admin_password = zmt_password_encrypt($values['admin_password']);
  }
  $server->exclude_mailbox = check_plain($values['exclude_mailbox']);
  $server->delete_domain = (int) $values['delete_domain'];
  $server->delete_dl = (int) $values['delete_dl'];
  $server->delete_account = (int) $values['delete_account'];
  $server->delete_alias = (int) $values['delete_alias'];
  zmt_server_save($server);

  $form_state['redirect'] = 'zmt/server';
}

/**
 * Delete exists zimbra server page callback.
 */
function zmt_server_delete_page($server) {
  backdrop_set_title(t('Delete Zimbra Server'));
  return backdrop_get_form('zmt_server_delete_form', $server);
}

/**
 * Form callback: confirmation form for deleting a server.
 *
 * @param $server
 *   The server object to be deleted.
 *
 * @see confirm_form()
 */
function zmt_server_delete_form($form, &$form_state, $server) {
  $form_state['server'] = $server;

  $form['#submit'][] = 'zmt_server_delete_form_submit';
  $form['#validate'][] = 'zmt_server_delete_form_validate';

  $form = confirm_form($form,
    t('Are you sure you want to delete server %name?', array('%name' => $server->name)),
    '',
    '<p>' . t('Deleting this server cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Validate callback for zmt_server_delete_form().
 */
function zmt_server_delete_form_validate($form, &$form_state) {
  $server = $form_state['server'];
  $mailbox_number = zmt_server_count_domain($server->id);
  if ($mailbox_number) {
    form_set_error('actions', t('Please delete all domains of this server before delete this server.'));
  }
}

/**
 * Submit callback for zmt_server_delete_form().
 */
function zmt_server_delete_form_submit($form, &$form_state) {
  $server = $form_state['server'];
  zmt_server_delete($server->id);
  backdrop_set_message(t('%name has been deleted.', array('%name' => $server->name)));
  watchdog('zmt_server', 'Deleted zimbra server %name.', array('%name' => $server->name), WATCHDOG_NOTICE);
}
