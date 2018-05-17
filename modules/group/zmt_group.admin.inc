<?php

/**
 * List all zimbra groupes page callback.
 */
function zmt_group_list_all() {
  global $user;

  drupal_set_title(t('Groups'));
  $header = array();
  $header[] = array('data' => t('No.'));
  $header[] = array('data' => t('Name'), 'field' => 'zg.name');
  $header[] = array('data' => t('Full name'), 'field' => 'zg.full_name');
  $header[] = array('data' => t('Total member'));
  $header[] = array('data' => t('Operations'));

  $query = db_select('zmt_group', 'zg')->extend('PagerDefault')->extend('TableSort');

  $domain_ids = array();
  if (!(user_access('administer zmt'))) {
    if (user_access('administer any zmt_group')) {
      $domains = zmt_domain_load_multiple_by_user($user);
      $domain_ids = array_keys($domains);
      if (empty($domain_ids)) {
        $domain_ids = array(0);
      }
      $domain_id = isset($_SESSION['zmt_filter_domain_id']) ? (int) $_SESSION['zmt_filter_domain_id'] : 0;
      if ($domain_id) {
        $query->condition('zg.domain_id', $domain_id);
      }
      else {
        $query->condition('zg.domain_id', $domain_ids);
      }
    }
    else {
      $query->condition(
        db_or()
          ->condition('zg.uid', $user->uid)
          ->condition('zg.group_uid', $user->uid)
      );
    }
  }
  else {
    $domain_id = isset($_SESSION['zmt_filter_domain_id']) ? (int) $_SESSION['zmt_filter_domain_id'] : 0;
    if ($domain_id) {
      $query->condition('zg.domain_id', $domain_id);
    }
  }
  $name = isset($_SESSION['zmt_group_filter_name']) ? $_SESSION['zmt_group_filter_name'] : '';
  if (!empty($name)) {
    $query->condition(
      db_or()
        ->condition('zg.name', '%' . db_like($name) . '%', 'LIKE')
        ->condition('zg.full_name', '%' . db_like($name) . '%', 'LIKE')
    );
  }

  $destination = drupal_get_destination();
  $item_per_page = variable_get('zimbra_item_per_page', 20);
  $ids = $query
    ->fields('zg', array('id'))
    ->orderByHeader($header)
    ->limit($item_per_page)
    ->execute()
    ->fetchCol();
  $groups = zmt_group_load_multiple($ids);

  $key = 1;
  $rows = array();
  foreach ($groups as $group) {
    $row = array();
    $row[] = $key + (pager_find_page() * $item_per_page);
    $group_name = $group->name . '@' . $group->domain;
    $row[] = l($group_name, 'zmt/group/' . $group->id . '/edit', array('query' => $destination));
    $row[] = array(
      'data' => $group->full_name,
    );
    $row[] = array(
      'data' => $group->total_member,
    );
    $row[] = implode('&nbsp;&nbsp;&nbsp;', array(
      l(t('edit'), 'zmt/group/' . $group->id . '/edit', array('query' => $destination)),
      l(t('delete'), 'zmt/group/' . $group->id . '/delete', array('query' => $destination)),
    ));

    $rows[] = $row;
    $key++;
  }

  $build = array();
  if (user_access('administer zmt')) {
    $build['group_filter'] = drupal_get_form('zmt_group_filter_by_domain_form');
  }
  elseif (user_access('administer any zmt_account')) {
    if (count($domain_ids) > 1) {
      $build['group_filter'] = drupal_get_form('zmt_group_filter_by_domain_form');
    }
  }
  $build['group_table']  = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('There are no groups available.') . ' ' . l(t('Add one now.'), 'zmt/group/add'),
  );
  $build['group_pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Add new zimbra group page callback.
 */
function zmt_group_add_page() {
  drupal_set_title(t('Add Group'));
  $group = zmt_group_new();
  return drupal_get_form('zmt_group_edit_form', $group);
}

/**
 * Edit exists zimbra group page callback.
 */
function zmt_group_edit_page($group) {
  drupal_set_title(t('Edit Group'));
  return drupal_get_form('zmt_group_edit_form', $group);
}

/**
 * Zimbra group edit form.
 */
function zmt_group_edit_form($form, &$form_state, $group){
  global $user;

  $form_state['#group'] = $group;
  $account = user_load($group->group_uid);
  $form_state['#user'] = ($account && (int) $account->uid > 0) ? $account : drupal_anonymous_user();
  $values = &$form_state['values'];
  $group_id = isset($group->id) ? (int) $group->id : 0;

  $form['group_name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $group->name,
    '#required' => TRUE,
  );
  if ($group_id) {
    $form['group_name']['#attributes']['readonly'] = TRUE;
  }

  $form['full_name'] = array(
    '#title' => t('Full name'),
    '#type' => 'textfield',
    '#default_value' => $group->full_name,
    '#required' => TRUE,
  );

  $options = array();
  if ($group_id > 0) {
    $domain = zmt_domain_load($group->domain_id);
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
      $group->domain_id = $domain->id;
    }
    foreach ($domains as $zmt_domain) {
      $options[$zmt_domain->id] = $zmt_domain->name;
    }
  }

  $form['domain_id'] = array(
    '#type' => 'select',
    '#title' => t('Domain'),
    '#options' => $options,
    '#default_value' => $group->domain_id,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'zmt_group_ajax_select_domain_callback',
      'wrapper' => 'member-container-wapper',
    ),
  );

  $domain_id = isset($values['domain_id']) ? (int) $values['domain_id'] : (int) $group->domain_id;
  $change_domain = $domain_id !== (int) $group->domain_id;
  if ($change_domain) {
    $group->domain_id = $domain_id;
  }
  if (empty($form_state['group_members']) || $change_domain) {
    $group_members = array();
    $members = zmt_group_member_load_multiple_by_group(array($group_id));
    foreach ($members as $member) {
      $key = array(
        'name' => $member->name,
        'id' => (int) $member->member_id,
        'type' => (int) $member->type,
      );
      $key = drupal_json_encode($key);
      $group_members[$key] = $member->name;
    }
    $form_state['group_members'] = $group_members;
  }

  if (empty($form_state['available_members']) || $change_domain) {
    $group_member_values = array_keys($form_state['group_members']);
    $available_members = array();
    $domain = zmt_domain_load($domain_id);
    if ($domain) {
      $accounts = zmt_account_load_multiple_by_domain($domain);
      foreach ($accounts as $acc) {
        $mailbox = $acc->name . '@' . $domain->name;
        $key = array(
          'name' => $mailbox,
          'id' => (int) $acc->id,
          'type' => GROUP_MEMBER_ACCOUNT,
        );
        $key = drupal_json_encode($key);
        if (!in_array($key, $group_member_values)) {
          $available_members[$key] = $mailbox;
        }
      }
      $aliases = zmt_alias_load_multiple_by_domain($domain);
      foreach ($aliases as $alias) {
        $mailbox = $alias->name . '@' . $domain->name;
        $key = array(
          'name' => $mailbox,
          'id' => (int) $alias->id,
          'type' => GROUP_MEMBER_ALIAS,
        );
        $key = drupal_json_encode($key);
        if (!in_array($key, $group_member_values)) {
          $available_members[$key] = $mailbox;
        }
      }

      $groups = zmt_group_load_multiple_by_domain($domain, $group_id);
      foreach ($groups as $gr) {
        $mailbox = $gr->name . '@' . $domain->name;
        $key = array(
          'name' => $mailbox,
          'id' => (int) $gr->id,
          'type' => GROUP_MEMBER_DL,
        );
        $key = drupal_json_encode($key);
        if (!in_array($key, $group_member_values)) {
          $available_members[$key] = $mailbox;
        }
      }
    }
    $form_state['available_members'] = $available_members;
  }

  $available_values = !empty($form_state['available_values']) ? $form_state['available_values'] : array();
  $member_values = isset($form_state['member_values']) ? $form_state['member_values'] : array();
  $op = isset($values['op']) ? $values['op'] : '';
  if ($op === t('>>')) {
    $available_members = $form_state['available_members'];
    if (!empty($available_members)) {
      foreach ($available_members as $key => $value) {
        $form_state['group_members'][$key] = $value;
      }
    }
    $form_state['available_members'] = array();
  }
  elseif ($op === t('<<')) {
    $group_members = $form_state['group_members'];
    if (!empty($group_members)) {
      foreach ($group_members as $key => $value) {
        $form_state['available_members'][$key] = $value;
      }
    }
    $form_state['group_members'] = array();
  }
  elseif ($op === t('>')) {
    $members = array();
    $available_members = $form_state['available_members'];
    $group_members = $form_state['group_members'];

    foreach ($group_members as $key => $value) {
      if (in_array($key, array_keys($available_members))) {
        unset($group_members[$key]);
      }
    }

    foreach ($available_members as $key => $value) {
      if (in_array($key, $available_values)) {
        $group_members[$key] = $value;
      }
      else {
        $members[$key] = $value;
      }
    }
    $form_state['available_members'] = $members;
    $form_state['group_members'] = $group_members;
  }
  elseif ($op === t('<')) {
    $members = array();
    $available_members = $form_state['available_members'];
    $group_members = $form_state['group_members'];

    foreach ($available_members as $key => $value) {
      if (in_array($key, array_keys($group_members))) {
        unset($available_members[$key]);
      }
    }

    foreach ($group_members as $key => $value) {
      if (in_array($key, $member_values)) {
        $available_members[$key] = $value;
      }
      else {
        $members[$key] = $value;
      }
    }
    $form_state['available_members'] = $available_members;
    $form_state['group_members'] = $members;
  }

  $form['member_container'] = array(
    '#type' => 'container',
    '#prefix' => '<div id="member-container-wapper">',
    '#suffix' => '</div>',
  );

  $form['member_container']['available_members'] = array(
    '#type' => 'select',
    '#title' => t('Available members'),
    '#options' => $form_state['available_members'],
    '#attributes' => array(
      'multiple' => TRUE,
      'name' => 'available_members[]',
    ),
    '#weight' => 0,
  );

  $form['member_container']['buttons'] = array(
    '#type' => 'actions',
    '#weight' => 1,
  );
  $form['member_container']['buttons']['move_right_all'] = array(
    '#type' => 'button',
    '#value' => t('>>'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'zmt_group_ajax_select_member_callback',
      'wrapper' => 'member-container-wapper',
      'progress' => array(
        'type' => 'none',
      ),
    ),
  );
  $form['member_container']['buttons']['move_right'] = array(
    '#type' => 'submit',
    '#value' => t('>'),
    '#limit_validation_errors' => array(),
    '#submit' => array('zmt_group_move_member_callback'),
    '#ajax' => array(
      'callback' => 'zmt_group_ajax_select_member_callback',
      'wrapper' => 'member-container-wapper',
      'progress' => array(
        'type' => 'none',
      ),
    ),
  );
  $form['member_container']['buttons']['move_left'] = array(
    '#type' => 'submit',
    '#value' => t('<'),
    '#limit_validation_errors' => array(),
    '#submit' => array('zmt_group_move_member_callback'),
    '#ajax' => array(
      'callback' => 'zmt_group_ajax_select_member_callback',
      'wrapper' => 'member-container-wapper',
      'progress' => array(
        'type' => 'none',
      ),
    ),
  );
  $form['member_container']['buttons']['move_left_all'] = array(
    '#type' => 'button',
    '#value' => t('<<'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'zmt_group_ajax_select_member_callback',
      'wrapper' => 'member-container-wapper',
      'progress' => array(
        'type' => 'none',
      ),
    ),
  );

  $form['member_container']['members'] = array(
    '#type' => 'select',
    '#title' => t('Members'),
    '#options' => $form_state['group_members'],
    '#attributes' => array(
      'multiple' => TRUE,
      'name' => 'members[]',
    ),
    '#weight' => 2,
  );

  $have_user = (int) ($account && (int) $account->uid > 0);
  if (!$have_user) {
    $form['create_user'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create user'),
      '#ajax' => array(
        'callback' => 'zmt_group_ajax_create_user_checked',
        'wrapper' => 'user-container',
      ),
    );
  }

  $form['group_user'] = array(
    '#type' => 'container',
    '#prefix' => '<div id="user-container">',
    '#suffix' => '</div>',
  );
  $create_user = isset($values['create_user']) ? (int) $values['create_user'] : $have_user;
  if ($create_user) {
    $account_name = isset($values['name']) ? $values['name'] : $account->name;
    $form['group_user']['name'] = array(
      '#title' => t('User Name'),
      '#type' => 'textfield',
      '#default_value' => $account_name,
      '#required' => TRUE,
    );
    $account_mail = $domain ? $account_name . '@' . $domain->name : '';
    $form['group_user']['mail'] = array(
      '#title' => t('User Email'),
      '#type' => 'textfield',
      '#required' => TRUE,
      '#default_value' => ($account->mail) ? $account->mail : $account_mail,
    );
    $form['group_user']['pass'] = array(
      '#type' => 'password_confirm',
      '#required' => ($account) ? !((bool) $account->uid) : TRUE,
      '#size' => 30,
    );
    $form['group_user']['status'] = array(
      '#type' => 'radios',
      '#title' => t('Status'),
      '#options' => array(t('Blocked'), t('Active')),
      '#default_value' => ($account && (int) $account->uid > 0) ? $account->status : 1,
    );
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('zmt_group_edit_form_submit'),
    '#suffix' => l(t('Cancel'), 'zmt/group'),
  );

  $form['#validate'][] = 'zmt_group_edit_form_validate';

  $path = drupal_get_path('module', 'zmt_core');
  $form['#attached']['js'] = array(
    $path . '/js/zmt.js',
  );
  $path = drupal_get_path('module', 'zmt_group');
  $form['#attached']['css'] = array(
    $path . '/zmt-group.css',
  );

  return $form;
}

/**
 * Group move member submit callback.
 */
function zmt_group_move_member_callback($form, &$form_state) {
  $available_values = isset($_POST['available_members']) ? $_POST['available_members'] : array();
  $form_state['available_values'] = $available_values;

  $member_values = isset($_POST['members']) ? $_POST['members'] : array();
  $form_state['member_values'] = $member_values;

  if (!empty($available_values) || !empty($member_values)) {
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Group select member ajax callback.
 */
function zmt_group_ajax_select_member_callback($form, $form_state) {
  return $form['member_container'];
}

/**
 * Group create user check ajax callback.
 */
function zmt_group_ajax_create_user_checked($form, $form_state) {
  return $form['group_user'];
}

/**
 * Group domain selection ajax callback.
 */
function zmt_group_ajax_select_domain_callback($form, $form_state){
  return $form['member_container'];
}

/**
 * Form validation handler for zmt_group_edit_form().
 *
 * @see zmt_group_edit_form()
 */
function zmt_group_edit_form_validate($form, &$form_state) {
  $account = $form_state['#user'];
  $group = $form_state['#group'];
  $values = &$form_state['values'];

  $domain_id = (int) $values['domain_id'];
  $group_name = check_plain(trim($values['group_name']));
  $id = isset($group->id) ? (int) $group->id : 0;

  $exists = zmt_group_exists($domain_id, $group_name, $id);
  if ($exists) {
    form_set_error('group_name', t('The mailbox name already exists in selected domain.'));
  }

  $domain_id = (int) $values['domain_id'];
  $domain = zmt_domain_load($domain_id);
  if (!$domain) {
    form_set_error('domain_id', t('The domain you selected does not exist.'));
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
 * Zimbra group edit form submit callback.
 */
function zmt_group_edit_form_submit($form, &$form_state) {
  global $user;

  $group = $form_state['#group'];
  $account = $form_state['#user'];
  $values = &$form_state['values'];

  $create_user = isset($values['create_user']) ? (int) $values['create_user'] : 0;
  if ($create_user) {
    $edit = array(
      'name' => check_plain(trim($values['name'])),
      'mail' => check_plain(trim($values['mail'])),
      'status' => (int) $values['status'],
      'init' => 'email address',
    );
    if (isset($values['pass']) && !empty($values['pass'])) {
      $edit['pass'] = trim($values['pass']);
    }
    $role_id = (int) variable_get('zmt_group_role_id', 0);
    if ($role_id) {
      $edit['roles'] = drupal_map_assoc(array($role_id));
    }
    $account = user_save($account, $edit);

    $group->group_uid = $account->uid;
  }

  $group->domain_id = (int) $values['domain_id'];
  $group->name = check_plain(trim($values['group_name']));
  $group->full_name = check_plain(trim($values['full_name']));
  $domain = zmt_domain_load($group->domain_id);

  zmt_group_save($group);

  $members = zmt_group_member_load_multiple_by_group(array($group->id));
  $available_members = array();
  $value_members = is_array($values['members']) ? array_values($values['members']) : array();
  $selected_members = array();
  $member_names = array();
  foreach ($value_members as $member) {
    $member = drupal_json_decode($member);
    if (isset($member['name'])) {
      $member_names[] = $member['name'];
      $selected_members[] = (object) $member;
    }
  }

  foreach ($members as $member) {
    $available_members[] = $member->name;
    if (!in_array($member->name, $member_names)) {
      zmt_group_member_delete($member->id, FALSE);
    }
  }

  foreach ($selected_members as $member) {
    if (!in_array($member->name, $available_members)) {
      $group_member = zmt_group_member_new($member->name);
      $group_member->group_id = $group->id;
      $group_member->member_id = $member->id;
      $group_member->type = $member->type;
      zmt_group_member_save($group_member, FALSE);
    }
  }

  $form_state['redirect'] = 'zmt/group';
}

/**
 * Delete exists zimbra group page callback.
 */
function zmt_group_delete_page($group) {
  drupal_set_title(t('Delete Group'));
  return drupal_get_form('zmt_group_delete_form', $group);
}

/**
 * Form callback: confirmation form for deleting a group.
 *
 * @param $group
 *   The group object to be deleted.
 *
 * @see confirm_form()
 */
function zmt_group_delete_form($form, &$form_state, $group) {
  $form_state['#group'] = $group;

  $form['#submit'][] = 'zmt_group_delete_form_submit';

  $description = '';
  if ((int) $group->delete_dl > 0 && !empty($group->zimbra_id)) {
    $description .= t('Delete this group also delete it in zimbra server.');
  }
  $description .= t(' This action cannot be undone.');

  $form = confirm_form($form,
    t('Are you sure you want to delete group %name?', array('%name' => $group->name)),
    '',
    '<p>' . $description . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for zmt_group_delete_form().
 */
function zmt_group_delete_form_submit($form, &$form_state) {
  $group = $form_state['#group'];
  zmt_group_delete($group->id);
  drupal_set_message(t('%name has been deleted.', array('%name' => $group->name)));
  watchdog('zmt_group', 'Deleted group %name.', array('%name' => $group->name), WATCHDOG_NOTICE);
}

/**
 * Filter group by domain form.
 */
function zmt_group_filter_by_domain_form($form, &$form_state) {
  $form = zmt_filter_by_domain_form($form, $form_state);

  $name = isset($_SESSION['zmt_group_filter_name']) ? $_SESSION['zmt_group_filter_name'] : '';
  $form['name'] = array(
    '#type' => 'textfield',
    '#size' => 15,
    '#default_value' => $name,
    '#attributes' => array(
      'placeholder' => t('Group name'),
    ),
    '#theme_wrappers' => array(),
  );

  $form['apply']['#weight'] = 9;
  $form['reset']['#weight'] = 10;

  $form['apply']['#submit'][] = 'zmt_group_filter_by_domain_apply_callback';
  $form['reset']['#submit'][] = 'zmt_group_filter_by_domain_reset_callback';

  return $form;
}

/**
 * zmt group filter apply callback.
 */
function zmt_group_filter_by_domain_apply_callback($form, &$form_state) {
  $_SESSION['zmt_group_filter_name'] = check_plain($form_state['values']['name']);
}

/**
 * zmt group filter reset callback.
 */
function zmt_group_filter_by_domain_reset_callback($form, &$form_state){
  unset($_SESSION['zmt_group_filter_name']);
}
