<?php

/**
 * List all zimbra aliases page callback.
 */
function zmt_alias_list_all() {
  global $user;

  backdrop_set_title('Aliases');
  $header = array();
  $header[] = array('data' => t('No.'));
  $header[] = array('data' => t('Name'), 'field' => 'za.name');
  $header[] = array('data' => t('Target account'), 'field' => 'za.account');
  $header[] = array('data' => t('Operations'));

  $query = db_select('zmt_alias', 'za')->extend('PagerDefault')->extend('TableSort');
  $query->leftJoin('zmt_domain', 'zd', 'zd.id = za.domain_id');
  $query->leftJoin('zmt_account', 'zac', 'zac.id = za.account_id');
  $query->addField('zd', 'name', 'domain');
  $query->addField('zac', 'name', 'account');

  $domain_ids = array();
  if (!(user_access('administer zmt'))) {
    if (user_access('administer any zmt_alias')) {
      $domains = zmt_domain_load_multiple_by_user($user);
      $domain_ids = array_keys($domains);
      if (empty($domain_ids)) {
        $domain_ids = array(0);
      }

      $domain_id = isset($_SESSION['zmt_filter_domain_id']) ? (int) $_SESSION['zmt_filter_domain_id'] : 0;
      if ($domain_id) {
        $query->condition('za.domain_id', $domain_id);
      }
      else {
        $query->condition('za.domain_id', $domain_ids);
        $query->addTag('alias_query_filter');
      }
    }
    else {
      $query->condition('za.uid', $user->uid);
    }
  }
  else {
    $domain_id = isset($_SESSION['zmt_filter_domain_id']) ? (int) $_SESSION['zmt_filter_domain_id'] : 0;
    if ($domain_id) {
      $query->condition('za.domain_id', $domain_id);
    }
  }

  $item_per_page = variable_get('zimbra_item_per_page', 20);
  $result = $query
    ->fields('za')
    ->orderByHeader($header)
    ->limit($item_per_page)
    ->execute();

  $destination = backdrop_get_destination();
  $rows = array();
  foreach ($result as $key => $alias) {
    $row = array();
    $row[] = $key + 1 + (pager_find_page() * $item_per_page);
    $alias_name = $alias->name . '@' . $alias->domain;
    $row[] = l($alias_name, 'zmt/alias/' . $alias->id . '/edit', array('query' => $destination));
    $row[] = array(
      'data' => $alias->account . '@' . $alias->domain,
    );

    $operations = l(t('edit'), 'zmt/alias/' . $alias->id . '/edit', array('query' => $destination));
    $operations .= '&nbsp;&nbsp;&nbsp;' . l(t('delete'), 'zmt/alias/' . $alias->id . '/delete', array('query' => $destination));
    $row[] = $operations;
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('There are no aliases available.') . ' ' . l(t('Add one now'), 'zmt/alias/add') . '.',
        'colspan' => count($header)
      ),
    );
  }

  $build = array();
  if (user_access('administer zmt')) {
    $build['alias_filter'] = backdrop_get_form('zmt_filter_by_domain_form');
  }
  elseif (user_access('administer any zmt_account')) {
    if (count($domain_ids) > 1) {
      $build['alias_filter'] = backdrop_get_form('zmt_filter_by_domain_form');
    }
  }
  $build['alias_table']  = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['alias_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Add new zimbra alias page callback.
 */
function zmt_alias_add_page() {
  backdrop_set_title('Add Alias');
  $alias = zmt_alias_new();
  return backdrop_get_form('zmt_alias_edit_form', $alias);
}

/**
 * Edit exists zimbra alias page callback.
 */
function zmt_alias_edit_page($alias) {
  backdrop_set_title('Edit Alias');
  return backdrop_get_form('zmt_alias_edit_form', $alias);
}

/**
 * Zimbra alias edit form.
 */
function zmt_alias_edit_form($form, &$form_state, $alias){
  global $user;

  $form_state['#alias'] = $alias;

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $alias->name,
    '#required' => TRUE,
  );

  $options = array();
  if (isset($alias->id) && (int) $alias->id > 0) {
    $domain = zmt_domain_load($alias->domain_id);
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
    else {
      $domain = reset($domains);
      $alias->domain_id = $domain->id;
    }
    foreach ($domains as $domain) {
      $options[$domain->id] = $domain->name;
    }
  }
  $form['domain_id'] = array(
    '#type' => 'select',
    '#title' => t('Domain'),
    '#options' => $options,
    '#default_value' => $alias->domain_id,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'ajax_alias_domain_select_callback',
      'wrapper' => 'dropdown-account-replace',
    ),
  );

  $domain_id = isset($form_state['values']['domain_id']) ? (int) $form_state['values']['domain_id'] : (int) $alias->domain_id;
  $accounts = zmt_account_load_multiple_by_domain($domain_id);
  $options = array();
  foreach ($accounts as $account) {
    $options[$account->id] = $account->name;
  }
  $form['account_id'] = array(
    '#type' => 'select',
    '#title' => t('Account'),
    '#options' => $options,
    '#empty_option' => t('- None -'),
    '#default_value' => $alias->account_id,
    '#required' => TRUE,
    '#prefix' => '<div id="dropdown-account-replace">',
    '#suffix' => '</div>',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('zmt_alias_edit_form_submit'),
    '#suffix' => l(t('Cancel'), 'zmt/alias'),
  );

  $form['#validate'][] = 'zmt_alias_edit_form_validate';

  return $form;
}

/**
 * Alias ajax domain selection callback.
 */
function ajax_alias_domain_select_callback($form, $form_state) {
  return $form['account_id'];
}

/**
 * Zimbra alias edit form validate callback.
 */
function zmt_alias_edit_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $alias = $form_state['#alias'];

  $domain_id = (int) $values['domain_id'];
  $name = trim($values['name']) ;
  $id = isset($alias->id) ? (int) $alias->id : 0;

  $exists = zmt_alias_exists($domain_id, $name, $id);
  if ($exists === TRUE) {
    form_set_error('name', t('The mailbox name already exists in selected domain.'));
  }
}

/**
 * Zimbra alias edit form submit callback.
 */
function zmt_alias_edit_form_submit($form, &$form_state) {
  $alias = $form_state['#alias'];
  $values = $form_state['values'];

  $alias->name = trim($values['name']);
  $alias->domain_id = (int) $values['domain_id'];
  $alias->account_id = (int) $values['account_id'];
  zmt_alias_save($alias);

  $form_state['redirect'] = 'zmt/alias';
}

/**
 * Delete exists zimbra alias page callback.
 */
function zmt_alias_delete_page($alias) {
  backdrop_set_title('Delete Alias');
  return backdrop_get_form('zmt_alias_delete_form', $alias);
}

/**
 * Form callback: confirmation form for deleting a alias.
 *
 * @param $alias
 *   The alias object to be deleted.
 *
 * @see confirm_form()
 */
function zmt_alias_delete_form($form, &$form_state, $alias) {
  $form_state['#alias'] = $alias;

  $form['#submit'][] = 'zmt_alias_delete_form_submit';

  $description = '';
  if ((int) $alias->delete_alias > 0 && !empty($alias->zimbra_target_id)) {
    $description .= t('Delete this alias also delete it in zimbra server.');
  }
  $description .= t(' This action cannot be undone.');

  $form = confirm_form($form,
    t('Are you sure you want to delete alias %name?', array('%name' => $alias->name)),
    '',
    '<p>' . $description . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for zmt_alias_delete_form().
 */
function zmt_alias_delete_form_submit($form, &$form_state) {
  $alias = $form_state['#alias'];
  zmt_alias_delete($alias->id);
  backdrop_set_message(t('%name has been deleted.', array('%name' => $alias->name)));
  watchdog('zmt_alias', 'Deleted alias %name.', array('%name' => $alias->name), WATCHDOG_NOTICE);
}
