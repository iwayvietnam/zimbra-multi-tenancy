<?php

/**
 * List all zimbra COS page callback.
 */
function zmt_cos_list_all() {
  global $user;

  drupal_set_title(t('Class of Services'));
  $header = array();
  $header[] = array('data' => t('No.'));
  $header[] = array('data' => t('Name'), 'field' => 'zc.name');
  $header[] = array('data' => t('Mailbox quota'), 'field' => 'zc.mail_quota');
  $header[] = array('data' => t('Max account number'), 'field' => 'zc.max_account');
  $header[] = array('data' => t('Server'), 'field' => 'zs.name');
  $header[] = array('data' => t('Operations'));

  $query = db_select('zmt_cos', 'zc')->extend('PagerDefault')->extend('TableSort');
  $query->join('zmt_server', 'zs', 'zs.id = zc.server_id');

  if (!(user_access('administer zmt') || user_access('administer any zmt_cos'))) {
    $query->condition('zc.uid', $user->uid);
  }
  $server_id = isset($_SESSION['zmt_filter_server_id']) ? (int) $_SESSION['zmt_filter_server_id'] : 0;
  if ($server_id) {
    $query->condition('zc.server_id', $server_id);
  }

  $destination = drupal_get_destination();
  $item_per_page = variable_get('zimbra_item_per_page', 20);
  $ids = $query
    ->fields('zc', array('id'))
    ->orderByHeader($header)
    ->limit($item_per_page)
    ->execute()
    ->fetchCol();
  $coses = zmt_cos_load_multiple($ids);

  $key = 1;
  $rows = array();
  foreach ($coses as $cos) {
    $row = array();
    $row[] = $key + (pager_find_page() * $item_per_page);
    $row[] = l($cos->name, 'zmt/cos/' . $cos->id . '/edit', array('query' => $destination));
    $row[] = array(
      'data' => $cos->mail_quota . ' MB',
    );
    $row[] = array(
      'data' => $cos->max_account,
    );
    $row[] = array(
      'data' => $cos->server,
    );
    $row[] = l(t('edit'), 'zmt/cos/' . $cos->id . '/edit', array('query' => $destination));
    $rows[] = $row;
    $key++;
  }

  if (user_access('administer zmt')) {
    $build['cos_filter'] = drupal_get_form('zmt_filter_by_server_form');
  }
  $build['cos_table']  = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('There are no COS available.'),
  );
  $build['cos_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Edit exists zimbra cos page callback.
 */
function zmt_cos_edit_page($cos) {
  drupal_set_title(t('Edit Service'));
  return drupal_get_form('zmt_cos_edit_form', $cos);
}

/**
 * Zimbra cos edit form.
 */
function zmt_cos_edit_form($form, &$form_state, $cos){
  $form_state['cos'] = $cos;
  $cos_id = isset($cos->id) ? (int) $cos->id : 0;

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $cos->name,
    '#required' => TRUE,
    '#attributes' => array(
      'readonly' => TRUE,
    ),
  );
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $cos->description,
    '#attributes' => array(
      'readonly' => TRUE,
    ),
  );

  $form['mail_quota'] = array(
    '#title' => t('Mailbox quota (MB)'),
    '#type' => 'textfield',
    '#default_value' => $cos->mail_quota,
    '#attributes' => array(
      'readonly' => TRUE,
    ),
  );

  $form['max_account'] = array(
    '#title' => t('Max account number'),
    '#type' => 'textfield',
    '#default_value' => $cos->max_account,
    '#element_validate' => array(
      'element_validate_integer_positive',
    ),
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('zmt_cos_edit_form_submit'),
    '#suffix' => l(t('Cancel'), 'zmt/cos'),
  );

  return $form;
}

/**
 * Zimbra cos edit form submit callback.
 */
function zmt_cos_edit_form_submit($form, &$form_state) {
  $cos = $form_state['cos'];
  $values = $form_state['values'];

  $cos->name = check_plain($values['name']);
  $cos->description = check_plain($values['description']);
  $cos->max_account = abs((int) $values['max_account']);
  $cos->mail_quota = abs((int) $values['mail_quota']);
  zmt_cos_save($cos);

  $form_state['redirect'] = 'zmt/cos';
}
