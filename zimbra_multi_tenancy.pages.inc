<?php

function zimbra_pull_data($server_nid)
{
    $node = node_load($server_nid);
    $server = normalize_server($node);

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

    $zimbra_domains = zimbra_list_all_domain($api);
    foreach ($zimbra_domains as $domain)
    {
        if(!domain_exists($domain->name))
        {
            $node = new stdClass();
            $node->type = 'domain';
            node_object_prepare($node);

            $node->title = $domain->name;
            $node->field_domain_zimbra_id[LANGUAGE_NONE][0]['value'] = $domain->id;
            node_save($node);
        }
    }
    $add_groups = array();
    $zimbra_dls = zimbra_list_al_dl($api, $domain->name);
    foreach ($zimbra_dls as $dl)
    {
        $name = substr($dl->name, 0, (int) strpos($dl->name, '@'));
        if(!empty($name))
        {
        }
    }
}

/**
 * Form builder function for module settings.
 */
function zimbra_multi_tenancy_settings()
{
    $roles = user_roles();
    $form['zimbra_zimbra_role_id'] = array(
        '#type' => 'select',
        '#title' => t('Role manage Zimbra multi tenancy'),
        '#default_value' => variable_get('zimbra_zimbra_role_id', 4),
        '#options' => $roles,
        '#weight' => -5,
    );
    $form['zimbra_tenant_role_id'] = array(
        '#type' => 'select',
        '#title' => t('Role for Tenant domain base'),
        '#default_value' => variable_get('zimbra_tenant_role_id', 5),
        '#options' => $roles,
        '#weight' => -4,
    );
    $form['zimbra_tenant_group_role_id'] = array(
        '#type' => 'select',
        '#title' => t('Role for Tenant group base'),
        '#default_value' => variable_get('zimbra_tenant_group_role_id', 6),
        '#options' => $roles,
        '#weight' => -3,
    );
    $form['zimbra_domain_role_id'] = array(
        '#type' => 'select',
        '#title' => t('Role for Domain'),
        '#default_value' => variable_get('zimbra_domain_role_id', 7),
        '#options' => $roles,
        '#weight' => -2,
    );  
    $form['zimbra_group_role_id'] = array(
        '#type' => 'select',
        '#title' => t('Role for Group'),
        '#default_value' => variable_get('zimbra_group_role_id', 8),
        '#options' => $roles,
        '#weight' => -1,
    );  
  
    $form = system_settings_form($form);
    return $form;
}

function zimbra_multi_tenancy_settings_validate($form, &$form_state)
{
    //Check invalid of Tenant name
    $values = $form_state['values'];
    /*$zimbra_domain_server = strtolower($form_state['values']['zimbra_domain_server']);
    if(!preg_match('!^[a-z0-9_.-]+$!', $zimbra_domain_server))
    {
        form_set_error('zimbra_domain_server', t('The @fieldname must contain only lowercase letters, numbers, and underscores.', array('@fieldname' => $form['zimbra_domain_server']['#title'])));
    }*/
}

function zimbra_page_addcontent()
{
    $type = arg(3);
    
    if($type == '')
    {
        $item = menu_get_item();
        $content = system_admin_menu_block($item);
      
        if (count($content) == 1)
        {
            $item = array_shift($content);
            drupal_goto($item['href']);
        }
        return theme('node_add_list', array('content' => $content));
    }
    else
    {
        drupal_goto('node/add/' . $type);
        return '';
    }
}

function zimbra_page_content()
{
    if(arg(0) == 'admin' && arg(1) == 'zimbra' && arg(2) == 'list')
    {
        $url_reditect = 'zimbra/content/list';
        $url_reditect .= (arg(3) !='') ? '/' . arg(3) : '';
        drupal_goto($url_reditect);
    }
    
    $output = '';
    $zimbra_type = arg(3);
    $additem = 1;
    if(arg(2) == 'list' || arg(0) == 'zimbra') $additem = 0;
    
    switch($zimbra_type)
    {
        case 'server':
            $output = page_list_server();
            break;
        case 'tenant':
            $output = page_list_tenant();
            break;
        case 'domain':
            $output = page_list_domain();
            break;
        case 'group':
            $output = page_list_group();
            break;
        case 'alias':
            $output = page_list_alias();
            break;
        case 'mailbox':
            $output = page_list_mailbox();
            break;
        default:
            $output = page_content_general($additem);
            break;          
    }
    
    return $output;
}

function page_content_general($add_item = 1)
{
    global $user;
    $output = '';
    $menu_items = array();
    if(user_access('create server content'))
        $menu_items[] = l(t('List of @type', array('@type' => t('Server'))),'zimbra/content/list/server');
    if(user_access('create tenant content'))
        $menu_items[] = l(t('List of @type', array('@type' => t('Tenant'))),'zimbra/content/list/tenant');
    if(user_access('create domain content'))
        $menu_items[] = l(t('List of @type', array('@type' => t('Domain'))),'zimbra/content/list/domain');
    if(user_access('create group content'))
        $menu_items[] = l(t('List of @type', array('@type' => t('Group'))),'zimbra/content/list/group');
    if(user_access('create alias content'))
        $menu_items[] = l(t('List of @type', array('@type' => t('Alias'))),'zimbra/content/list/alias');
    if(user_access('create mailbox content'))
        $menu_items[] = l(t('List of @type', array('@type' => t('Mailbox'))),'zimbra/content/list/mailbox');
    
    if($add_item == 1)
    {
        if(user_access('create server content')) $menu_items[] = l(t('Add new Server'),'node/add/server');
        if(user_access('create tenant content')) $menu_items[] = l(t('Add new Tenant'),'node/add/tenant');
        if(user_access('create domain content')) $menu_items[] = l(t('Add new Domain'),'node/add/domain');
        if(user_access('create group content')) $menu_items[] = l(t('Add new Group'),'node/add/group');
        if(user_access('create alias content')) $menu_items[] = l(t('Add new Alias'),'node/add/alias');
        if(user_access('create mailbox content')) $menu_items[] = l(t('Add new Mailbox'),'node/add/mailbox');
    }   
    
    foreach($menu_items as $key => $value)
    {
        $output .= '<li class="item mnu-item-' . $key . '">' . $value . '</li>';
    }   
        
    if($output !='') $output = '<ul class="zimbra-general-page">' . $output . '</ul>';
    return $output; 
}

function page_list_server()
{
    global $user;
    drupal_set_title(t('Server list'));
    $header = array(
        'stt' => t('No.'),
        'title' => array('data' => t('Server name'), 'type' => 'none', 'field' => 'title', 'sort' => 'asc'),
        'location' => array('data' => t('Service location'), 'type' => 'field',
            'specifier' => array(
                'field' => 'field_server_service_location', 'column' => 'value',
            ),
        ),
        'user' => array('data' => t('Admin user'), 'type' => 'field',
            'specifier' => array(
                'field' => 'field_server_admin_user', 'column' => 'value',
            ),
        ),
        'operations' => t('Operations'),
    );
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'server')
        ->tableSort($header)
        ->pager(50);
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $rows = array();
    $destination = drupal_get_destination();
    foreach($nodes as $key => $node)
    {
        $operations = '<ul class="links inline">'
            .'<li class="edit first"><a href="'.url('node/'.$node ->nid.'/edit', array('query' => $destination)) .'">'.t('edit').'</a></li>'
            .'<li class="edit last"><a href="'.url('pull-data/'.$node ->nid, array('query' => $destination)) .'">'.t('pull data').'</a></li>'
        .'</ul>';

        $server = normalize_server($node);

        $row = array();
        $row['stt'] = $key;
        $row['title'] = $server->title;
        $row['location'] = $server->service_location;
        $row['user'] = $server->admin_user;
        $row['operations'] = $operations;
        $rows[] = $row;
    }

    $output = '';
    if(user_access('create server content')) 
        $output .= '<ul class="action-links"><li><a href="' .url('node/add/server', array('query' => $destination)) .'">' .t('Add new Server') .'</a></li></ul>';
    $output .= theme('table', array('header' => $header, 'rows' => $rows)).theme('pager');
    return $output;
}

function page_list_tenant()
{
    global $user;
    drupal_set_title(t('Tenant list'));

    $header = array(
        'stt' => t('No.'),
        'title' => array('data' => t('Tenant name'), 'type' => 'none', 'field' => 'title', 'sort' => 'asc'),
        'user' => t('Tenant user'),
        'maxsize' => array('data' => t('Quota of Mailbox'), 'type' => 'field',
            'specifier' => array(
                'field' => 'field_tenant_mailbox_max_size', 'column' => 'value',
            ),
        ),
        'number' => array('data' => t('Number of Mailbox'), 'type' => 'field',
            'specifier' => array(
                'field' => 'field_tenant_number_of_mailboxes', 'column' => 'value',
            ),
        ),
        'operations' => t('Operations'),
    );
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'tenant')
        ->tableSort($header)
        ->pager(50);
    if(!user_access('administer zimbra'))
    {
        $efq->fieldCondition('field_tenant_user', 'uid', $user->uid, '=');
    }
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $rows = array();
    $destination = drupal_get_destination();
    foreach($nodes as $key => $node)
    {
        $operations = '<ul class="links inline">'
            .'<li class="edit first"><a href="'.url('node/'.$node ->nid.'/edit', array('query' => $destination)) .'">'.t('edit').'</a></li>'
        .'</ul>';
        $tenant = normalize_tenant($node);

        $row = array();
        $row['stt'] = $key;
        $row['title'] = $node->title;
        $row['user'] = $tenant->user->name;
        $row['maxsize'] = ($tenant->mailbox_max_size == '') ? '' : t('@number MB', array('@number' => $tenant->mailbox_max_size));
        $row['number'] = $tenant->number_of_mailboxes;
        $row['operations'] = $operations;
        $rows[] = $row;
    }

    $output = '';
    if(user_access('create tenant content')) 
        $output .= '<ul class="action-links"><li><a href="' .url('node/add/tenant', array('query' => $destination)) .'">' .t('Add new Tenant') .'</a></li></ul>';
    $output .= theme('table', array('header' => $header, 'rows' => $rows)).theme('pager');
    return $output;
}

function page_list_domain()
{
    global $user;
    drupal_set_title(t('Domain list'));

    $header = array(
        'stt' => t('No.'),
        'title' => array('data' => t('Domain name'), 'type' => 'none', 'field' => 'title', 'sort' => 'asc'),
        'user' => t('Domain user'),
        'maxsize' => array('data' => t('Quota of Mailbox'), 'type' => 'field',
            'specifier' => array(
                'field' => 'field_domain_mailbox_max_size', 'column' => 'value',
            ),
        ),
        'number' => array('data' => t('Number of Mailbox'), 'type' => 'field',
            'specifier' => array(
                'field' => 'field_domain_number_of_mailboxes', 'column' => 'value',
            ),
        ),
        'operations' => t('Operations'),
    );
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'domain')
        ->tableSort($header)
        ->pager(50);
    if(!user_access('administer zimbra'))
    {
        $tenants = user_get_tenant($user->uid);
        if(count($tenants))
        {
            $tenant_nids = array_keys($tenants);
            $efq->fieldCondition('field_domain_tenant', 'nid', $tenant_nids, 'IN');
        }
        else
        {
            $efq->fieldCondition('field_domain_user', 'uid', $user->uid, '=');
        }
    }
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $rows = array();
    $destination = drupal_get_destination();
    foreach($nodes as $key => $node)
    {
        $operations = '<ul class="links inline">'
            .'<li class="edit first"><a href="'.url('node/'.$node ->nid.'/edit', array('query' => $destination)) .'">'.t('edit').'</a></li>'
        .'</ul>';
        $domain = normalize_domain($node);

        $row = array();
        $row['stt'] = $key;
        $row['title'] = $node->title;
        $row['user'] = $domain->user->name;
        $row['maxsize'] = ($domain->mailbox_max_size == '') ? '' : t('@number MB', array('@number' => $domain->mailbox_max_size));
        $row['number'] = $domain->number_of_mailboxes;
        $row['operations'] = $operations;
        $rows[] = $row;
    }

    $output = '';
    if(user_access('create domain content')) 
        $output .= '<ul class="action-links"><li><a href="' .url('node/add/domain', array('query' => $destination)) .'">' .t('Add new Domain') .'</a></li></ul>';
    $output .= theme('table', array('header' => $header, 'rows' => $rows)).theme('pager');
    return $output;
}

function page_list_group()
{
    global $user;
    drupal_set_title(t('Group list'));

    $header = array(
        'stt' => t('No.'),
        'title' => array('data' => t('Group'), 'type' => 'none', 'field' => 'title', 'sort' => 'asc'),
        'fullname' => array('data' => t('Group fullname'), 'type' => 'field',
            'specifier' => array(
                'field' => 'field_group_fullname', 'column' => 'value',
            ),
        ),
        'operations' => t('Operations'),
    );
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'group')
        ->tableSort($header)
        ->pager(50);
    if(!user_access('administer zimbra'))
    {
        $domains = user_get_domain($user->uid);
        if(count($domains))
        {
            $domain_nids = array_keys($domains);
            $efq->fieldCondition('field_node_domain', 'nid', $domain_nids, 'IN');
        }
        else
        {
            $efq->fieldCondition('field_group_user', 'uid', $user->uid, '=');
        }
    }
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $rows = array();
    $destination = drupal_get_destination();
    foreach($nodes as $key => $node)
    {
        $operations = '<ul class="links inline">'
            .'<li class="edit first"><a href="'.url('node/'.$node ->nid.'/edit', array('query' => $destination)) .'">'.t('edit').'</a></li>'
        .'</ul>';
        $group = normalize_group($node);
        $group_address = $group->title . '@' . $group->domain->title;

        $row = array();
        $row['stt'] = $key;
        $row['title'] = $group_address;
        $row['fullname'] = $group->fullname;
        $row['operations'] = $operations;
        $rows[] = $row;
    }

    $output = '';
    if(user_access('create group content')) 
        $output .= '<ul class="action-links"><li><a href="' .url('node/add/group', array('query' => $destination)) .'">' .t('Add new Group') .'</a></li></ul>';
    $output .= theme('table', array('header' => $header, 'rows' => $rows)).theme('pager');
    return $output;
}

function page_list_alias(){
    global $user;
    drupal_set_title('Alias list');

    $header = array(
        'stt' => t('No.'),
        'title' => array('data' => t('Alias'), 'type' => 'none', 'field' => 'title', 'sort' => 'asc'),
        'mapping' => t('Mapping mailbox'),
        'operations' => t('Operations'),
    );
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'alias')
        ->tableSort($header)
        ->pager(50);
    if(!user_access('administer zimbra'))
    {
        $domains = user_get_domain($user->uid);
        $domain_nids = array_keys($domains);
        if(count($domain_nids) === 0) $domain_nids = array(0);
        $efq->fieldCondition('field_node_domain', 'nid', $domain_nids, 'IN');

        $groups = user_get_group($user->uid);
        $group_nids = array_keys($groups);
        if(count($group_nids))
        {
            $efq->fieldCondition('field_node_group', 'nid', $group_nids, 'IN');
        }
    }

    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $rows = array();
    $destination = drupal_get_destination();
    foreach($nodes as $key => $node)
    {
        $operations = '<ul class="links inline">'
            .'<li class="edit first"><a href="'.url('node/'.$node ->nid.'/edit', array('query' => $destination)) .'">'.t('edit').'</a></li>'
        .'</ul>';
        $alias = normalize_alias($node);
        $alias_address = $alias->title . '@' . $alias->domain->title;
        $alias_mapping = $alias->mailbox->title . '@' . $alias->domain->title;;

        $row = array();
        $row['stt'] = $key;
        $row['title'] = $alias_address;
        $row['mapping'] = $alias_mapping;
        $row['operations'] = $operations;
        $rows[] = $row;
    }

    $output = '';
    if(user_access('create alias content')) 
        $output .= '<ul class="action-links"><li><a href="' . url('node/add/alias') . '">' . t('Add new Alias') . '</a></li></ul>';
    $output .= theme('table', array('header' => $header, 'rows' => $rows)).theme('pager');
    return $output;
}

function page_list_mailbox(){
    global $user;
    $output = '';
    drupal_set_title('Mailbox list');

    $header = array(
        'stt' => t('No.'),
        'title' => array('data' => t('Email address'), 'type' => 'none', 'field' => 'title', 'sort' => 'asc'),
        'fullname' => array('data' => t('Mailbox fullname'), 'type' => 'field',
            'specifier' => array(
                'field' => 'field_mailbox_fullname', 'column' => 'value',
            ),
        ),
        'maxsize' => array('data' => t('Mailbox size'), 'type' => 'field',
            'specifier' => array(
                'field' => 'field_mailbox_size', 'column' => 'value',
            ),
        ),
        'operations' => t('Operations'),
    );
    $efq = new EntityFieldQuery();
    $efq->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'mailbox')
        ->tableSort($header)
        ->pager(50);
    if(!user_access('administer zimbra'))
    {
        $domains = user_get_domain($user->uid);
        $domain_nids = array_keys($domains);
        if(count($domain_nids) === 0) $domain_nids = array(0);
        $efq->fieldCondition('field_node_domain', 'nid', $domain_nids, 'IN');

        $groups = user_get_group($user->uid);
        $group_nids = array_keys($groups);
        if(count($group_nids))
        {
            $efq->fieldCondition('field_node_group', 'nid', $group_nids, 'IN');
        }
    }
    $result = $efq->execute();
    $nids = (isset($result['node']) AND is_array($result['node'])) ? array_keys($result['node']) : array();
    $nodes = node_load_multiple($nids);
    $rows = array();
    $destination = drupal_get_destination();
    foreach($nodes as $key => $node)
    {
        $operations = '<ul class="links inline">'
            .'<li class="edit first"><a href="'.url('node/'.$node ->nid.'/edit', array('query' => $destination)) .'">'.t('edit').'</a></li>'
        .'</ul>';
        $mailbox = normalize_mailbox($node);
        $mailbox_address = $mailbox->title . '@' . $mailbox->domain->title;

        $row = array();
        $row['stt'] = $key + 1;
        $row['title'] = $mailbox_address;
        $row['fullname'] = $mailbox->fullname;
        $row['maxsize'] = $mailbox->size;
        $row['operations'] = $operations;
        $rows[] = $row;
    }

    $output = '';
    if(user_access('create mailbox content')) 
        $output .= '<ul class="action-links"><li><a href="' .url('node/add/mailbox', array('query' => $destination)) .'">' .t('Add new Mailbox') .'</a></li></ul>';
    $output .= theme('table', array('header' => $header, 'rows' => $rows)).theme('pager');
    return $output;
}
