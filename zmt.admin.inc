<?php

/**
 * Zimbra multi tenancy setting page callback.
 */
function zmt_setting_form_page() {
  drupal_set_title('Zimbra multi-tenancy settings.');
  return drupal_get_form('zmt_setting_form');
}

/**
 * Zimbra multi tenancy setting form.
 */
function zmt_setting_form($form, &$form_state) {
  $form['zimbra_enable_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable debug'),
    '#default_value' => variable_get('zimbra_enable_debug', 0),
    '#weight' => -8,
  );
  $form['zimbra_item_per_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Item per page'),
    '#default_value' => variable_get('zimbra_item_per_page', 20),
    '#weight' => -7,
  );
  $form['zimbra_item_per_pull'] = array(
    '#type' => 'textfield',
    '#title' => t('Item per pull'),
    '#default_value' => variable_get('zimbra_item_per_pull', 100),
    '#weight' => -6,
  );
  
  $form = system_settings_form($form);
  return $form;
}

/**
 * zmt server pull data page callback.
 */
function zmt_server_pull_data_page() {
  drupal_set_title('Pull data from Zimbra Server');
  return drupal_get_form('zmt_server_pull_data_form');
}

/**
 * zmt server pull data page form.
 */
function zmt_server_pull_data_form($form, &$form_state) {
  $options = array();
  $servers = zmt_server_load_all();
  foreach ($servers as $key => $server) {
    $options[$key] = $server->name;
  }
  $form['server_id'] = array(
    '#type' => 'select',
    '#title' => t('Server'),
    '#options' => $options,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'zmt_ajax_server_select_callback',
      'wrapper' => 'dropdown-tenant-replace',
    ),
  );

  $form['data_type'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Data type'),
    '#required' => TRUE,
    '#options' => array(
      'cos' => t('COS'),
      'domain' => t('Domain'),
      'account' => t('Account'),
      'alias' => t('Alias'),
      'group' => t('Group'),
    ),
  );

  $server_id = isset($form_state['values']['server_id']) ? (int) $form_state['values']['server_id'] : 0;
  $options = array('' => t('- None -'));
  $tenants = zmt_tenant_load_multiple_by_server($server_id);
  foreach ($tenants as $tenant) {
    $options[$tenant->id] = $tenant->name;
  }

  $form['tenant_id'] = array(
    '#type' => 'select',
    '#title' => t('Tenant'),
    '#options' => $options,
    '#prefix' => '<div id="dropdown-tenant-replace">',
    '#suffix' => '</div>',
  );

  $form['actions']['pull'] = array(
    '#type' => 'submit',
    '#value' => t('Pull data'),
    '#ajax' => array(
      'callback' => 'zmt_ajax_pull_data_callback',
    ),
    '#suffix' => l(t('Cancel'), 'zmt/server'),
  );
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'zmt') .'/js/zmt.js',
  );

  return $form;
}

/**
 * ajax pull data server selection callback.
 */
function zmt_ajax_server_select_callback($form, $form_state) {
  return $form['tenant_id'];
}

/**
 * Pull data submit callback.
 */
function zmt_ajax_pull_data_callback($form, &$form_state) {
  global $user;

  module_load_include('inc', 'zmt', 'includes/zmt.zimbra');
  $values = $form_state['values'];
  $server_id = (int) $values['server_id'];
  $tenant_id = (int) $values['tenant_id'];
  $data_types = $values['data_type'];

  $server = zmt_server_load($server_id);
  $excludes = explode(PHP_EOL, $server->exclude_mailbox);
  $excludes = array_map('trim', $excludes);

  $api = zimbra_admin_api_by_server($server_id);
  $zimbra_domains = zimbra_get_all_domain($api);

  $types = array_filter($data_types);
  foreach ($types as $type) {
    switch ($type) {
      case 'cos':
        $zimbra_coses = zimbra_get_all_cos($api);
        if (!empty($zimbra_coses)) {
          _zmt_ui_update_cos_data($zimbra_coses, $server->id);
        }
        break;
      case 'domain':
        if (!empty($zimbra_domains)) {
          _zmt_ui_update_domain_data($zimbra_domains, $server->id, $tenant_id);
        }
        break;
      case 'account':
        if (!empty($zimbra_domains)) {
          _zmt_ui_update_account_data($api, $zimbra_domains, $excludes);
        }
        break;
      case 'alias':
        if (!empty($zimbra_domains)) {
          _zmt_ui_update_alias_data($api, $zimbra_domains, $excludes);
        }
        break;
      case 'group':
        if (!empty($zimbra_domains)) {
          _zmt_ui_update_group_data($api, $zimbra_domains);
        }
        break;
    }
  }

  $commands = array();
  $commands[] = zmt_ajax_command_redirect('zmt/server');
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

function _zmt_ui_update_cos_data(array $zimbra_coses, $server_id = 0) {
  $count = 0;
  $mb = 1024 * 1024;
  foreach ($zimbra_coses as $zimbra_id => $zimbra_cos) {
    $exists = zmt_cos_exists($zimbra_id);
    if (!$exists) {
      $cos = zmt_cos_new($zimbra_cos->name);
      $cos->server_id = $server_id;
      $attrs = !empty($zimbra_cos->a) ? $zimbra_cos->a : array();
      if (is_array($attrs)) {
        foreach ($attrs as $attr) {
          if ($attr->n === 'description') {
            $cos->description = $attr->_content;
          }
          if ($attr->n === 'zimbraMailQuota') {
            $cos->mail_quota = (int) $attr->_content / $mb;
          }
        }
      }
      $cos->zimbra_id = $zimbra_id;
      zmt_cos_save($cos, FALSE);
      $count++;
    }
  }
  drupal_set_message($count . ' coses were updated.');
  return $count;
}

/**
 * Update pull domain data.
 *
 * @param $zimbra_domains
 *   An array of zimbra domains.
 * @param $server_id
 *   An ID of zmt server.
 * @param $tenant_id
 *   An ID of zmt tenant.
 * @return
 *   Total number of updated domains.
 */
function _zmt_ui_update_domain_data(array $zimbra_domains, $server_id = 0, $tenant_id = 0) {
  $count = 0;
  foreach ($zimbra_domains as $zimbra_id => $zimbra_domain) {
    $exists = zmt_domain_exists($zimbra_domain->name);
    if (!$exists) {
      $domain = zmt_domain_new($zimbra_domain->name);
      $domain->server_id = $server_id;
      $domain->zimbra_id = $zimbra_id;
      $tenant = zmt_tenant_load($tenant_id);
      if ($tenant) {
        $domain->tenant_id = $tenant->id;
        $domain->cos_id = $tenant->cos_id;
        $tenant_user = user_load($tenant->tenant_uid);
        if ($tenant_user && (int) $tenant_user->uid > 0) {
          $domain->uid = (int) $tenant_user->uid;
        }
      }
      zmt_domain_save($domain, FALSE);
      $count++;
    }
  }
  drupal_set_message($count . ' domains were updated.');
  return $count;
}

/**
 * Update pull account data.
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param $zimbra_domains
 *   An array of zimbra domains.
 * @return
 *   Total number of updated accounts.
 */
function _zmt_ui_update_account_data(ZimbraAdminApi $api, array $zimbra_domains, array $excludes = array()) {
  $count = 0;
  foreach ($zimbra_domains as $zimbra_domain) {
    $domain_name = $zimbra_domain->name;
    $domain = zmt_domain_load_by_name($domain_name);
    if ($domain) {
      $pager = array();
      $limit = variable_get('zimbra_item_per_pull', 100);
      $offset = 0;
      do {
        $zmbra_accounts = zimbra_list_account($api, $domain_name, $limit, $offset, $pager);
        $offset += $limit;
        foreach ($zmbra_accounts as $zimbra_id => $zmbra_account) {
          $account_name = substr($zmbra_account->name, 0, (int) strpos($zmbra_account->name, '@'));
          $match = zmt_is_matches($excludes, $account_name);
          $exists = zmt_account_exists($domain->id, $account_name);
          if (!$exists && !$match) {
            $account = zmt_account_new($account_name);
            $account->domain_id = $domain->id;
            $account->zimbra_id = $zimbra_id;
            $attrs = isset($zmbra_account->a) ? $zmbra_account->a : array();
            if (is_array($attrs)) {
              foreach ($attrs as $attr) {
                if ($attr->n === 'displayName') {
                  $account->full_name = isset($attr->_content) ? $attr->_content : '';
                }
                if ($attr->n === 'zimbraAccountStatus') {
                  $account->status = isset($attr->_content) ? $attr->_content : '';
                }
                if ($attr->n === 'title') {
                  $account->title = isset($attr->_content) ? $attr->_content : '';
                }
                if ($attr->n === 'mobile') {
                  $account->mobile = isset($attr->_content) ? $attr->_content : '';
                }
                if ($attr->n === 'telephoneNumber') {
                  $account->telephone = isset($attr->_content) ? $attr->_content : '';
                }
                if ($attr->n === 'company') {
                  $account->company = isset($attr->_content) ? $attr->_content : '';
                }
              }
            }
            zmt_account_save($account, FALSE);
            $count++;
          }
        }
      } while (isset($pager['more']) && $pager['more'] === TRUE);
    }
  }
  drupal_set_message($count . ' accounts were updated.');
  return $count;
}

/**
 * Update pull alias data.
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param $zimbra_domains
 *   An array of zimbra domains.
 * @return
 *   Total number of updated aliases.
 */
function _zmt_ui_update_alias_data(ZimbraAdminApi $api, array $zimbra_domains, array $excludes = array()) {
  $count = 0;
  foreach ($zimbra_domains as $zimbra_domain) {
    $domain_name = $zimbra_domain->name;
    $domain = zmt_domain_load_by_name($domain_name);
    if ($domain) {
      $pager = array();
      $limit = variable_get('zimbra_item_per_pull', 100);
      $offset = 0;
      do {
        $zimbra_aliases = zimbra_list_alias($api, $domain_name, $limit, $offset, $pager);
        $offset += $limit;
        foreach ($zimbra_aliases as $zimbra_id => $zmbra_alias) {
          $alias_name = substr($zmbra_alias->name, 0, (int) strpos($zmbra_alias->name, '@'));
          $account_name = substr($zmbra_alias->account_name, 0, (int) strpos($zmbra_alias->account_name, '@'));
          $account = zmt_account_load_by_name($domain->id, $account_name);

          $match = zmt_is_matches($excludes, $zmbra_alias->name);
          $exists = zmt_alias_exists($domain->id, $alias_name);
          if (!$exists && !$match && $account) {
            $alias = zmt_alias_new($alias_name);
            $alias->domain_id = $domain->id;
            $alias->account_id = $account->id;
            $alias->zimbra_target_id = $account->zimbra_id;

            zmt_alias_save($alias, FALSE);
            $count++;
          }
        }
      } while (isset($pager['more']) && $pager['more'] === TRUE);
    }
  }
  drupal_set_message($count . ' aliases were updated.');
  return $count;
}

/**
 * Update pull group data.
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param $zimbra_domains
 *   An array of zimbra domains.
 * @return
 *   Total number of updated groups.
 */
function _zmt_ui_update_group_data(ZimbraAdminApi $api, array $zimbra_domains) {
  $count = 0;
  foreach ($zimbra_domains as $zimbra_domain) {
    $domain_name = $zimbra_domain->name;
    $domain = zmt_domain_load_by_name($domain_name);
    if ($domain) {
      $zimbra_dls = zimbra_get_all_dl($api, $domain_name);
      foreach ($zimbra_dls as $zimbra_id => $dl) {
        $group_name = substr($dl->name, 0, (int) strpos($dl->name, '@'));
        $exists = zmt_group_exists($domain->id, $group_name);
        if (!$exists) {
          $group = zmt_group_new($group_name);
          $group->full_name = $group_name;
          $group->domain_id = $domain->id;
          $group->zimbra_id = $zimbra_id;
          $attrs = isset($dl->a) ? $dl->a : array();
          if (is_array($attrs)) {
            foreach ($attrs as $attr) {
              if ($attr->n === 'displayName') {
                $group->full_name = isset($attr->_content) ? $attr->_content : '';
              }
            }
          }
          zmt_group_save($group, FALSE);

          $members = isset($dl->dlm) ? $dl->dlm : zimbra_get_dl_members($api, $dl->name);
          if (!is_array($members)) {
            $members = array($members);
          }

          if (!empty($members)) {
            $zmt_members = array();
            $accounts = zmt_account_load_multiple_by_domain($domain);
            foreach ($accounts as $account) {
              $mailbox = $account->name . '@' . $domain->name;
              $member = array(
                'name' => $mailbox,
                'id' => (int) $account->id,
                'type' => GROUP_MEMBER_ACCOUNT,
              );
              $zmt_members[$mailbox] = (object) $member;
            }
            $aliases = zmt_alias_load_multiple_by_domain($domain);
            foreach ($aliases as $alias) {
              $mailbox = $alias->name . '@' . $domain->name;
              $member = array(
                'name' => $mailbox,
                'id' => (int) $alias->id,
                'type' => GROUP_MEMBER_ALIAS,
              );
              $zmt_members[$mailbox] = (object) $member;
            }
            $groups = zmt_group_load_multiple_by_domain($domain, $group->id);
            foreach ($groups as $gr) {
              $mailbox = $gr->name . '@' . $domain->name;
              $member = array(
                'name' => $mailbox,
                'id' => (int) $gr->id,
                'type' => GROUP_MEMBER_DL,
              );
              $zmt_members[$mailbox] = (object) $member;
            }

            foreach ($members as $member) {
              $member_name = isset($member->_content) ? $member->_content : '';
              $zmt_member = isset($zmt_members[$member_name]) ? $zmt_members[$member_name] : NULL;
              if ($zmt_member) {
                $group_member = zmt_group_member_new($member_name);
                $group_member->group_id = $group->id;
                $group_member->member_id = $zmt_member->id;
                $group_member->type = $zmt_member->type;
                zmt_group_member_save($group_member, FALSE);
              }
            }
          }
          $count++;
        }
      }
    }
  }
  drupal_set_message($count . ' groups were updated.');
  return $count;
}
