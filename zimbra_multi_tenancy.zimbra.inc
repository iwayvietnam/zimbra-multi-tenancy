<?php

/**
 * Test connection to Zimbra service
 *
 * @param  string $location Zimbra service location.
 * @param  string $client Service client.
 * @param  string $user Zimbra admin user.
 * @param  string $password Zimbra admin password.
 * @return bool
 */
function zimbra_test_connection(
    $location = 'https://localhost:7071/service/admin/soap',
    $client = 'soap',
    $user = '',
    $password = ''
)
{
    $authenticated = false;
    $api = zimbra_admin_api($location, $client);
    try
    {
        $result = $api->auth($user, $password);
        $authenticated = isset($result->authToken);
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
    }
    return $authenticated;
}

/**
 * Initialize Zimbra API by tenant
 *
 * @param  string $tenant_id Tenant id.
 * @return Zimbra Admin API
 */
function zimbra_admin_api_by_tenant($tenant_id)
{
    $server = ternant_get_server($tenant_id);
    return zimbra_admin_api_by_server($server->nid);
}

/**
 * Initialize Zimbra API by server
 *
 * @param  string $server_id Server id.
 * @return Zimbra Admin API
 */
function zimbra_admin_api_by_server($server_id)
{
    $server = normalize_server(node_load($server_id));
    $location = isset($server->service_location) ? $server->service_location : 'https://localhost:7071/service/admin/soap';
    $client = isset($server->client_type) ? $server->client_type : 'soap';
    $user = isset($server->admin_user) ? $server->admin_user : '';
    $password = isset($server->admin_password) ? $server->admin_password : '';

    $api = zimbra_admin_api($location, $client);
    $authenticated = zimbra_authentication($api, $user, $password);
    if(!$authenticated)
    {
        $message = 'Connection or authentication to Zimbra server failed! <br />';
        $message .= 'Please contact to administrator to report this error.';
        drupal_set_message(t($message), 'error');
        drupal_access_denied();
        exit;
    }

    return $api;
}

/**
 * Initialize Zimbra API
 *
 * @param  string $location Zimbra service location.
 * @param  string $client Service client.
 * @return Zimbra Admin API
 */
function zimbra_admin_api($location = 'https://localhost:7071/service/admin/soap', $client = 'soap')
{
    static $api = null;
    if(!($api instanceof zimbra_admin_api))
    {
        $debug = (bool) variable_get('zimbra_enable_debug', 0);
        $api = new zimbra_admin_api($location, $client, $debug);
    }
    return $api;
}

/**
 * Zimbra authentication
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $user Zimbra admin user.
 * @param  string $password Zimbra admin password.
 * @return mix
 */
function zimbra_authentication(zimbra_admin_api $api, $user = '', $password = '')
{
    static $authenticated = false;
    $authToken = isset($_SESSION['authToken']) ? $_SESSION['authToken'] : '';
    if(!empty($authToken))
    {
        try
        {
            $result = $api->authByToken($user, $authToken);
            $authenticated = isset($result->authToken);
        }
        catch(Exception $ex)
        {
            watchdog('zimbra', $ex->__toString());
        }
    }
    if(!$authenticated)
    {
        try
        {
            $result = $api->auth($user, $password);
            $authenticated = isset($result->authToken);
            if($authenticated)
            {
                $_SESSION['authToken'] = $result->authToken;
            }
        }
        catch(Exception $ex)
        {
            watchdog('zimbra', $ex->__toString());
        }
    }
    return $authenticated;
}

/**
 * List all Zimbra domains
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @return mix
 */
function zimbra_list_all_domain(zimbra_admin_api $api)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[__FUNCTION__]))
    {
        return $cache[__FUNCTION__];
    }
    $all_domain = array();
    try
    {
        $result = $api->getAllDomains();
        if(isset($result->domain))
        {
            $domains = is_array($result->domain) ? $result->domain : array($result->domain);
            foreach ($domains as $domain)
            {
                $all_domain[$domain->id] = $domain;
            }
        }
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
    }
    $cache[__FUNCTION__] = $all_domain;
    return $all_domain;
}

/**
 * Get information of Zimbra domain
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $domain Domain name.
 * @return mix
 */
function zimbra_get_domain(zimbra_admin_api $api, $domain)
{
    $cache = &drupal_static(__FUNCTION__, null);
    if (isset($cache[$domain]))
    {
        return $cache[$domain];
    }
    try
    {
        $result = $api->getDomain($domain);
        $cache[$domain] = $result->domain;
        return isset($result->domain) ? $result->domain : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return null;
    }
}

/**
 * Create Zimbra domain
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $domain Domain name.
 * @param  array  $attrs  Domain attributes.
 * @return mix
 */
function zimbra_create_domain(zimbra_admin_api $api, $domain, array $attrs = array())
{
    try
    {
        $result = $api->createDomain($domain, $attrs);
        return isset($result->domain) ? $result->domain : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return null;
    }
}

/**
 * Modify Zimbra domain
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $id Zimbra domain ID.
 * @param  array  $attrs  Domain attributes.
 * @return mix
 */
function zimbra_modify_domain(zimbra_admin_api $api, $id, array $attrs = array())
{
    try
    {
        $result = $api->modifyDomain($id, $attrs);
        return isset($result->domain) ? $result->domain : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return null;
    }
}

/**
 * Delete Zimbra domain
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $zimbra_id Domain Zimbra ID.
 * @return mix
 */
function zimbra_delete_domain(zimbra_admin_api $api, $zimbra_id)
{
    try
    {
        return $api->deleteDomain($zimbra_id);
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return false;
    }
}

/**
 * List all Zimbra dl
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $domain Domain name.
 * @return mix
 */
function zimbra_list_al_dl(zimbra_admin_api $api, $domain = '')
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$domain]))
    {
        return $cache[$domain];
    }
    $all_dl = array();
    try
    {
        $result = $api->getAllDistributionLists($domain);
        if(isset($result->dl))
        {
            $dls = is_array($result->dl) ? $result->dl : array($result->dl);
            foreach ($dls as $dl)
            {
                $all_dl[$dl->id] = $dl;
            }
        }
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
    }
    $cache[$domain] = $all_dl;
    return $all_dl;
}

/**
 * Get information of Zimbra distribution list
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $dl Distribution list name.
 * @return mix
 */
function zimbra_get_dl(zimbra_admin_api $api, $dl)
{
    $cache = &drupal_static(__FUNCTION__, null);
    if (isset($cache[$dl]))
    {
        return $cache[$dl];
    }
    try
    {
        $result = $api->getDistributionList($dl);
        $cache[$dl] = $result->dl;
        return isset($result->dl) ? $result->dl : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return null;
    }
}

/**
 * Get Zimbra distribution list members
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $dl    Distribution list name.
 * @return array
 */
function zimbra_get_dl_members(zimbra_admin_api $api, $dl)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$dl]))
    {
        return $cache[$dl];
    }
    $members = array();
    try
    {
        $result = $api->getDistributionList($dl);
        $zimbra_dl = $result->dl;

        $members = isset($zimbra_dl->dlm) ? $zimbra_dl->dlm : array();
        if(!is_array($members))
        {
            $members = array($members);
        }
        $cache[$dl] = $members;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
    }
    return $members;
}

/**
 * Create Zimbra distribution list
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $dl    Distribution list name.
 * @param  array  $attrs Domain attributes.
 * @return mix
 */
function zimbra_create_dl(zimbra_admin_api $api, $dl, array $attrs = array())
{
    try
    {
        $result = $api->createDistributionList($dl, $attrs);
        return isset($result->dl) ? $result->dl : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return null;
    }
}

/**
 * Modify Zimbra distribution list
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $zimbra_id Zimbra ID.
 * @param  array  $attrs Dl attributes.
 * @return mix
 */
function zimbra_modify_dl(zimbra_admin_api $api, $zimbra_id, array $attrs = array())
{
    try
    {
        $result = $api->modifyDistributionList($zimbra_id, $attrs);
        return isset($result->dl) ? $result->dl : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return null;
    }
}

/**
 * Rename Zimbra distribution list
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $zimbra_id Zimbra ID.
 * @param  string $newName New dl name.
 * @return mix
 */
function zimbra_rename_dl(zimbra_admin_api $api, $zimbra_id, $newName)
{
    try
    {
        $result = $api->renameDistributionList($zimbra_id, $newName);
        return isset($result->dl) ? $result->dl : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return null;
    }
}

/**
 * Adding members to a distribution list.
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $zimbra_id Value of zimbra identify.
 * @param  array  $members   Distribution list members.
 * @return mix
 */
function zimbra_add_dl_members(zimbra_admin_api $api, $zimbra_id, array $members = array())
{
    try
    {
        return $api->addDistributionListMember($zimbra_id, $members);
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return null;
    }
}

/**
 * Remove distribution list members.
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $zimbra_id Value of zimbra identify.
 * @param  array  $members   Distribution list members.
 * @return mix
 */
function zimbra_remove_dl_members(zimbra_admin_api $api, $id, array $members)
{
    try
    {
        return $api->removeDistributionListMember($id, $members);
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return null;
    }
}

/**
 * Delete Zimbra distribution list
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $zimbra_id Distribution list Zimbra ID.
 * @return mix
 */
function zimbra_delete_dl(zimbra_admin_api $api, $zimbra_id)
{
    try
    {
        return $api->deleteDistributionList($zimbra_id);
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return false;
    }
}

/**
 * List all Zimbra account
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $domain Domain name.
 * @param  string $server Server name
 * @return mix
 */
function zimbra_list_all_account(zimbra_admin_api $api, $domain = '', $server = '')
{
    $cache = &drupal_static(__FUNCTION__, null);
    $cache_key = md5($domain.'-'.$server);
    if (isset($cache[$cache_key]))
    {
        return $cache[$cache_key];
    }
    $all_account = array();
    try
    {
        $result = $api->getAllAccounts($server, $domain);
        if(isset($result->account))
        {
            $accounts = is_array($result->account) ? $result->account : array($result->account);
            foreach ($accounts as $account)
            {
                $all_account[$account->id] = $account;
            }
        }
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $all_account;
    return $all_account;
}

/**
 * Get information of Zimbra account
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $account The name used to identify the account.
 * @return mix
 */
function zimbra_get_account(zimbra_admin_api $api, $account)
{
    $cache = &drupal_static(__FUNCTION__, null);
    if (isset($cache[$account]))
    {
        return $cache[$account];
    }
    try
    {
        $result = $api->getAccount($account);
        if(isset($result->account))
        {
            $cache[$account] = $result->account;
        }
        return isset($result->account) ? $result->account : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return null;
    }
}

/**
 * Create Zimbra account
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $name     New account's name. Must include domain (uid@name), and domain specified in name must exist.
 * @param  string $password New account's password.
 * @param  array  $attrs    Account attributes.
 * @return mix
 */
function zimbra_create_account(zimbra_admin_api $api, $account, $password, array $attrs = array())
{
    try
    {
        $result = $api->createAccount($account, $password, $attrs);
        return isset($result->account) ? $result->account : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return $ex;
    }
}

/**
 * Modify Zimbra account
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $id    Zimbra ID of account.
 * @param  array  $attrs Attributes.
 * @return mix
 */
function zimbra_modify_account(zimbra_admin_api $api, $id, array $attrs = array())
{
    try
    {
        $result = $api->modifyAccount($id, $attrs);
        return isset($result->account) ? $result->account : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return $ex;
    }
}

/**
 * Rename Zimbra account
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $id   Zimbra ID of account.
 * @param  array  $name New account name.
 * @return mix
 */
function zimbra_rename_account(zimbra_admin_api $api, $id, $name)
{
    try
    {
        $result = $api->renameAccount($id, $name);
        return isset($result->account) ? $result->account : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return $ex;
    }
}

/**
 * Delete Zimbra account
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $zimbra_id Account Zimbra ID.
 * @return mix
 */
function zimbra_delete_account(zimbra_admin_api $api, $zimbra_id)
{
    try
    {
        return $api->deleteAccount($zimbra_id);
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return $ex;
    }
}

/**
 * List all Zimbra account alias
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $domain Domain name.
 * @return array
 */
function zimbra_list_all_alias(zimbra_admin_api $api, $domain = '')
{
    $cache = &drupal_static(__FUNCTION__, array());
    $cache_key = md5(__FUNCTION__.'-'.$domain);
    if (isset($cache[$cache_key]))
    {
        return $cache[$cache_key];
    }
    $all_alias = array();
    try
    {
		$attrs = array('zimbraAliasTargetId', 'zimbraId', 'targetName', 'uid', 'type', 'description', 'zimbraIsDelegatedAdminAccount', 'zimbraIsAdminAccount', 'zimbraIsSystemResource');
        $result = $api->searchDirectory('', $domain, false, false, false, $attrs, array('aliases'));
        if(isset($result->alias))
        {
            $aliases = is_array($result->alias) ? $result->alias : array($result->alias);
            foreach($aliases as $alias)
            {
                $mail_alias = new stdClass;
                $mail_alias->id = $alias->id;
                $mail_alias->name = $alias->name;
                $mail_alias->account_id = '';
                $mail_alias->account_name = $alias->targetName;
                $mail_alias->domain = $domain;
                $attrs = isset($alias->a) ? $alias->a : array();
                foreach ($attrs as $attr)
                {
                    if($attr->n === 'zimbraAliasTargetId')
                    {
                        $mail_alias->account_id = isset($attr->_) ? $attr->_ : '';
                    }
                }
                $all_alias[$alias->id] = $mail_alias;
            }
        }
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
    }
    $cache[$cache_key] = $all_alias;
    return $all_alias;
}

/**
 * Add Zimbra account alias
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $id    Value of zimbra identify.
 * @param  string $alias Account alias.
 * @return mix
 */
function zimbra_add_account_alias(zimbra_admin_api $api, $id, $alias)
{
    try
    {
        return $api->addAccountAlias($id, $alias);
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return false;
    }
}

/**
 * Remove Zimbra account alias.
 * 
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $alias Account alias.
 * @param  string $id    Zimbra ID.
 * @return mix
 */
function zimbra_remove_account_alias(zimbra_admin_api $api, $alias, $id = '')
{
    try
    {
        return $api->removeAccountAlias($alias, $id);
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return false;
    }
}

/**
 * Set password.
 * 
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $id       Zimbra ID.
 * @param  string $password New password.
 * @return mix
 */
function zimbra_set_password(zimbra_admin_api $api, $id, $password)
{
    try
    {
        return $api->setPassword($id, $password);
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return $ex;
    }
}

/**
 * Get all classes of service
 *
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @return mix
 */
function zimbra_list_all_cos(zimbra_admin_api $api)
{
    $cache = &drupal_static(__FUNCTION__, null);
    if (isset($cache[__FUNCTION__]))
    {
        return $cache[__FUNCTION__];
    }
    $all_cos = array();
    try
    {
        $result = $api->getAllCos();
        if(isset($result->cos))
        {
            $arr_cos = is_array($result->cos) ? $result->cos : array($result->cos);
            foreach ($arr_cos as $cos)
            {
                $all_cos[$cos->id] = $cos->name;
            }
        }
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
    }
    $cache[__FUNCTION__] = $all_cos;
    return $all_cos;
}

/**
 * Check password strength.
 * 
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @param  string $id       Zimbra ID.
 * @param  string $password New password.
 * @return mix
 */
function zimbra_check_password(zimbra_admin_api $api, $id, $password)
{
    try
    {
        return $api->checkPasswordStrength($id, $password);
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return $ex->getMessage();
    }
}

/**
 * Get version information .
 * 
 * @param  zimbra_admin_api $api Zimbra admin api.
 * @return mix
 */
function zimbra_get_version_info(zimbra_admin_api $api)
{
    $cache = &drupal_static(__FUNCTION__, null);
    if (isset($cache[__FUNCTION__]))
    {
        return $cache[__FUNCTION__];
    }
    try
    {
        $result = $api->getVersionInfo();
        if(isset($result->info))
        {
            $cache[__FUNCTION__] = $result->info;
        }
        return isset($result->info) ? $result->info : $result;
    }
    catch(Exception $ex)
    {
        watchdog('zimbra', $ex->__toString());
        return null;
    }
}
