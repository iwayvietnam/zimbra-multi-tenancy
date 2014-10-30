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
  $header[] = array('data' => t('Mailbox quota'), 'field' => 'c.mail_quota');
  $header[] = array('data' => t('Max account number'), 'field' => 'c.max_account');
  $header[] = array('data' => t('Server'), 'field' => 's.name');
  $header[] = array('data' => t('Operations'));

  $query = db_select('zmt_cos', 'c')->extend('PagerDefault')->extend('TableSort');
  $query->join('zmt_server', 's', 's.id = c.server_id');
  $query->addField('s', 'name', 'server');
  if (!(user_access('administer zmt') || user_access('administer any zmt_cos'))) {
    $query->condition('c.uid', $user->uid);
  }
  $server_id = isset($_SESSION['zmt_filter_server_id']) ? (int) $_SESSION['zmt_filter_server_id'] : 0;
  if ($server_id) {
    $query->condition('c.server_id', $server_id);
  }

  $item_per_page = variable_get('zimbra_item_per_page', 20);
  $result = $query
    ->fields('c')
    ->orderByHeader($header)
    ->limit($item_per_page)
    ->execute();

  $destination = drupal_get_destination();

  $rows = array();
  foreach ($result as $key => $cos) {
    $row = array();
    $row[] = $key + 1 + (pager_find_page() * $item_per_page);
    $row[] = l($cos->name, 'zmt/cos/' . $cos->id . '/edit', array('query' => $destination));
    $row[] = array(
      'data' => $cos->mail_quota . ' MB',
      'id' => $cos->mail_quota,
    );
    $row[] = array(
      'data' => $cos->max_account,
      'id' => $cos->max_account,
    );
    $row[] = array(
      'data' => $cos->server,
      'id' => $cos->server,
    );
    $row[] = l(t('edit'), 'zmt/cos/' . $cos->id . '/edit', array('query' => $destination));
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('There are no COS available.'),
        'colspan' => count($header)
      ),
    );
  }

  if (user_access('administer zmt')) {
    $build['cos_filter'] = drupal_get_form('zmt_filter_by_server_form');
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
  $cos = $form_state['#cos'];
  $values = $form_state['values'];

  $cos->name = check_plain($values['name']);
  $cos->description = check_plain($values['description']);
  $cos->max_account = abs((int) $values['max_account']);
  $cos->mail_quota = abs((int) $values['mail_quota']);
  zmt_cos_save($cos);

  $form_state['redirect'] = 'zmt/cos';
}
