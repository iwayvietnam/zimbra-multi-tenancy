<?php

module_load_include('inc', 'zmt_core', 'includes/zmt.zimbra.api');

/**
 * Initialize Zimbra API
 *
 * @param string $location
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
 * @param string $location
 *   Zimbra service location.
 * @param string $user
 *   Zimbra admin user.
 * @param string $password
 *   Zimbra admin password.
 * @return authentication token
 */
function zimbra_test_connection(
  $location = 'https://localhost:7071/service/admin/soap',
  $user = '',
  $password = ''
) {
  $api = zimbra_admin_api($location);
  return zimbra_authentication_by_name($api, $user, $password);
}

/**
 * Initialize Zimbra API by tenant
 *
 * @param string $tenant_id
 *   Tenant id.
 * @return ZimbraAdminApi
 */
function zimbra_admin_api_by_tenant($tenant_id) {
  $server = zmt_ternant_get_server($tenant_id);
  $server_id = $server ? (int) $server->id : 0;
  return zimbra_admin_api_by_server($server_id);
}

/**
 * Initialize Zimbra API by domain
 *
 * @param string $domain_id
 *   Domain id.
 * @return ZimbraAdminApi
 */
function zimbra_admin_api_by_domain($domain_id) {
  $server = zmt_domain_get_server($domain_id);
  $server_id = $server ? (int) $server->id : 0;
  return zimbra_admin_api_by_server($server_id);
}

/**
 * Initialize Zimbra API by server
 *
 * @param string $server_id
 *   Server id.
 * @return ZimbraAdminApi
 */
function zimbra_admin_api_by_server($server_id) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  if (!isset($cache[$server_id])) {
    $server = zmt_server_load($server_id);
    $location = $server ? $server->service_location : 'https://localhost:7071/service/admin/soap';
    $api = zimbra_admin_api($location);

    $auth_token = zimbra_authentication_by_server($api, $server_id);

    if (empty($auth_token)) {
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
 * Zimbra authentication by server
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $server_id
 *   Server id.
 * @return authentication token
 */
function zimbra_authentication_by_server(ZimbraAdminApi $api, $server_id) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $server_id);
  if (!isset($cache[$cache_key])) {
    $auth_token = FALSE;
    $server = zmt_server_load($server_id);
    if ($server) {
      if (!empty($server->auth_token)) {
        $auth_token = zimbra_authentication_by_token($api, $server->admin_user, $server->auth_token);
      }
      if (empty($auth_token)) {
        $password = zmt_password_decrypt($server->admin_password);
        $auth_token = zimbra_authentication_by_name($api, $server->admin_user, $password);
      }
      if (!empty($auth_token) && $server->auth_token !== $auth_token) {
        $server->auth_token = $auth_token;
        zmt_server_save($server);
      }
    }
    $cache[$cache_key] = $auth_token;
  }
  return $cache[$cache_key];
}

/**
 * Zimbra authentication by name
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $user
 *   Zimbra admin user.
 * @param string $password
 *   Zimbra admin password.
 * @return authentication token
 */
function zimbra_authentication_by_name(ZimbraAdminApi $api, $user, $password) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $user . $password);
  if (!isset($cache[$cache_key])) {
    $auth_token = FALSE;
    try {
      $auth_token = $api->authByName($user, $password);
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $auth_token;
  }
  return $cache[$cache_key];
}

/**
 * Zimbra authentication by token
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $user
 *   Zimbra admin user.
 * @param string $token
 *   Zimbra authentication token.
 * @return authentication token
 */
function zimbra_authentication_by_token(ZimbraAdminApi $api, $user, $token = '') {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $token);
  if (!isset($cache[$cache_key])) {
    $auth_token = FALSE;
    if (!empty($token)) {
      try {
        $auth_token = $api->authByToken($user, $token);
      }
      catch (Exception $ex) {
        watchdog('zimbra', $ex->__toString());
      }
    }
    $cache[$cache_key] = ($auth_token === $token) ? $auth_token : FALSE;
  }
  return $cache[$cache_key];
}

/**
 * Count number of objects.
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $type
 *   Object type.
 * @param string $domain
 *   Domain name.
 * @return integer
 *   Number of objects.
 */
function zimbra_count_objects(ZimbraAdminApi $api, $type, $domain = NULL) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $type . $domain);
  if (!isset($cache[$cache_key])) {
    try {
      if (!empty($domain)) {
        $result = $api->countObjects($type, new DomainSelector('name', $domain));
      }
      else {
        $result = $api->countObjects($type);
      }
      $cache[$cache_key] = isset($result->num) ? (int) $result->num : (int) $result;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return FALSE;
    }
  }
  return $cache[$cache_key];
}

/**
 * Gets all classes of service
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @return
 *   An array of classes of service
 */
function zimbra_get_all_cos(ZimbraAdminApi $api, $reset = FALSE) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location());
  if ($reset && isset($cache[$cache_key])) {
    unset($cache[$cache_key]);
  }
  if (!isset($cache[$cache_key])) {
    $all_cos = array();
    try {
      $result = $api->getAllCos();
      if (isset($result->cos)) {
        $arr_cos = is_array($result->cos) ? $result->cos : array($result->cos);
        foreach ($arr_cos as $cos) {
          $all_cos[$cos->id] = $cos;
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
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $name
 *   COS name.
 * @return mix
 */
function zimbra_get_cos(ZimbraAdminApi $api, $name, $reset = FALSE) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $name);
  if ($reset && isset($cache[$cache_key])) {
    unset($cache[$cache_key]);
  }
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->getCos(new CosSelector('name', $name));
      $cache[$cache_key] = isset($result->cos[0]) ? $result->cos[0] : $result;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return FALSE;
    }
  }
  return $cache[$cache_key];
}

/**
 * Create Zimbra classes of service
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $name
 *   COS name.
 * @param array  $attrs
 *   COS attributes.
 * @return mix
 */
function zimbra_create_cos(ZimbraAdminApi $api, $name, array $attrs = array()) {
  try {
    $result = $api->createCos($name, _zimbra_attrs_normalize($attrs));
    return isset($result->cos[0]) ? $result->cos[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Modify Zimbra classes of service
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $id
 *   Zimbra COS ID.
 * @param array  $attrs
 *   COS attributes.
 * @return mix
 */
function zimbra_modify_cos(ZimbraAdminApi $api, $id, array $attrs = array()) {
  try {
    $result = $api->modifyCos($id, _zimbra_attrs_normalize($attrs));
    return isset($result->cos[0]) ? $result->cos[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return FALSE;
  }
}

/**
 * Rename Zimbra classes of service
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $zimbra_id
 *   Zimbra COS ID.
 * @param string $new_name
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
    return FALSE;
  }
}

/**
 * Delete Zimbra classes of service
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $zimbra_id
 *   COS Zimbra ID.
 * @return mix
 */
function zimbra_delete_cos(ZimbraAdminApi $api, $zimbra_id) {
  try {
    return $api->deleteCos($zimbra_id);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Gets all Zimbra domains
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @return mix
 */
function zimbra_get_all_domain(ZimbraAdminApi $api, $reset = FALSE) {
  $cache = &drupal_static(__FUNCTION__, array());
  $cache_key = md5($api->location());
  if ($reset && isset($cache[$cache_key])) {
    unset($cache[$cache_key]);
  }
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
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $name
 *   Domain name.
 * @return mix
 */
function zimbra_get_domain(ZimbraAdminApi $api, $name, $reset = FALSE) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $name);
  if ($reset && isset($cache[$cache_key])) {
    unset($cache[$cache_key]);
  }
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->getDomain(new DomainSelector('name', $name));
      $cache[$cache_key] = isset($result->domain[0]) ? $result->domain[0] : $result;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return FALSE;
    }
  }
  return $cache[$cache_key];
}

/**
 * Create Zimbra domain
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $name
 *   Domain name.
 * @param array  $attrs
 *   Domain attributes.
 * @return mix
 */
function zimbra_create_domain(ZimbraAdminApi $api, $name, array $attrs = array()) {
  try {
    $result = $api->createDomain($name, _zimbra_attrs_normalize($attrs));
    return isset($result->domain[0]) ? $result->domain[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Modify Zimbra domain
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $id
 *   Zimbra domain ID.
 * @param array  $attrs
 *   Domain attributes.
 * @return mix
 */
function zimbra_modify_domain(ZimbraAdminApi $api, $id, array $attrs = array()) {
  try {
    $result = $api->modifyDomain($id, _zimbra_attrs_normalize($attrs));
    return isset($result->domain[0]) ? $result->domain[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return FALSE;
  }
}

/**
 * Delete Zimbra domain
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $zimbra_id
 *   Domain Zimbra ID.
 * @return mix
 */
function zimbra_delete_domain(ZimbraAdminApi $api, $zimbra_id) {
  try {
    return $api->deleteDomain($zimbra_id);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Gets all Zimbra distribution lists
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $domain
 *   Domain name.
 * @return array of distribution lists
 */
function zimbra_get_all_dl(ZimbraAdminApi $api, $domain = NULL, $reset = FALSE) {
  $cache = &drupal_static(__FUNCTION__, array());
  $cache_key = md5($api->location() . $domain);
  if ($reset && isset($cache[$cache_key])) {
    unset($cache[$cache_key]);
  }
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
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $domain
 *   The domain name to limit the search to.
 * @param int $limit
 *   The maximum number of accounts to return (0 is default and means all)
 * @param int $offset
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
        'total' => isset($result->searchTotal) ? (int) $result->searchTotal : 0,
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
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $name
 *   Distribution list name.
 * @return mix
 */
function zimbra_get_dl(ZimbraAdminApi $api, $name, $reset = FALSE) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $name);
  if ($reset && isset($cache[$cache_key])) {
    unset($cache[$cache_key]);
  }
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->getDistributionList(new DistributionListSelector('name', $name));
      $cache[$cache_key] = isset($result->dl[0]) ? $result->dl[0] : $result;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return FALSE;
    }
  }
  return $cache[$cache_key];
}

/**
 * Get Zimbra distribution list members
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $name
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
        $members[] = isset($member->_content) ? $member->_content : $member;
      }
    }
    $cache[$cache_key] = $members;
  }
  return $cache[$cache_key];
}

/**
 * Count number of distribution lists in a domain.
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $domain
 *   Domain name.
 * @return mix
 */
function zimbra_count_dl(ZimbraAdminApi $api, $domain = NULL) {
  $cache = &drupal_static(__FUNCTION__);
  $cache_key = md5($api->location() . $domain);
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->searchDirectory(
        '', NULL, 1, 0, $domain, NULL, NULL,
        'distributionlists', NULL, NULL, TRUE, 'zimbraId'
      );
      if (!empty($result->num)) {
        $cache[$cache_key] = (int) $result->num;
      }
      elseif (!empty($result->searchTotal)) {
        $cache[$cache_key] = (int) $result->searchTotal;
      }
      else {
        $cache[$cache_key] = 0;
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return FALSE;
    }
  }
  return $cache[$cache_key];
}

/**
 * Create Zimbra distribution list
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $name
 *   Distribution list name.
 * @param array  $attrs
 *   Domain attributes.
 * @return mix
 */
function zimbra_create_dl(ZimbraAdminApi $api, $name, array $attrs = array()) {
  try {
    $result = $api->createDistributionList($name, NULL, _zimbra_attrs_normalize($attrs));
    return isset($result->dl[0]) ? $result->dl[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Modify Zimbra distribution list
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $zimbra_id
 *   Zimbra ID.
 * @param array  $attrs
 *   Dl attributes.
 * @return mix
 */
function zimbra_modify_dl(ZimbraAdminApi $api, $zimbra_id, array $attrs = array()) {
  try {
    $result = $api->modifyDistributionList($zimbra_id, _zimbra_attrs_normalize($attrs));
    return isset($result->dl[0]) ? $result->dl[0] : $result;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return FALSE;
  }
}

/**
 * Rename Zimbra distribution list
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $zimbra_id
 *   Zimbra ID.
 * @param string $new_name
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
    return FALSE;
  }
}

/**
 * Adding members to a distribution list.
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $zimbra_id
 *   Value of zimbra identify.
 * @param array  $members
 *   Distribution list members.
 * @return mix
 */
function zimbra_add_dl_members(ZimbraAdminApi $api, $zimbra_id, array $members = array()) {
  try {
    return $api->addDistributionListMember($zimbra_id, $members);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return FALSE;
  }
}

/**
 * Remove distribution list members.
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $zimbra_id
 *   Value of zimbra identify.
 * @param array  $members
 *   Distribution list members.
 * @return mix
 */
function zimbra_remove_dl_members(ZimbraAdminApi $api, $id, array $members) {
  try {
    return $api->removeDistributionListMember($id, $members);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Delete Zimbra distribution list
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $zimbra_id
 *   Distribution list Zimbra ID.
 * @return mix
 */
function zimbra_delete_dl(ZimbraAdminApi $api, $zimbra_id) {
  try {
    return $api->deleteDistributionList($zimbra_id);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Gets all Zimbra account
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $domain_name
 *   Domain name.
 * @param string $server_name
 *   Server name
 * @return mix
 */
function zimbra_get_all_account(ZimbraAdminApi $api, $domain_name = '', $server_name = '', $reset = FALSE) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $domain_name . '-' . $server_name);
  if ($reset && isset($cache[$cache_key])) {
    unset($cache[$cache_key]);
  }
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
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $domain
 *   The domain name to limit the search to.
 * @param int $limit
 *   The maximum number of accounts to return (0 is default and means all)
 * @param int $offset
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
        'title',
        'mobile',
        'telephoneNumber',
        'company',
        'description',
        'zimbraMailHost',
        'uid',
        'zimbraCOSId',
        'zimbraAccountStatus',
        'zimbraMailQuota',
        'zimbraLastLogonTimestamp',
        'zimbraCreateTimestamp',
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
        'zimbraMailForwardingAddress',
      );
      $result = $api->searchDirectory(
        '(&(!(zimbraIsSystemAccount=TRUE)))', NULL, $limit, $offset, $domain, FALSE, FALSE,
        'accounts', 'name', TRUE, NULL, implode(',', $attrs)
      );
      $pager = array(
        'num' => isset($result->num) ? (int) $result->num : 0,
        'more' => (bool) $result->more,
        'total' => isset($result->searchTotal) ? (int) $result->searchTotal : 0,
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
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $name
 *   The name used to identify the account.
 * @return mix
 */
function zimbra_get_account(ZimbraAdminApi $api, $name, $reset = FALSE) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location() . $name);
  if ($reset && isset($cache[$cache_key])) {
    unset($cache[$cache_key]);
  }
  if (!isset($cache[$cache_key])) {
    try {
      $attrs = array(
        'displayName',
        'zimbraId',
        'cn',
        'sn',
        'title',
        'mobile',
        'telephoneNumber',
        'company',
        'description',
        'zimbraMailHost',
        'uid',
        'zimbraCOSId',
        'zimbraAccountStatus',
        'zimbraMailQuota',
        'zimbraLastLogonTimestamp',
        'zimbraCreateTimestamp',
        'zimbraIsSystemAccount',
        'zimbraIsDelegatedAdminAccount',
        'zimbraIsAdminAccount',
        'zimbraIsSystemResource',
        'zimbraAuthTokenValidityValue',
        'zimbraIsExternalVirtualAccount',
        'zimbraMailStatus',
        'zimbraMailForwardingAddress',
      );
      $result = $api->getAccount(new AccountSelector('name', $name), NULL, implode(',', $attrs));
      $cache[$cache_key] = isset($result->account[0]) ? $result->account[0] : $result;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return FALSE;
    }
  }
  return $cache[$cache_key];
}

/**
 * Count number of accounts in a domain.
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $domain
 *   Domain name.
 * @return mix
 */
function zimbra_count_account(ZimbraAdminApi $api, $domain = NULL) {
  $cache = &drupal_static(__FUNCTION__);
  $cache_key = md5($api->location() . $domain);
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->searchDirectory(
        '', NULL, NULL, NULL, $domain, NULL, NULL,
        'accounts', NULL, NULL, TRUE, 'zimbraId'
      );
      if (!empty($result->num)) {
        $cache[$cache_key] = (int) $result->num;
      }
      elseif (!empty($result->searchTotal)) {
        $cache[$cache_key] = (int) $result->searchTotal;
      }
      else {
        $cache[$cache_key] = 0;
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return FALSE;
    }
  }
  return $cache[$cache_key];
}

/**
 * Count number of accounts by cos in a domain.
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $domain
 *   Domain name.
 * @return mix
 */
function zimbra_count_account_by_cos(ZimbraAdminApi $api, $domain) {
  $cache = &drupal_static(__FUNCTION__, array());
  $cache_key = md5($api->location() . $type . $domain);
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->countAccount(new DomainSelector('name', $domain));
      $cache[$cache_key] = isset($result->cos) ? $result->cos : array($result);
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return FALSE;
    }
  }
  return $cache[$cache_key];
}

/**
 * Create Zimbra account
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $name
 *   New account's name. Must include domain (uid@name), and domain specified in name must exist.
 * @param string $password
 *   New account's password.
 * @param array  $attrs
 *   Account attributes.
 * @return mix
 */
function zimbra_create_account(ZimbraAdminApi $api, $name, $password, array $attrs = array()) {
  try {
    $result = $api->createAccount($name, $password, _zimbra_attrs_normalize($attrs));
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
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $id
 *   Zimbra ID of account.
 * @param array  $attrs
 *   Attributes.
 * @return mix
 */
function zimbra_modify_account(ZimbraAdminApi $api, $id, array $attrs = array()) {
  try {
    $result = $api->modifyAccount($id, _zimbra_attrs_normalize($attrs));
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
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $id
 *   Zimbra ID of account.
 * @param string $name
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
 * Used to request a new auth token that is valid for the specified account.
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $name
 *   An account name.
 * @param int $duration
 *   Lifetime in seconds of the newly-created authtoken.
 * @return mix
 */
function zimbra_delegate_auth(ZimbraAdminApi $api, $name, $duration = NULL) {
  try {
    $authToken = FALSE;
    $result = $api->delegateAuth(new AccountSelector('name', $name), $duration);
    if (!empty($result->authToken)){
      if (isset($result->authToken[0]->_content)) {
        $authToken =  $result->authToken[0]->_content;
      }
    }
    return $authToken;
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Delete Zimbra account
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $zimbra_id
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
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $domain
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
        'zimbraIsSystemAccount',
        'zimbraIsExternalVirtualAccount',
        'zimbraCreateTimestamp',
      );
      $result = $api->searchDirectory(
        '(&(!(zimbraIsSystemAccount=TRUE)))', NULL, $limit, $offset, $domain, FALSE, FALSE,
        'aliases', 'name', 1, NULL, implode(',', $attrs)
      );
      $pager = array(
        'num' => isset($result->num) ? (int) $result->num : 0,
        'more' => (bool) $result->more,
        'total' => isset($result->searchTotal) ? (int) $result->searchTotal : 0,
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
          $mail_alias->account_name = !empty($alias->targetName) ? $alias->targetName : '';
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
 * Count number of aliases in a domain.
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $domain
 *   Domain name.
 * @return mix
 */
function zimbra_count_alias(ZimbraAdminApi $api, $domain = NULL) {
  $cache = &drupal_static(__FUNCTION__);
  $cache_key = md5($api->location() . $domain);
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->searchDirectory(
        '', NULL, NULL, NULL, $domain, NULL, NULL,
        'aliases', NULL, NULL, TRUE, 'zimbraId'
      );
      if (!empty($result->num)) {
        $cache[$cache_key] = (int) $result->num;
      }
      elseif (!empty($result->searchTotal)) {
        $cache[$cache_key] = (int) $result->searchTotal;
      }
      else {
        $cache[$cache_key] = 0;
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return FALSE;
    }
  }
  return $cache[$cache_key];
}

/**
 * Add Zimbra account alias
 *
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $id
 *   Value of zimbra identify.
 * @param string $alias
 *   Account alias.
 * @return mix
 */
function zimbra_add_account_alias(ZimbraAdminApi $api, $id, $alias) {
  try {
    return $api->addAccountAlias($id, $alias);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Remove Zimbra account alias.
 * 
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $alias
 *   Account alias.
 * @param string $id
 *   Zimbra ID.
 * @return mix
 */
function zimbra_remove_account_alias(ZimbraAdminApi $api, $alias, $id = '') {
  try {
    return $api->removeAccountAlias($alias, $id);
  }
  catch (Exception $ex) {
    watchdog('zimbra', $ex->__toString());
    return $ex;
  }
}

/**
 * Set password.
 * 
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $id
 *   Zimbra ID.
 * @param string $password
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
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @return An array of skin
 */
function zimbra_get_all_skins(ZimbraAdminApi $api, $reset = FALSE) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location());
  if ($reset && isset($cache[$cache_key])) {
    unset($cache[$cache_key]);
  }
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
      return FALSE;
    }
  }
  return $cache[$cache_key];
}

/**
 * Get version information.
 * 
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @return mix
 */
function zimbra_get_version_info(ZimbraAdminApi $api) {
  $cache = &drupal_static(__FUNCTION__, NULL);
  $cache_key = md5($api->location());
  if (!isset($cache[$cache_key])) {
    try {
      $result = $api->getVersionInfo();
      $cache[$cache_key] = isset($result->info[0]) ? $result->info[0] : $result->info;
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      return FALSE;
    }
  }
  return $cache[$cache_key];
}

/**
 * Check password strength.
 * 
 * @param ZimbraAdminApi $api
 *   Zimbra admin api.
 * @param string $id
 *   Zimbra ID.
 * @param string $password
 *   Passowrd to check.
 * @return mix
 */
function zimbra_check_password_strength(ZimbraAdminApi $api, $id, $password) {
  $cache = &drupal_static(__FUNCTION__, FALSE);
  $cache_key = md5($api->location() . $id . $password);
  if (!isset($cache[$cache_key])) {
    try {
      $cache[$cache_key] = $api->checkPasswordStrength($id, $password);
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
      $cache[$cache_key] = $ex;
    }
  }
  return $cache[$cache_key];
}

/**
 * Get Quota Usage
 *
 * @param ZimbraAdminApi $api Zimbra admin api.
 * @param string $domain Domain - the domain name to limit the search to
 * @param int $limit Limit - the number of accounts to return (0 is default and means all)
 * @param int $offset Offset - the starting offset (0, 25, etc)
 * @return mix
 */
function zimbra_get_quota_usage(
  ZimbraAdminApi $api,
  $domain = null,
  $limit = null,
  $offset = null,
  &$pager = array()
) {
  $cache = &drupal_static(__FUNCTION__, array());
  $cache_key = md5($api->location() . '-' . $domain . '-' . $limit . '-' . $offset);
  if (!isset($cache[$cache_key])) {
    $quota_usage = array();
    try {
      $result = $api->getQuotaUsage(
        $domain, null, $limit, $offset, null, null, null
      );
      $pager = array(
        'more' => isset($result->more) ? (bool) $result->more : false,
        'total' => isset($result->searchTotal) ? (int) $result->searchTotal : 0,
        'limit' => (int) $limit,
        'offset' => (int) $offset,
      );
      if(isset($result->account)) {
        $accounts = is_array($result->account) ? $result->account : array($result->account);
        foreach ($accounts as $account) {
          $quota_usage[$account->id] = $account;
        }
      }
    }
    catch (Exception $ex) {
      watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $quota_usage;
  }
  return $cache[$cache_key];
}

/**
 * Normalize zimbra attributes
 */
function _zimbra_attrs_normalize(array $attrs = array()) {
  $a = array();
  foreach ($attrs as $key => $value) {
    if (is_array($value)) {
      if (empty($value)) {
        $a[] = new KeyValuePair($key, '');
      }
      else {
        foreach ($value as $v) {
          $a[] = new KeyValuePair($key, $v);
        }
      }
    }
    else {
      $a[] = new KeyValuePair($key, $value);
    }
  }
  return $a;
}
