<?php

module_load_include('inc', 'zmt', 'includes/zmt.zimbra.api');

/**
 * Initialize Zimbra API
 *
 * @param  string $location
 *   Zimbra service location.
 * @return ZimbraAdminApi
 */
function zimbra_admin_api($location = 'https://localhost:7071/service/admin/soap') {
  $cache = &drupal_static(__FUNCTION__, NULL);
  if (!isset($cache[$location])) {
    $debug = (bool) variable_get('zimbra_enable_debug', FALSE);
    $cache[$location] = new ZimbraAdminApi($location, $debug);
  }
  return $cache[$location];
}

/**
 * Test connection to Zimbra service
 *
 * @param  string $location
 *   Zimbra service location.
 * @param  string $user
 *   Zimbra admin user.
 * @param  string $password
 *   Zimbra admin password.
 * @return bool
 */
function zimbra_test_connection(
  $location = 'https://localhost:7071/service/admin/soap',
  $user = '',
  $password = ''
) {
  $authenticated = FALSE;
  $api = zimbra_admin_api($location);
  try {
    $authToken = $api->authByName($user, $password);
    $authenticated = !empty($authToken);
    if ($authenticated) {
      $sessKey = md5($api->location() . $user . $password) . 'authToken';
      $_SESSION[$sessKey] = $authToken;
    }
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
  }
  return $authenticated;
}

/**
 * Initialize Zimbra API by tenant
 *
 * @param  string $tenant_id
 *   Tenant id.
 * @return Zimbra Admin API
 */
function zimbra_admin_api_by_tenant($tenant_id) {
  $server = zmt_ternant_get_server($tenant_id);
  $server_id = $server ? (int) $server->id : 0;
  return zimbra_admin_api_by_server($server_id);
}

/**
 * Initialize Zimbra API by domain
 *
 * @param  string $domain_id
 *   Domain id.
 * @return Zimbra Admin API
 */
function zimbra_admin_api_by_domain($domain_id) {
  $server = zmt_domain_get_server($domain_id);
  $server_id = $server ? (int) $server->id : 0;
  return zimbra_admin_api_by_server($server_id);
}

/**
 * Initialize Zimbra API by server
 *
 * @param  string $server_id
 *   Server id.
 * @return Zimbra Admin API
 */
function zimbra_admin_api_by_server($server_id) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  if (!isset($cache[$server_id])) {
    $server = zmt_server_load($server_id);
    $location = $server ? $server->service_location : 'https://localhost:7071/service/admin/soap';
    $user = $server ? $server->admin_user : '';
    $password = $server ? $server->admin_password : '';
    $password = zmt_password_decrypt($server->admin_password);

    $api = zimbra_admin_api($location);
    $authenticated = zimbra_authentication($api, $user, $password);
    if (!$authenticated) {
      $message = 'Connection or authentication to Zimbra server failed! <br />';
      $message .= 'Please contact to administrator to report this error.';
      drupal_set_message(t($message), 'error');
      drupal_access_denied();
      exit;
    }
    $cache[$server_id] = $api;
  }
  return $cache[$server_id];
}

/**
 * Zimbra authentication
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $user
 *   Zimbra admin user.
 * @param  string $password
 *   Zimbra admin password.
 * @return mix
 */
function zimbra_authentication(ZimbraAdminApi $api, $user, $password = '') {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $user . $password);
  if (!isset($cache[$cache_key])) {
    $authenticated = FALSE;
    $sessKey = $cache_key . 'authToken';
    $sessAuthToken = isset($_SESSION[$sessKey]) ? $_SESSION[$sessKey] : '';
    if (!empty($sessAuthToken)) {
      try {
        $authToken = $api->authByToken($user, $sessAuthToken);
        $authenticated = ($authToken === $sessAuthToken);
      }
      catch (Exception $ex) {
        watchdog('zimbra', $ex->__toString());
      }
    }
    if(!$authenticated) {
      try {
        $authToken = $api->authByName($user, $password);
        $authenticated = !empty($authToken);
        if ($authenticated) {
          $_SESSION[$sessKey] = $authToken;
        }
      }
      catch (Exception $ex) {
        watchdog('zimbra', $ex->__toString());
      }
    }
    $cache[$cache_key] = $authenticated;
  }
  return $cache[$cache_key];
}


/**
 * Gets all classes of service
 *
 * @param  ZimbraAdminApi $api Zimbra admin api.
 * @return mix
 */
function zimbra_get_all_cos(ZimbraAdminApi $api) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location());
  if (!isset($cache[$cache_key])) {
    $all_cos = array();
    try {
      $result = $api->getAllCos();
      if (isset($result->cos)) {
        $arr_cos = is_array($result->cos) ? $result->cos : array($result->cos);
        foreach ($arr_cos as $cos) {
          $all_cos[$cos->id] = $cos->name;
        }
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $all_cos;
  }
  return $cache[$cache_key];
}

/**
 * Get information of Zimbra classes of service
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $name
 *   COS name.
 * @return mix
 */
function zimbra_get_cos(ZimbraAdminApi $api, $name) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $name);
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->getCos(new CosSelector('name', $name));
      $cache[$cache_key] = isset($result->cos[0]) ? $result->cos[0] : $result;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return NULL;
    }
  }
  return $cache[$cache_key];
}

/**
 * Create Zimbra classes of service
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $name
 *   COS name.
 * @param  array  $attrs
 *   COS attributes.
 * @return mix
 */
function zimbra_create_cos(ZimbraAdminApi $api, $name, array $attrs = array()) {
  try {
    $a = array();
    foreach ($attrs as $key => $value) {
      $a[] = new KeyValuePair($key, $value);
    }
    $result = $api->createCos($name, $a);
    return isset($result->cos[0]) ? $result->cos[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return NULL;
  }
}

/**
 * Modify Zimbra classes of service
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $id
 *   Zimbra COS ID.
 * @param  array  $attrs
 *   COS attributes.
 * @return mix
 */
function zimbra_modify_cos(ZimbraAdminApi $api, $id, array $attrs = array()) {
  try {
    $a = array();
    foreach ($attrs as $key => $value) {
      $a[] = new KeyValuePair($key, $value);
    }
    $result = $api->modifyCos($id, $a);
    return isset($result->cos[0]) ? $result->cos[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return NULL;
  }
}

/**
 * Rename Zimbra classes of service
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $zimbra_id
 *   Zimbra COS ID.
 * @param  string $new_name
 *   New COS name.
 * @return mix
 */
function zimbra_rename_cos(ZimbraAdminApi $api, $zimbra_id, $new_name) {
  try {
    $result = $api->renameCos($zimbra_id, $new_name);
    return isset($result->cos) ? $result->cos : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return NULL;
  }
}

/**
 * Delete Zimbra classes of service
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $zimbra_id
 *   COS Zimbra ID.
 * @return mix
 */
function zimbra_delete_cos(ZimbraAdminApi $api, $zimbra_id) {
  try {
    return $api->deleteCos($zimbra_id);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return false;
  }
}

/**
 * Gets all Zimbra domains
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @return mix
 */
function zimbra_get_all_domain(ZimbraAdminApi $api) {
  $cache = &drupal_static(__FUNCTION__, array());
  $cache_key = md5($api->location());
  if (!isset($cache[$cache_key])) {
    $all_domain = array();
    try {
      $result = $api->getAllDomains();
      if (isset($result->domain)) {
        $domains = is_array($result->domain) ? $result->domain : array($result->domain);
        foreach ($domains as $domain) {
          $all_domain[$domain->id] = $domain;
        }
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $all_domain;
  }
  return $cache[$cache_key];
}

/**
 * Get information of Zimbra domain
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $name
 *   Domain name.
 * @return mix
 */
function zimbra_get_domain(ZimbraAdminApi $api, $name) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $name);
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->getDomain(new DomainSelector('name', $name));
      $cache[$cache_key] = isset($result->domain[0]) ? $result->domain[0] : $result;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return NULL;
    }
  }
  return $cache[$cache_key];
}

/**
 * Create Zimbra domain
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $name
 *   Domain name.
 * @param  array  $attrs
 *   Domain attributes.
 * @return mix
 */
function zimbra_create_domain(ZimbraAdminApi $api, $name, array $attrs = array()) {
  try {
    $a = array();
    foreach ($attrs as $key => $value) {
      $a[] = new KeyValuePair($key, $value);
    }
    $result = $api->createDomain($name, $a);
    return isset($result->domain[0]) ? $result->domain[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return NULL;
  }
}

/**
 * Modify Zimbra domain
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $id
 *   Zimbra domain ID.
 * @param  array  $attrs
 *   Domain attributes.
 * @return mix
 */
function zimbra_modify_domain(ZimbraAdminApi $api, $id, array $attrs = array()) {
  try {
    $a = array();
    foreach ($attrs as $key => $value) {
      $a[] = new KeyValuePair($key, $value);
    }
    $result = $api->modifyDomain($id, $a);
    return isset($result->domain[0]) ? $result->domain[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return NULL;
  }
}

/**
 * Delete Zimbra domain
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $zimbra_id
 *   Domain Zimbra ID.
 * @return mix
 */
function zimbra_delete_domain(ZimbraAdminApi $api, $zimbra_id) {
  try {
    return $api->deleteDomain($zimbra_id);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return false;
  }
}

/**
 * Gets all Zimbra distribution lists
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $domain
 *   Domain name.
 * @return array of distribution lists
 */
function zimbra_get_all_dl(ZimbraAdminApi $api, $domain = NULL) {
  $cache = &drupal_static(__FUNCTION__, array());
  $cache_key = md5($api->location() . $domain);
  if (!isset($cache[$cache_key])) {
    $all_dl = array();
    try {
      if (!empty($domain)) {
        $result = $api->getAllDistributionLists(new DomainSelector('name', $domain));
      }
      else {
        $result = $api->getAllDistributionLists();
      }
      if (isset($result->dl)) {
        $dls = is_array($result->dl) ? $result->dl : array($result->dl);
        foreach ($dls as $dl) {
          $all_dl[$dl->id] = $dl;
        }
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $all_dl;
  }
  return $cache[$cache_key];
}

/**
 * Lists Zimbra distribution lists
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $domain
 *   The domain name to limit the search to.
 * @param  int $limit
 *   The maximum number of accounts to return (0 is default and means all)
 * @param  int $offset
 *   The starting offset
 * @return array of distribution lists
 */
function zimbra_list_dl(ZimbraAdminApi $api, $domain = NULL, $limit = NULL, $offset = NULL, &$pager = array()) {
  $cache = &drupal_static(__FUNCTION__, array());
  $cache_key = md5($api->location() . '-' . $domain . '-' . $limit . '-' . $offset);
  if (!isset($cache[$cache_key])) {
    $all_dls = array();
    try {
      $attrs = array(
        'displayName',
        'zimbraId',
        'zimbraMailHost',
        'uid',
        'description',
        'zimbraIsAdminGroup',
        'zimbraMailStatus',
        'zimbraIsDelegatedAdminAccount',
        'zimbraIsAdminAccount',
        'zimbraIsSystemResource',
      );
      $result = $api->searchDirectory(
        '', NULL, $limit, $offset, $domain, NULL, NULL,
        'distributionlists', NULL, NULL, NULL, implode(',', $attrs)
      );
      $pager = array(
        'num' => isset($result->num) ? (int) $result->num : 0,
        'more' => (bool) $result->more,
        'total' => (int) $result->searchTotal,
        'limit' => (int) $limit,
        'offset' => (int) $offset,
      );
      if(isset($result->dl)) {
        $dls = is_array($result->dl) ? $result->dl : array($result->dl);
        foreach ($dls as $dl) {
          $all_accounts[$dl->id] = $dl;
        }
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $all_dls;
  }
  return $cache[$cache_key];
}

/**
 * Get information of Zimbra distribution list
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $name
 *   Distribution list name.
 * @return mix
 */
function zimbra_get_dl(ZimbraAdminApi $api, $name, &$members = array()) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $name);
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->getDistributionList(new DistributionListSelector('name', $name));
      $cache[$cache_key] = isset($result->dl[0]) ? $result->dl[0] : $result;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return NULL;
    }
  }
  return $cache[$cache_key];
}

/**
 * Get Zimbra distribution list members
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $name
 *   Distribution list name.
 * @return array
 */
function zimbra_get_dl_members(ZimbraAdminApi $api, $name) {
  $cache = &drupal_static(__FUNCTION__, array());
  $cache_key = md5($api->location() . $name);
  if (!isset($cache[$cache_key])) {
    $dl = zimbra_get_dl($api, $name);
    $members = array();
    if (isset($dl->dlm) && is_array($dl->dlm)) {
      foreach ($dl->dlm as $member) {
        $members[] = isset($member->_content) ? $member->_content : '';
      }
    }
    $cache[$cache_key] = $members;
  }
  return $cache[$cache_key];
}

/**
 * Create Zimbra distribution list
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $name
 *   Distribution list name.
 * @param  array  $attrs
 *   Domain attributes.
 * @return mix
 */
function zimbra_create_dl(ZimbraAdminApi $api, $name, array $attrs = array()) {
  try {
    $a = array();
    foreach ($attrs as $key => $value) {
      $a[] = new KeyValuePair($key, $value);
    }
    $result = $api->createDistributionList($name, NULL, $a);
    return isset($result->dl[0]) ? $result->dl[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return NULL;
  }
}

/**
 * Modify Zimbra distribution list
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $zimbra_id
 *   Zimbra ID.
 * @param  array  $attrs
 *   Dl attributes.
 * @return mix
 */
function zimbra_modify_dl(ZimbraAdminApi $api, $zimbra_id, array $attrs = array()) {
  try {
    $a = array();
    foreach ($attrs as $key => $value) {
      $a[] = new KeyValuePair($key, $value);
    }
    $result = $api->modifyDistributionList($zimbra_id, $a);
    return isset($result->dl[0]) ? $result->dl[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return NULL;
  }
}

/**
 * Rename Zimbra distribution list
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $zimbra_id
 *   Zimbra ID.
 * @param  string $new_name
 *   New dl name.
 * @return mix
 */
function zimbra_rename_dl(ZimbraAdminApi $api, $zimbra_id, $new_name) {
  try {
    $result = $api->renameDistributionList($zimbra_id, $new_name);
    return isset($result->dl[0]) ? $result->dl[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return NULL;
  }
}

/**
 * Adding members to a distribution list.
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $zimbra_id
 *   Value of zimbra identify.
 * @param  array  $members
 *   Distribution list members.
 * @return mix
 */
function zimbra_add_dl_members(ZimbraAdminApi $api, $zimbra_id, array $members = array()) {
  try {
    return $api->addDistributionListMember($zimbra_id, $members);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return NULL;
  }
}

/**
 * Remove distribution list members.
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $zimbra_id
 *   Value of zimbra identify.
 * @param  array  $members
 *   Distribution list members.
 * @return mix
 */
function zimbra_remove_dl_members(ZimbraAdminApi $api, $id, array $members) {
  try {
    return $api->removeDistributionListMember($id, $members);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return NULL;
  }
}

/**
 * Delete Zimbra distribution list
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $zimbra_id
 *   Distribution list Zimbra ID.
 * @return mix
 */
function zimbra_delete_dl(ZimbraAdminApi $api, $zimbra_id) {
  try {
    return $api->deleteDistributionList($zimbra_id);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return false;
  }
}

/**
 * Gets all Zimbra account
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $domain_name
 *   Domain name.
 * @param  string $server_name
 *   Server name
 * @return mix
 */
function zimbra_get_all_account(ZimbraAdminApi $api, $domain_name = '', $server_name = '') {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $domain_name . '-' . $server_name);
  if (!isset($cache[$cache_key])) {
    $all_account = array();
    try {
      $server = !empty($server_name) ? new ServerSelector('name', $server_name) : NULL;
      $domain = !empty($domain_name) ? new DomainSelector('name', $domain_name) : NULL;
      $result = $api->getAllAccounts($server, $domain);
      if (isset($result->account)) {
        $accounts = is_array($result->account) ? $result->account : array($result->account);
        foreach ($accounts as $account) {
          $all_account[$account->id] = $account;
        }
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $all_account;
  }
  return $cache[$cache_key];
}

/**
 * Lists Zimbra account
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $domain
 *   The domain name to limit the search to.
 * @param  int $limit
 *   The maximum number of accounts to return (0 is default and means all)
 * @param  int $offset
 *   The starting offset
 * @return mix
 */
function zimbra_list_account(ZimbraAdminApi $api, $domain = NULL, $limit = NULL, $offset = NULL, &$pager = array()) {
  $cache = &drupal_static(__FUNCTION__, array());
  $cache_key = md5($api->location() . '-' . $domain . '-' . $limit . '-' . $offset);
  if (!isset($cache[$cache_key])) {
    $all_accounts = array();
    try {
      $attrs = array(
        'displayName',
        'zimbraId',
        'zimbraAliasTargetId',
        'cn',
        'sn',
        'zimbraMailHost',
        'uid',
        'zimbraCOSId',
        'zimbraAccountStatus',
        'zimbraMailQuota',
        'zimbraLastLogonTimestamp',
        'description',
        'zimbraIsSystemAccount',
        'zimbraIsDelegatedAdminAccount',
        'zimbraIsAdminAccount',
        'zimbraIsSystemResource',
        'zimbraAuthTokenValidityValue',
        'zimbraIsExternalVirtualAccount',
        'zimbraMailStatus',
        'zimbraIsAdminGroup',
        'zimbraCalResType',
        'zimbraDomainType',
        'zimbraDomainName',
        'zimbraDomainStatus',
      );
      $result = $api->searchDirectory(
        '', NULL, $limit, $offset, $domain, NULL, NULL,
        'accounts', NULL, NULL, NULL, implode(',', $attrs)
      );
      $pager = array(
        'num' => isset($result->num) ? (int) $result->num : 0,
        'more' => (bool) $result->more,
        'total' => (int) $result->searchTotal,
        'limit' => (int) $limit,
        'offset' => (int) $offset,
      );
      if(isset($result->account)) {
        $accounts = is_array($result->account) ? $result->account : array($result->account);
        foreach ($accounts as $account) {
          $all_accounts[$account->id] = $account;
        }
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $all_accounts;
  }
  return $cache[$cache_key];
}

/**
 * Get information of Zimbra account
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $name
 *   The name used to identify the account.
 * @return mix
 */
function zimbra_get_account(ZimbraAdminApi $api, $name) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $name);
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->getAccount(new AccountSelector('name', $name));
      $cache[$cache_key] = isset($result->account[0]) ? $result->account[0] : $result;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return NULL;
    }
  }
  return $cache[$cache_key];
}

/**
 * Create Zimbra account
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $name
 *   New account's name. Must include domain (uid@name), and domain specified in name must exist.
 * @param  string $password
 *   New account's password.
 * @param  array  $attrs
 *   Account attributes.
 * @return mix
 */
function zimbra_create_account(ZimbraAdminApi $api, $name, $password, array $attrs = array()) {
  try {
    $a = array();
    foreach ($attrs as $key => $value) {
      $a[] = new KeyValuePair($key, $value);
    }
    $result = $api->createAccount($name, $password, $a);
    return isset($result->account[0]) ? $result->account[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Modify Zimbra account
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $id
 *   Zimbra ID of account.
 * @param  array  $attrs
 *   Attributes.
 * @return mix
 */
function zimbra_modify_account(ZimbraAdminApi $api, $id, array $attrs = array()) {
  try {
    $a = array();
    foreach ($attrs as $key => $value) {
      $a[] = new KeyValuePair($key, $value);
    }
    $result = $api->modifyAccount($id, $a);
    return isset($result->account[0]) ? $result->account[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Rename Zimbra account
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $id
 *   Zimbra ID of account.
 * @param  string $name
 *   New account name.
 * @return mix
 */
function zimbra_rename_account(ZimbraAdminApi $api, $id, $name) {
  try {
    $result = $api->renameAccount($id, $name);
    return isset($result->account[0]) ? $result->account[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Delete Zimbra account
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $zimbra_id
 *   Account Zimbra ID.
 * @return mix
 */
function zimbra_delete_account(ZimbraAdminApi $api, $zimbra_id){
  try {
    return $api->deleteAccount($zimbra_id);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Lists Zimbra account alias
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $domain
 *   Domain name.
 * @return array
 */
function zimbra_list_alias(ZimbraAdminApi $api, $domain = NULL, $limit = NULL, $offset = NULL, &$pager = array()) {
  $cache = &drupal_static(__FUNCTION__, array());
  $cache_key = md5($api->location() . '-' . $domain);
  if (!isset($cache[$cache_key])) {
    $all_alias = array();
    try {
      $attrs = array(
        'zimbraAliasTargetId',
        'zimbraId',
        'targetName',
        'uid',
        'type',
        'description',
        'zimbraIsDelegatedAdminAccount',
        'zimbraIsAdminAccount',
        'zimbraIsSystemResource'
      );
      $result = $api->searchDirectory(
        '', NULL, $limit, $offset, $domain, NULL, NULL,
        'aliases', NULL, NULL, NULL, implode(',', $attrs)
      );
      $pager = array(
        'num' => isset($result->num) ? (int) $result->num : 0,
        'more' => (bool) $result->more,
        'total' => (int) $result->searchTotal,
        'limit' => (int) $limit,
        'offset' => (int) $offset,
      );
      if(isset($result->alias)) {
        $aliases = is_array($result->alias) ? $result->alias : array($result->alias);
        foreach ($aliases as $alias) {
          $mail_alias = new stdClass;
          $mail_alias->id = $alias->id;
          $mail_alias->name = $alias->name;
          $mail_alias->account_id = '';
          $mail_alias->account_name = $alias->targetName;
          $mail_alias->domain = $domain;
          $attrs = isset($alias->a) ? $alias->a : array();
          foreach ($attrs as $attr) {
            if ($attr->n === 'zimbraAliasTargetId') {
              $mail_alias->account_id = isset($attr->_content) ? $attr->_content : '';
            }
          }
          $all_alias[$alias->id] = $mail_alias;
        }
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $all_alias;
  }
  return $cache[$cache_key];
}

/**
 * Add Zimbra account alias
 *
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $id
 *   Value of zimbra identify.
 * @param  string $alias
 *   Account alias.
 * @return mix
 */
function zimbra_add_account_alias(ZimbraAdminApi $api, $id, $alias) {
  try {
    return $api->addAccountAlias($id, $alias);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return false;
  }
}

/**
 * Remove Zimbra account alias.
 * 
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $alias
 *   Account alias.
 * @param  string $id
 *   Zimbra ID.
 * @return mix
 */
function zimbra_remove_account_alias(ZimbraAdminApi $api, $alias, $id = '') {
  try {
    return $api->removeAccountAlias($alias, $id);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return false;
  }
}

/**
 * Set password.
 * 
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param  string $id
 *   Zimbra ID.
 * @param  string $password
 *   New password.
 * @return mix
 */
function zimbra_set_password(ZimbraAdminApi $api, $id, $password) {
  try {
    return $api->setPassword($id, $password);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Get all installed skins on the server.
 * 
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @return An array of skin
 */
function zimbra_get_all_skins(ZimbraAdminApi $api) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location());
  if (!isset($cache[$cache_key])) {
    try {
      $all_skins = array();
      $result = $api->getAllSkins();
      if (isset($result->skin)) {
        $skins = is_array($result->skin) ? $result->skin : array($result->skin);
        foreach ($skins as $skin) {
          $all_skins[] = $skin->name;
        }
      }
      $cache[$cache_key] = $all_skins;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return NULL;
    }
  }
  return $cache[$cache_key];
}

/**
 * Get quota usage.
 * 
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @return mix
 */
function zimbra_get_quota_usage(ZimbraAdminApi $api, $domain = NULL, $limit = NULL, $offset = NULL, &$pager = array()) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location());
  if (!isset($cache[$cache_key])) {
    $usages = array();
    try {
      $result = $api->getQuotaUsage($domain, NULL, $limit, $offset, 'totalUsed');
      $pager = array(
        'more' => (bool) $result->more,
        'total' => (int) $result->searchTotal,
        'limit' => (int) $limit,
        'offset' => (int) $offset,
      );
      if (!empty($result->account)) {
        if (is_array($result->account)) {
          foreach ($result->account as $usage) {
            $usages[] = $usage;
          }
        }
        else {
          $usages[] = $result->account;
        }
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $usages;
  }
  return $cache[$cache_key];
}

/**
 * Get version information.
 * 
 * @param  ZimbraAdminApi $api
 *   Zimbra admin api.
 * @return mix
 */
function zimbra_get_version_info(ZimbraAdminApi $api) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location());
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->getVersionInfo();
      $cache[$cache_key] = isset($result->info) ? $result->info : $result;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return NULL;
    }
  }
  return $cache[$cache_key];
}
