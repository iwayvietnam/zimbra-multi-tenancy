<?php

/**
 * Get information of mailbox
 *
 * @param  string $mid Mailbox ID.
 * @return mix
 */
function mailbox_get_info($mid)
{
    $mailbox = array();
    $mail_node = node_load($mid);
    if(!isset($mail_node)) return $mailbox;

    $domain_name = '';
    $domain_id = '';
    if(isset( $mail_node->field_node_domain[LANGUAGE_NONE][0]['nid']))
    {   
        $domain_id = $mail_node->field_node_domain[LANGUAGE_NONE][0]['nid'];
        $domain_node = node_load($mail_node->field_node_domain[LANGUAGE_NONE][0]['nid']);
        if(isset($domain_node->title)) $domain_name = $domain_node->title;
    }

    $mailbox['mid'] = $mail_node->nid;
    $mailbox['mail'] = $mail_node->title . '@' . $domain_name;
    $mailbox['name'] = $mail_node->title;
    $mailbox['domain'] = $domain_name;
    $mailbox['domain_id'] = $domain_id;
    $mailbox['fullname'] = $mail_node->field_mailbox_fullname[LANGUAGE_NONE][0]['value'];
    $mailbox['mailsize'] = intval($mail_node->field_mailbox_size[LANGUAGE_NONE][0]['value']);

    return $mailbox;
}

/**
 * Get tenant of domain
 *
 * @param  int $domain_id Domain ID
 * @return mix
 */
function tenant_get_id_from_domain($domain_id)
{
    $tenant_id = 0;
    $domain_node = node_load($domain_id);
    if(isset($domain_node->field_domain_tenant[LANGUAGE_NONE][0]['nid']))
    {
        $tenant_id = $domain_node->field_domain_tenant[LANGUAGE_NONE][0]['nid'];
    }
    return $tenant_id;
}

/**
 * Get server of domain
 *
 * @param  int $domain_id Domain ID
 * @return mix
 */
function server_get_id_from_domain($domain_id)
{
    $server_id = 0;
    $domain_node = node_load($domain_id);
    if(isset($domain_node->field_domain_server[LANGUAGE_NONE][0]['nid']))
    {
        $server_id = $domain_node->field_domain_server[LANGUAGE_NONE][0]['nid'];
    }
    return $server_id;
}

/**
 * Get domain of mailbox
 *
 * @param  int $mailbox_id Mailbox ID
 * @return mix
 */
function domain_get_id_from_mailbox($mailbox_id)
{
    $domain_id = 0;
    $mailbox_node = node_load($mailbox_id);
    if(isset($mailbox_node->field_node_domain[LANGUAGE_NONE][0]['nid']))
    {
        $domain_id = $mailbox_node ->field_node_domain[LANGUAGE_NONE][0]['nid'];
    }
    return $domain_id;
}

/**
 * Check the quota of Tenant
 *
 * @param  int $tenant_id Tenant ID
 * @param  int $nid       Node ID.
 * @return mix
 */
function tenant_check_quota($tenant_id, $nid = 0)
{
    $tenant_id = intval($tenant_id);
    $nid = intval($nid);
    $tenant_node = node_load($tenant_id);

    if(!isset($tenant_node->field_tenant_mailbox_max_size[LANGUAGE_NONE][0]['value'])) return 0;
    $tenant_quota_total = intval($tenant_node->field_tenant_mailbox_max_size[LANGUAGE_NONE][0]['value']);
    $tenant_quota_used = 0;

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'domain')
        ->propertyCondition('nid', $nid, '!=')
        ->fieldCondition('field_domain_tenant', 'nid', $tenant_id, '=');
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    foreach ($nodes as $node)
    {
        $mailbox_quota = isset($node->field_domain_mailbox_max_size[LANGUAGE_NONE][0]['value']) ? (int) $node->field_domain_mailbox_max_size[LANGUAGE_NONE][0]['value'] : 0;
        $tenant_quota_used += $mailbox_quota;
    }

    return $tenant_quota_total - $tenant_quota_used;
}

/**
 * Check the quota of Domain
 *
 * @param  int $domain_id Domain ID
 * @param  int $nid       Node ID.
 * @return mix
 */
function domain_check_quota($domain_id, $nid = 0)
{
    $domain_id = intval($domain_id);
    $nid = intval($nid);
    $domain_node = node_load($domain_id);

    if(!isset($domain_node->field_domain_mailbox_max_size[LANGUAGE_NONE][0]['value'])) return 0;
    $domain_quota_total = intval($domain_node->field_domain_mailbox_max_size[LANGUAGE_NONE][0]['value']);
    $domain_quota_used = 0;

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'mailbox')
        ->propertyCondition('nid', $nid, '!=')
        ->fieldCondition('field_node_domain', 'nid', $domain_id, '=');
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    foreach ($nodes as $node)
    {
        $mailbox_size = isset($node->field_mailbox_size[LANGUAGE_NONE][0]['value']) ? (int) $node->field_mailbox_size[LANGUAGE_NONE][0]['value'] : 0;
        $domain_quota_used += $mailbox_size;
    }
    return $domain_quota_total - $domain_quota_used;
}

/**
 * Check the number Mailbox of Tenant
 *
 * @param  int $tenant_id Tenant ID
 * @param  int $nid       Node ID.
 * @return mix
 */
function tenant_check_number($tenant_id, $nid = 0)
{
    $tenant_node = node_load($tenant_id);

    if(!isset($tenant_node->field_tenant_number_of_mailboxes[LANGUAGE_NONE][0]['value'])) return 0;
    $tenant_number_total = intval($tenant_node->field_tenant_number_of_mailboxes[LANGUAGE_NONE][0]['value']);
    $tenant_number_used = 0;

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'domain')
        ->propertyCondition('nid', $nid, '!=')
        ->fieldCondition('field_domain_tenant', 'nid', $tenant_id, '=');
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    foreach ($nodes as $node)
    {
        $number_of_mailboxes = isset($node->field_domain_number_of_mailboxes[LANGUAGE_NONE][0]['value']) ? (int) $node->field_domain_number_of_mailboxes[LANGUAGE_NONE][0]['value'] : 0;
        $tenant_number_used += $number_of_mailboxes;
    }
    
    return $tenant_number_total - $tenant_number_used;
}

/**
 * Check the number Mailbox of Domain
 *
 * @param  int $domain_id Domain ID
 * @param  int $nid       Node ID.
 * @return mix
 */
function domain_check_number($domain_id, $nid = 0)
{
    $node = node_load($domain_id);

    if(!isset($node->field_domain_number_of_mailboxes[LANGUAGE_NONE][0]['value']))
    {
        return 0;
    }
    $domain_number_total = intval($node->field_domain_number_of_mailboxes[LANGUAGE_NONE][0]['value']);
    $domain_number_used = 0;

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'mailbox')
        ->propertyCondition('nid', $nid, '!=')
        ->fieldCondition('field_node_domain', 'nid', $domain_id);
    $domain_number_used = (int) $efq->count()->execute();
    return $domain_number_total - $domain_number_used;
}

function user_get_account_by_domain($nid)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$nid]))
    {
        return $cache[$nid];
    }

    $domain = node_load($nid);

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', array('mailbox', 'alias', 'group'), 'IN')
        ->fieldCondition('field_node_domain', 'nid', $nid, '=');
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $accounts = array();
    foreach ($nodes as $key => $value)
    {
        if($domain)
        {
            $accounts[$key] = $value->title . '@' . $domain->title;
        }
        else
        {
            $accounts[$key] = $value->title;
        }
    }
    $cache[$nid] = $accounts;
    return $accounts;
}

/**
 * Get all account of user
 * 
 * @param  int $uid User ID
 * @return mix
 */
function user_get_all_account($uid)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$uid]))
    {
        return $cache[$uid];
    }

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', array('mailbox', 'alias', 'group'), 'IN');
    $domains = user_get_domain($uid);
    $nids = array_keys($domains);
    if(count($nids))
    {
        $efq->fieldCondition('field_node_domain', 'nid', $nids, 'IN');
    }
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $accounts = array();
    foreach ($nodes as $key => $value)
    {
        $domain_id = isset($value->field_node_domain[LANGUAGE_NONE][0]['nid']) ? $value->field_node_domain[LANGUAGE_NONE][0]['nid'] : 0;
        $domain = isset($domains[$domain_id]) ? $domains[$domain_id] : '';
        $accounts[$key] = $value->title . '@' . $domain;
    }
    $cache[$uid] = $accounts;
    return $nodes;
}

/**
 * Get total tenant of user
 *
 * @param  int $uid User ID.
 * @return mix
 */
function user_access_list_tenant($uid)
{
    $cache = &drupal_static(__FUNCTION__, null);
    if (isset($cache[$uid]))
    {
        return $cache[$uid];
    }
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'tenant')
        ->fieldCondition('field_tenant_user', 'uid', $uid, '=');
    $total = (int) $efq->count()->execute();
    $cache[$uid] = $total;
    return $total;
}

/**
 * Get total domain of user
 *
 * @param  int $uid User ID.
 * @return mix
 */
function user_access_list_domain($uid)
{
    $cache = &drupal_static(__FUNCTION__, null);
    if (isset($cache[$uid]))
    {
        return $cache[$uid];
    }
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'domain')
        ->fieldCondition('field_domain_user', 'uid', $uid, '=');
    $tenants = user_get_tenant($uid);
    $nids = array_keys($tenants);
    if(count($nids))
    {
        $efq->fieldCondition('field_domain_tenant', 'nid', $nids, 'IN');
    }
    $total = (int) $efq->count()->execute();
    $cache[$uid] = $total;
    return $total;
}

/**
 * Get Server form Tenant
 * 
 * @param  int $nid Tenant ID
 * @return mix
 */
function ternant_get_server($nid)
{
    $node = node_load($nid);
    $server_nid = isset($node->field_tenant_server[LANGUAGE_NONE][0]['nid']) ? $node->field_tenant_server[LANGUAGE_NONE][0]['nid'] : 0;
    return normalize_server(node_load($server_nid));
}

/**
 * Get Server form Domain
 * 
 * @param  int $nid Domain ID
 * @return mix
 */
function domain_get_server($nid)
{
    $node = node_load($nid);
    $server_nid = isset($node->field_domain_server[LANGUAGE_NONE][0]['nid']) ? $node->field_domain_server[LANGUAGE_NONE][0]['nid'] : 0;
    return normalize_server(node_load($server_nid));
}

/**
 * Get all Tenant of server
 * 
 * @param  int $nid Server node ID
 * @return array
 */
function server_get_tenant($nid)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$nid]))
    {
        return $cache[$nid];
    }
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'tenant');
    if(!user_access('administer zimbra'))
    {
        $efq->fieldCondition('field_tenant_server', 'nid', $nid, '=');
    }
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $tenants = array();
    foreach ($nodes as $nid => $node)
    {
        $tenants[$nid] = normalize_tenant($node);
    }
    $cache[$nid] = $tenants;
    return $tenants;
}

/**
 * Get all Tenant of user
 * 
 * @param  int $uid User ID
 * @param  int $server_id Server node ID
 * @return array
 */
function user_get_tenant($uid, $server_id = 0)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$uid . $server_id]))
    {
        return $cache[$uid . $server_id];
    }
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'tenant');
    if(!user_access('administer zimbra'))
    {
        $efq->fieldCondition('field_tenant_user', 'uid', $uid, '=');
    }
    if((int) $server_id > 0)
    {
        $efq->fieldCondition('field_tenant_server', 'nid', $server_id, '=');
    }
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $tenants = array();
    foreach ($nodes as $nid => $node)
    {
        $tenants[$nid] = normalize_tenant($node);
    }
    $cache[$uid . $server_id] = $tenants;
    return $tenants;
}

/**
 * Get all domain of user
 * 
 * @param  int $uid User ID
 * @return mix
 */
function user_get_domain($uid)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$uid]))
    {
        return $cache[$uid];
    }

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'domain');

    if(!user_access('administer zimbra'))
    {
        $tenant_efq = clone $efq;

        $efq->fieldCondition('field_domain_user', 'uid', $uid, '=');
        $result = $efq->execute();
        $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
        if(count($nids) === 0)
        {
            $tenants = user_get_tenant($uid);
            $tenant_nids = array_keys($tenants);
            if(count($tenant_nids))
            {
                $tenant_efq->fieldCondition('field_domain_tenant', 'nid', $tenant_nids, 'IN');
                $result = $tenant_efq->execute();
                $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
            }
            if(count($nids) === 0)
            {
                $efq = new EntityFieldQuery();
                $efq->entityCondition('entity_type', 'node')
                    ->entityCondition('bundle', 'group')
                    ->fieldCondition('field_group_user', 'uid', $uid);
                $result = $efq->execute();
                $group_nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
                $group_nodes = node_load_multiple($group_nids);
                foreach ($group_nodes as $nid => $node)
                {
                    if(isset($node->field_node_domain[LANGUAGE_NONE]))
                    {
                        $nids[] = $node->field_node_domain[LANGUAGE_NONE][0]['nid'];
                    }
                }
            }
        }
    }
    else
    {
        $result = $efq->execute();
        $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    }

    $nodes = node_load_multiple($nids);
    $domains = array();
    foreach ($nodes as $nid => $node)
    {
        $domains[$nid] = normalize_domain($node);
    }
    $cache[$uid] = $domains;
    return $domains;
}

/**
 * Get all group of user
 * 
 * @param  int $uid User ID
 * @return mix
 */
function user_get_group($uid)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$uid]))
    {
        return $cache[$uid];
    }

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'group');
    if(!user_access('administer zimbra'))
    {
        $domain_efq = clone $efq;

        $efq->fieldCondition('field_group_user', 'uid', $uid);
        $result = $efq->execute();
        $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();

        if(count($nids) === 0)
        {
            $domains = user_get_domain($uid);
            $domain_nids = array_keys($domains);
            if(count($domain_nids))
            {
                $domain_efq->fieldCondition('field_node_domain', 'nid', $domain_nids, 'IN');
                $result = $domain_efq->execute();
                $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
            }
        }
    }
    else
    {
        $result = $efq->execute();
        $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    }

    $nodes = node_load_multiple($nids);
    $groups = array();
    foreach ($nodes as $nid => $node)
    {
        $groups[$nid] = normalize_group($node);
    }
    $cache[$uid] = $groups;
    return $groups;
}

function domain_get_group($nid)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$nid]))
    {
        return $cache[$nid];
    }
    $domain = node_load($nid);

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'group')
        ->fieldCondition('field_node_domain', 'nid', $nid);
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $groups = array();
    foreach ($nodes as $nid => $node)
    {
        $groups[$nid] = normalize_group($node);
    }
    $cache[$nid] = $groups;
    return $groups;
}

function domain_get_mailbox($nid)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$nid]))
    {
        return $cache[$nid];
    }
    $domain = node_load($nid);

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'mailbox')
        ->fieldCondition('field_node_domain', 'nid', $nid);
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $groups = array();
    foreach ($nodes as $nid => $node)
    {
        $groups[$nid] = normalize_mailbox($node);
    }
    $cache[$nid] = $groups;
    return $groups;
}


function domain_get_alias($nid)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$nid]))
    {
        return $cache[$nid];
    }
    $domain = node_load($nid);

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'alias')
        ->fieldCondition('field_node_domain', 'nid', $nid);
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $groups = array();
    foreach ($nodes as $nid => $node)
    {
        $groups[$nid] = normalize_alias($node);
    }
    $cache[$nid] = $groups;
    return $groups;
}

/**
 * Get all mailbox of user
 * 
 * @param  int $uid User ID
 * @return mix
 */
function user_get_mailbox($uid)
{
    $cache = &drupal_static(__FUNCTION__, array());
    if (isset($cache[$uid]))
    {
        return $cache[$uid];
    }

    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'mailbox');
    $domains = user_get_domain($uid);
    $nids = array_keys($domains);
    if(count($nids))
    {
        $efq->fieldCondition('field_node_domain', 'nid', $nids, 'IN');
    }
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $mailboxes = array();
    foreach ($nodes as $nid => $node)
    {
        $mailboxes[$nid] = normalize_mailbox($node);
    }
    $cache[$uid] = $mailboxes;
    return $mailboxes;
}

/**
 * Get account by name
 * 
 * @param  string $name Account name
 * @return mix
 */
function get_account_by_name($name, $domain_id = 0)
{
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', array('mailbox', 'alias', 'group'), 'IN')
        ->propertyCondition('title', $name, '=')
        ->fieldCondition('field_node_domain', 'nid', $domain_id, '=');
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nid = count($nids) ? (int) $nids[0] : 0;
    return node_load($nid);
}

/**
 * Get domain by name
 * 
 * @param  string $domain Domain name
 * @return mix
 */
function get_domain_by_name($domain)
{
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'domain')
        ->propertyCondition('title', $domain, '=');
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nid = count($nids) ? (int) $nids[0] : 0;
    return normalize_domain(node_load($nid));
}

/**
 * Check domain exists
 * 
 * @param  string $domain Domain name
 * @param  int    $nid    Node ID
 * @return mix
 */
function domain_exists($domain, $nid = 0)
{
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'domain')
        ->propertyCondition('nid', $nid, '!=')
        ->propertyCondition('title', $domain, '=');
    $result = $efq->execute();
    return (isset($result['node']) AND is_array($result['node'])) ? count($result['node']) > 0 : false;
}

/**
 * Check mailbox exists
 * 
 * @param  string $name   Mailbox name
 * @param  string $domain Domain name
 * @param  int    $nid    Node ID
 * @return mix
 */
function mailbox_exists($name, $domain, $nid = 0)
{
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'mailbox')
        ->propertyCondition('nid', $nid, '!=')
        ->propertyCondition('title', $name, '=')
        ->fieldCondition('field_node_domain', 'nid', $domain, '=');
    $result = $efq->execute();
    $exists = (isset($result['node']) AND is_array($result['node'])) ? count($result['node']) > 0 : false;
    if(!$exists)
    {
        $efq = new EntityFieldQuery();
        $efq->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', 'group')
            ->propertyCondition('nid', $nid, '!=')
            ->propertyCondition('title', $name, '=')
            ->fieldCondition('field_node_domain', 'nid', $domain, '=');
        $result = $efq->execute();
        $exists = (isset($result['node']) AND is_array($result['node'])) ? count($result['node']) > 0 : false;
    }
    if(!$exists)
    {
        $efq = new EntityFieldQuery();
        $efq->entityCondition('entity_type', 'node')
            ->entityCondition('bundle', 'alias')
            ->propertyCondition('nid', $nid, '!=')
            ->propertyCondition('title', $name, '=')
            ->fieldCondition('field_node_domain', 'nid', $domain, '=');
        $result = $efq->execute();
        $exists = (isset($result['node']) AND is_array($result['node'])) ? count($result['node']) > 0 : false;
    }
    return $exists;
}

/**
 * Delete group by domain
 * 
 * @param  int $nid Domain node ID
 */
function delete_group_by_domain($nid)
{
	$groups = domain_get_group();
	foreach($groups as $group)
	{
		node_delete($group->nid);
	}
}

/**
 * Delete mailbox by domain
 * 
 * @param  int $nid Domain node ID
 */
function delete_mailbox_by_domain($nid)
{
	$mailboxes = domain_get_mailbox();
	foreach($mailboxes as $mailbox)
	{
		node_delete($mailbox->nid);
	}
}

/**
 * Delete alias by domain
 * 
 * @param  int $nid Domain node ID
 */
function delete_alias_by_domain($nid)
{
	$aliases = domain_get_alias();
	foreach($aliases as $alias)
	{
		node_delete($alias->nid);
	}
}

/**
 * Normalize server node type
 * 
 * @param  mix $node Server node
 * @return mix
 */
function normalize_server($node)
{
    $cache = &drupal_static(__FUNCTION__, array());
    $nid = isset($node->nid) ? (int) $node->nid : 0;
    if (isset($cache[$nid]))
    {
        return $cache[$nid];
    }

    $server = new stdClass;
    $server->nid = isset($node->nid) ? (int) $node->nid : 0;
    $server->title = isset($node->title) ? $node->title : '';

    $server->service_location = isset($node->field_server_service_location[LANGUAGE_NONE][0]['value']) ? $node->field_server_service_location[LANGUAGE_NONE][0]['value'] : '';
    $server->client_type = isset($node->field_server_client_type[LANGUAGE_NONE][0]['value']) ? $node->field_server_client_type[LANGUAGE_NONE][0]['value'] : '';
    $server->admin_user = isset($node->field_server_admin_user[LANGUAGE_NONE][0]['value']) ? $node->field_server_admin_user[LANGUAGE_NONE][0]['value'] : '';
    $password = isset($node->field_server_admin_password[LANGUAGE_NONE][0]['password_field']) ? $node->field_server_admin_password[LANGUAGE_NONE][0]['password_field'] : '';
    $server->admin_password = password_field_decrypt($password);
    $server->exclude_mailbox = isset($node->field_server_exclude_mailbox[LANGUAGE_NONE][0]['value']) ? $node->field_server_exclude_mailbox[LANGUAGE_NONE][0]['value'] : '';
    $server->delete_domain = isset($node->field_server_delete_domain[LANGUAGE_NONE][0]['value']) ? (int) $node->field_server_delete_domain[LANGUAGE_NONE][0]['value'] : 0;
    $server->delete_group = isset($node->field_server_delete_group[LANGUAGE_NONE][0]['value']) ? (int) $node->field_server_delete_group[LANGUAGE_NONE][0]['value'] : 0;
    $server->delete_mailbox = isset($node->field_server_delete_mailbox[LANGUAGE_NONE][0]['value']) ? (int) $node->field_server_delete_mailbox[LANGUAGE_NONE][0]['value'] : 0;
    $server->delete_alias = isset($node->field_server_delete_alias[LANGUAGE_NONE][0]['value']) ? (int) $node->field_server_delete_alias[LANGUAGE_NONE][0]['value'] : 0;

    $cache[$nid] = $server;

    return $server;
}

/**
 * Normalize tenant node type
 * 
 * @param  mix $node Tenant node
 * @return mix
 */
function normalize_tenant($node)
{
    $cache = &drupal_static(__FUNCTION__, array());
    $nid = isset($node->nid) ? (int) $node->nid : 0;
    if (isset($cache[$nid]))
    {
        return $cache[$nid];
    }

    $tenant = new stdClass;
    $tenant->nid = isset($node->nid) ? (int) $node->nid : 0;
    $tenant->title = isset($node->title) ? $node->title : '';

    $tenant->user_name = isset($node->field_tenant_user_name[LANGUAGE_NONE][0]['value']) ? $node->field_tenant_user_name[LANGUAGE_NONE][0]['value'] : '';
    $tenant->user_mail = isset($node->field_tenant_user_mail[LANGUAGE_NONE][0]['value']) ? $node->field_tenant_user_mail[LANGUAGE_NONE][0]['value'] : '';
    $tenant->user_password = isset($node->field_tenant_user_password[LANGUAGE_NONE][0]['value']) ? $node->field_tenant_user_password[LANGUAGE_NONE][0]['value'] : '';
    $tenant->tenant_base = isset($node->field_tenant_base[LANGUAGE_NONE][0]['value']) ? $node->field_tenant_base[LANGUAGE_NONE][0]['value'] : '';
    $tenant->mailbox_max_size = isset($node->field_tenant_mailbox_max_size[LANGUAGE_NONE][0]['value']) ? $node->field_tenant_mailbox_max_size[LANGUAGE_NONE][0]['value'] : '';
    $tenant->number_of_mailboxes = isset($node->field_tenant_number_of_mailboxes[LANGUAGE_NONE][0]['value']) ? $node->field_tenant_number_of_mailboxes[LANGUAGE_NONE][0]['value'] : '';

    $server_nid = isset($node->field_tenant_server[LANGUAGE_NONE][0]['nid']) ? $node->field_tenant_server[LANGUAGE_NONE][0]['nid'] : 0;
    $tenant->server = normalize_server(node_load($server_nid));

    $user_uid = isset($node->field_tenant_user[LANGUAGE_NONE][0]['uid']) ? $node->field_tenant_user[LANGUAGE_NONE][0]['uid'] : 0;
    $tenant->user = user_load($user_uid);

    $cache[$nid] = $tenant;

    return $tenant;
}

/**
 * Normalize domain node type
 * 
 * @param  mix $node Domain node
 * @return mix
 */
function normalize_domain($node)
{
    $cache = &drupal_static(__FUNCTION__, array());
    $nid = isset($node->nid) ? (int) $node->nid : 0;
    if (isset($cache[$nid]))
    {
        return $cache[$nid];
    }

    $domain = new stdClass;
    $domain->nid = isset($node->nid) ? (int) $node->nid : 0;
    $domain->title = isset($node->title) ? $node->title : '';

    $domain->user_mail = isset($node->field_domain_user_mail[LANGUAGE_NONE][0]['value']) ? $node->field_domain_user_mail[LANGUAGE_NONE][0]['value'] : '';
    $domain->user_password = isset($node->field_domain_user_password[LANGUAGE_NONE][0]['value']) ? $node->field_domain_user_password[LANGUAGE_NONE][0]['value'] : '';
    $domain->mailbox_max_size = isset($node->field_domain_mailbox_max_size[LANGUAGE_NONE][0]['value']) ? $node->field_domain_mailbox_max_size[LANGUAGE_NONE][0]['value'] : '';
    $domain->number_of_mailboxes = isset($node->field_domain_number_of_mailboxes[LANGUAGE_NONE][0]['value']) ? $node->field_domain_number_of_mailboxes[LANGUAGE_NONE][0]['value'] : '';
    $domain->zimbra_id = isset($node->field_domain_zimbra_id[LANGUAGE_NONE][0]['value']) ? $node->field_domain_zimbra_id[LANGUAGE_NONE][0]['value'] : '';

    $server_nid = isset($node->field_domain_server[LANGUAGE_NONE][0]['nid']) ? $node->field_domain_server[LANGUAGE_NONE][0]['nid'] : 0;
    $domain->server = normalize_server(node_load($server_nid));

    $tenant_nid = isset($node->field_domain_tenant[LANGUAGE_NONE][0]['nid']) ? $node->field_domain_tenant[LANGUAGE_NONE][0]['nid'] : 0;
    $domain->tenant = normalize_tenant(node_load($tenant_nid));

    $user_uid = isset($node->field_domain_user[LANGUAGE_NONE][0]['uid']) ? $node->field_domain_user[LANGUAGE_NONE][0]['uid'] : 0;
    $domain->user = user_load($user_uid);

    $cache[$nid] = $domain;

    return $domain;
}

/**
 * Normalize group node type
 * 
 * @param  mix $node Group node
 * @return mix
 */
function normalize_group($node)
{
    $cache = &drupal_static(__FUNCTION__, array());
    $nid = isset($node->nid) ? (int) $node->nid : 0;
    if (isset($cache[$nid]))
    {
        return $cache[$nid];
    }

    $group = new stdClass;
    $group->nid = isset($node->nid) ? (int) $node->nid : 0;
    $group->title = isset($node->title) ? $node->title : '';

    $group->fullname = isset($node->field_group_fullname[LANGUAGE_NONE][0]['value']) ? $node->field_group_fullname[LANGUAGE_NONE][0]['value'] : '';
    $group->user_password = isset($node->field_group_user_password[LANGUAGE_NONE][0]['value']) ? $node->field_group_user_password[LANGUAGE_NONE][0]['value'] : '';
    $group->zimbra_id = isset($node->field_group_zimbra_id[LANGUAGE_NONE][0]['value']) ? $node->field_group_zimbra_id[LANGUAGE_NONE][0]['value'] : '';

    $group_members = isset($node->field_group_members[LANGUAGE_NONE][0]['value']) ? $node->field_group_members[LANGUAGE_NONE][0]['value'] : '';
    $group->members = explode('|', $group_members);

    $domain_nid = isset($node->field_node_domain[LANGUAGE_NONE][0]['nid']) ? $node->field_node_domain[LANGUAGE_NONE][0]['nid'] : 0;
    $group->domain = normalize_domain(node_load($domain_nid));

    $user_uid = isset($node->field_group_user[LANGUAGE_NONE][0]['uid']) ? $node->field_group_user[LANGUAGE_NONE][0]['uid'] : 0;
    $group->user = user_load($user_uid);

    $cache[$nid] = $group;

    return $group;
}

/**
 * Normalize mailbox node type
 * 
 * @param  mix $node Mailbox node
 * @return mix
 */
function normalize_mailbox($node)
{
    $cache = &drupal_static(__FUNCTION__, array());
    $nid = isset($node->nid) ? (int) $node->nid : 0;
    if (isset($cache[$nid]))
    {
        return $cache[$nid];
    }

    $mailbox = new stdClass;
    $mailbox->nid = isset($node->nid) ? (int) $node->nid : 0;
    $mailbox->title = isset($node->title) ? $node->title : '';

    $mailbox->fullname = isset($node->field_mailbox_fullname[LANGUAGE_NONE][0]['value']) ? $node->field_mailbox_fullname[LANGUAGE_NONE][0]['value'] : '';
    $mailbox->password = isset($node->field_mailbox_password[LANGUAGE_NONE][0]['value']) ? $node->field_mailbox_password[LANGUAGE_NONE][0]['value'] : '';
    $mailbox->status = isset($node->field_mailbox_status[LANGUAGE_NONE][0]['value']) ? $node->field_mailbox_status[LANGUAGE_NONE][0]['value'] : '';
    $mailbox->size = isset($node->field_mailbox_size[LANGUAGE_NONE][0]['value']) ? $node->field_mailbox_size[LANGUAGE_NONE][0]['value'] : '';
    $mailbox->zimbra_id = isset($node->field_mailbox_zimbra_id[LANGUAGE_NONE][0]['value']) ? $node->field_mailbox_zimbra_id[LANGUAGE_NONE][0]['value'] : '';

    $domain_nid = isset($node->field_node_domain[LANGUAGE_NONE][0]['nid']) ? $node->field_node_domain[LANGUAGE_NONE][0]['nid'] : 0;
    $mailbox->domain = normalize_domain(node_load($domain_nid));

    $group_nid = isset($node->field_node_group[LANGUAGE_NONE][0]['nid']) ? $node->field_node_group[LANGUAGE_NONE][0]['nid'] : 0;
    $mailbox->group = normalize_group(node_load($group_nid));

    $cache[$nid] = $mailbox;

    return $mailbox;
}

/**
 * Normalize alias node type
 * 
 * @param  mix $node Alias node
 * @return mix
 */
function normalize_alias($node)
{
    $cache = &drupal_static(__FUNCTION__, array());
    $nid = isset($node->nid) ? (int) $node->nid : 0;
    if (isset($cache[$nid]))
    {
        return $cache[$nid];
    }

    $alias = new stdClass;
    $alias->nid = isset($node->nid) ? (int) $node->nid : 0;
    $alias->title = isset($node->title) ? $node->title : '';

    $domain_nid = isset($node->field_node_domain[LANGUAGE_NONE][0]['nid']) ? $node->field_node_domain[LANGUAGE_NONE][0]['nid'] : 0;
    $alias->domain = normalize_domain(node_load($domain_nid));

    $group_nid = isset($node->field_node_group[LANGUAGE_NONE][0]['nid']) ? $node->field_node_group[LANGUAGE_NONE][0]['nid'] : 0;
    $alias->group = normalize_group(node_load($group_nid));

    $mailbox_nid = isset($node->field_alias_mailbox[LANGUAGE_NONE][0]['nid']) ? $node->field_alias_mailbox[LANGUAGE_NONE][0]['nid'] : 0;
    $alias->mailbox = normalize_mailbox(node_load($mailbox_nid));

    $cache[$nid] = $alias;

    return $alias;
}

/**
 * Check request is ajax.
 */
function is_ajax_request()
{
    $requested_with = isset($_SERVER['HTTP_X_REQUESTED_WITH']) ? $_SERVER['HTTP_X_REQUESTED_WITH'] : '';
    return strtolower($requested_with) === 'xmlhttprequest';
}

function is_matches(array $patterns = array(), $subject = '')
{
	$is_match = false;
	$pattern = implode('|', $patterns);
	$is_match = preg_match('/'.$pattern.'/msS', $subject);
	return $is_match;
}

/**
 * Add the text field password
 *
 * @param $type
 *   A node type object.
 *
 * @ingroup issue
 */
function zimbra_add_password_field(
    $type,
    $field_name,
    $field_label = '',
    $weight = 0,
    $multiple = false
)
{
    $field = field_info_field($field_name);
    $instance = field_info_instance('node', $field_name, $type->type);
    if (empty($field))
    {
        $field = array(
            'field_name' => $field_name,
            'type' => 'password_field',
            'entity_types' => array('node'),
        );
        if($multiple === true)
        {
			$field['cardinality'] = FIELD_CARDINALITY_UNLIMITED;
		}
        $field = field_create_field($field);
    }
    if (empty($instance))
    {
        $instance = array(
            'field_name' => $field_name,
            'entity_type' => 'node',
            'bundle' => $type->type,
            'label' => t($field_label),
            'widget' => array('type' => 'password_field', 'weight' => $weight),
            'display' => array(
                'default' => array(
                    'label' => 'hidden',
                    'type' => 'hidden',
                ),
                'teaser' => array(
                    'label' => 'hidden',
                    'type' => 'hidden',
                ),
            ),
        );
        field_create_instance($instance);
    }
    return $instance;
}

/**
 * Add the text field
 *
 * @param $type
 *   A node type object.
 *
 * @ingroup issue
 */
function zimbra_add_text_field(
    $type,
    $field_name,
    $field_label = '',
    $weight = 0,
    $multiple = false
)
{
    $field = field_info_field($field_name);
    $instance = field_info_instance('node', $field_name, $type->type);
    if (empty($field))
    {
        $field = array(
            'field_name' => $field_name,
            'type' => 'text',
            'entity_types' => array('node'),
        );
        if($multiple === true)
        {
			$field['cardinality'] = FIELD_CARDINALITY_UNLIMITED;
		}
        $field = field_create_field($field);
    }
    if (empty($instance))
    {
        $instance = array(
            'field_name' => $field_name,
            'entity_type' => 'node',
            'bundle' => $type->type,
            'label' => t($field_label),
            'widget' => array('type' => 'text_textfield', 'weight' => $weight),
            'display' => array(
                'default' => array(
                    'label' => 'above',
                    'type' => 'text_default',
                ),
                'teaser' => array(
                    'label' => 'hidden',
                    'type' => 'hidden',
                ),
            ),
        );
        field_create_instance($instance);
    }
    return $instance;
}

/**
 * Add the text field
 *
 * @param $type
 *   A node type object.
 *
 * @ingroup issue
 */
function zimbra_add_text_long_field(
    $type,
    $field_name,
    $field_label = '',
    $weight = 0,
    $multiple = false
)
{
    $field = field_info_field($field_name);
    $instance = field_info_instance('node', $field_name, $type->type);
    if (empty($field))
    {
        $field = array(
            'field_name' => $field_name,
            'type' => 'text_long',
            'entity_types' => array('node'),
        );
        if($multiple === true)
        {
			$field['cardinality'] = FIELD_CARDINALITY_UNLIMITED;
		}
        $field = field_create_field($field);
    }
    if (empty($instance))
    {
        $instance = array(
            'field_name' => $field_name,
            'entity_type' => 'node',
            'bundle' => $type->type,
            'label' => t($field_label),
            'widget' => array('type' => 'text_textarea', 'weight' => $weight),
            'display' => array(
                'default' => array(
                    'label' => 'above',
                    'type' => 'text_default',
                ),
                'teaser' => array(
                    'label' => 'hidden',
                    'type' => 'hidden',
                ),
            ),
        );
        field_create_instance($instance);
    }
    return $instance;
}

function zimbra_add_boolean_field(
    $type,
    $field_name,
    $field_label = '',
    $weight = 0,
    $multiple = false
)
{
    $field = field_info_field($field_name);
    $instance = field_info_instance('node', $field_name, $type->type);
    if (empty($field))
    {
        $field = array(
            'field_name' => $field_name,
            'type' => 'list_boolean',
            'entity_types' => array('node'),
            'settings' => array('allowed_values' => array(0 => 0, 1 => 1)),
        );
        if($multiple === true)
        {
			$field['cardinality'] = FIELD_CARDINALITY_UNLIMITED;
		}
        $field = field_create_field($field);
    }
    if (empty($instance))
    {
        $instance = array(
            'field_name' => $field_name,
            'entity_type' => 'node',
            'bundle' => $type->type,
            'label' => t($field_label),
            'widget' => array(
				'type' => 'options_onoff',
				'weight' => $weight,
				'settings' => array('display_label' => 1),
			),
            'display' => array(
                'default' => array(
                    'label' => 'above',
                ),
                'teaser' => array(
                    'label' => 'hidden',
                    'type' => 'hidden',
                ),
            ),
        );
        field_create_instance($instance);
    }
    return $instance;
}

/**
 * Add the Integer field
 *
 * @param $type
 *   A node type object.
 *
 * @ingroup issue
 */
function zimbra_add_integer_field(
    $type,
    $field_name,
    $field_label = '',
    $weight = 0,
    $suffix = '',
    $multiple = false
)
{
    $field = field_info_field($field_name);
    $instance = field_info_instance('node', $field_name, $type->type);
    if (empty($field))
    {
        $field = array(
            'field_name' => $field_name,
            'type' => 'number_integer',
            'entity_types' => array('node'),
        );
        if($multiple === true)
        {
			$field['cardinality'] = FIELD_CARDINALITY_UNLIMITED;
		}
        $field = field_create_field($field);
    }
    if (empty($instance))
    {
        $instance = array(
            'field_name' => $field_name,
            'entity_type' => 'node',
            'bundle' => $type->type,
            'label' => t($field_label),
            'widget' => array('type' => 'number', 'weight' => $weight),
            'display' => array(
                'default' => array(
                    'label' => 'above',
                    'type' => 'number_integer',
                ),
                'teaser' => array(
                    'label' => 'hidden',
                    'type' => 'hidden',
                ),
            ),
            'settings' => array( 'suffix' => $suffix,),
        );
        field_create_instance($instance);
    }
    return $instance;
}

/**
 * Add the hidden text field
 *
 * @param $type
 *   A node type object.
 *
 * @ingroup issue
 */
function zimbra_add_hidden_text_field(
    $type,
    $field_name,
    $field_label = '',
    $weight = 0,
    $multiple = false
)
{
    $field = field_info_field($field_name);
    $instance = field_info_instance('node', $field_name, $type->type);
    if (empty($field))
    {
        $field = array(
            'field_name' => $field_name,
            'type' => 'field_hidden_text',
            'entity_types' => array('node'),
        );
        if($multiple === true)
        {
			$field['cardinality'] = FIELD_CARDINALITY_UNLIMITED;
		}
        $field = field_create_field($field);
    }
    if (empty($instance))
    {
        $instance = array(
            'field_name' => $field_name,
            'entity_type' => 'node',
            'bundle' => $type->type,
            'label' => t($field_label),
            'widget' => array('type' => 'field_hidden', 'weight' => $weight),
            'display' => array(
                'default' => array(
                    'label' => 'hidden',
                    'type' => 'hidden',
                ),
                'teaser' => array(
                    'label' => 'hidden',
                    'type' => 'hidden',
                ),
            ),
        );
        field_create_instance($instance);
    }
    return $instance;
}

/**
 * Add the text field
 *
 * @param $type
 *   A node type object.
 *
 * @ingroup issue
 */
function zimbra_add_list_text_field(
    $type,
    $field_name,
    $field_label = '',
    $weight = 0,
    $multiple = false
)
{
    $field = field_info_field($field_name);
    $instance = field_info_instance('node', $field_name, $type->type);
    if (empty($field))
    {
        $field = array(
            'field_name' => $field_name,
            'type' => 'list_text',
            'entity_types' => array('node'),
        );
        if($multiple === true)
        {
			$field['cardinality'] = FIELD_CARDINALITY_UNLIMITED;
		}
        $field = field_create_field($field);
    }
    if (empty($instance))
    {
        $instance = array(
            'field_name' => $field_name,
            'entity_type' => 'node',
            'bundle' => $type->type,
            'label' => t($field_label),
            'widget' => array('type' => 'options_select', 'weight' => $weight),
            'display' => array(
                'default' => array(
                    'label' => 'above',
                    'type' => 'text_default',
                ),
                'teaser' => array(
                    'label' => 'hidden',
                    'type' => 'hidden',
                ),
            ),
        );
        field_create_instance($instance);
    }
    return $instance;
}

/**
 * Add the Node reference field
 *
 * @param $type
 *   A node type object.
 *
 * @ingroup issue
 */
function zimbra_add_node_reference_field(
    $type,
    $field_name,
    $field_label = '',
    $weight = 0,
    $reference = 'article',
    $multiple = false
)
{
    $field = field_info_field($field_name);
    $instance = field_info_instance('node', $field_name, $type->type);
    if (empty($field))
    {
        $field = array(
            'field_name' => $field_name,
            'type' => 'node_reference',
            'entity_types' => array(),
            'settings' => array(
                'referenceable_types' => array($reference => $reference),
            ),
        );
        if($multiple === true)
        {
			$field['cardinality'] = FIELD_CARDINALITY_UNLIMITED;
		}
        $field = field_create_field($field);
    }
    if (empty($instance))
    {
        $instance = array(
            'field_name' => $field_name,
            'entity_type' => 'node',
            'bundle' => $type->type,
            'label' => t($field_label),
            'widget' => array('type' => 'options_select', 'weight' => $weight),
            'display' => array(
                'default' => array(
                    'label' => 'above',
                    'type' => 'node_reference_default',
                ),
                'teaser' => array(
                    'label' => 'hidden',
                    'type' => 'hidden',
                ),
            ),
        );
        field_create_instance($instance);
    }
    return $instance;
}

/**
 * Add the User reference field
 *
 * @param $type
 *   A node type object.
 *
 * @ingroup issue
 */
function zimbra_add_user_reference_field(
    $type,
    $field_name,
    $field_label = '',
    $weight = 0,
    $multiple = false
)
{
    $field = field_info_field($field_name);
    $instance = field_info_instance('node', $field_name, $type->type);
    if (empty($field))
    {
        $field = array(
            'field_name' => $field_name,
            'type' => 'user_reference',
            'entity_types' => array(),
            'settings' => array(
                'referenceable_roles' => array('2' =>2),
            ),
        );
        if($multiple === true)
        {
			$field['cardinality'] = FIELD_CARDINALITY_UNLIMITED;
		}
        $field = field_create_field($field);
    }
    if (empty($instance))
    {
        $instance = array(
            'field_name' => $field_name,
            'entity_type' => 'node',
            'bundle' => $type->type,
            'label' => t($field_label),
            'widget' => array('type' => 'options_select', 'weight' => $weight),
            'display' => array(
                'default' => array(
                    'label' => 'above',
                    'type' => 'user_reference_default',
                ),
                'teaser' => array(
                    'label' => 'hidden',
                    'type' => 'hidden',
                ),
            ),
        );
        field_create_instance($instance);
    }
    return $instance;
}
