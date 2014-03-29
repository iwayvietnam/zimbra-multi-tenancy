<?php

/**
 * Zimbra pull data form
 */
function zimbra_pull_data_form($form, &$form_state)
{
    global $user;
    $server_id = (int) arg(2);
    $node = node_load($server_id);
    if(!$node)
    {
        drupal_access_denied();
        exit;
    }
    $server = normalize_server($node);

    $form['pull_data'] = array(
        '#type' => 'fieldset',
        '#title' => t('Pull data from :server server', array(':server' => $server->title)),
    );
    $form['pull_data']['data_type'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Data type'),
        '#required' => true,
        '#options' => array(
            'domain' => t('Domain'),
            'group' => t('Group'),
            'mailbox' => t('Mailbox'),
            'alias' => t('Alias'),
        ),
    );

    $tenants = array('_none' => t('-- None --'));
    $user_tenants = server_get_tenant($user->uid);
    foreach ($user_tenants as $nid => $tenant)
    {
        $tenants[$nid] = $tenant->title;
    }

    $form['pull_data']['tenant'] = array(
        '#type' => 'select',
        '#title' => t('Tenant'),
        '#options' => $tenants,
    );

    $form['pull_data']['server_id'] = array(
        '#type' => 'hidden',
        '#value' => $server_id,
    );

    $form['pull_data']['pull_data'] = array(
        '#type' => 'submit',
        '#value' => t('Pull data'),
        '#submit' => array('zimbra_pull_data_callback'),
    );
    return $form;
}

/**
 * Zimbra pull data submit callback
 */
function zimbra_pull_data_callback($form, &$form_state)
{
    $values = $form_state['values'];
    $server_id = (int) $values['server_id'];
    $tenant_id = (int) $values['tenant'];
    $data_types = $values['data_type'];

	$server = normalize_server(node_load($server_id));
    $api = zimbra_admin_api_by_server($server_id);

    foreach ($data_types as $key => $value)
    {
        switch ($value)
        {
            case 'domain':
                $zimbra_domains = zimbra_list_all_domain($api);
                foreach ($zimbra_domains as $zimbra_id => $domain)
                {
                    if(!domain_exists($domain->name))
                    {
                        $user_name = $user_pass = $domain->name;
                        $user_mail = 'admin@'.$domain->name;
                        $user_roles = array();
                        if( variable_get('zimbra_domain_role_id', 0) != 0)
                        {
							$user_roles[] = variable_get('zimbra_domain_role_id', 0);
						}

                        $user_uid = auto_create_user($user_name, $user_pass, $user_mail, $user_roles);

                        $node = new stdClass();
                        $node->type = 'domain';
                        $node->title = $domain->name;
                        $node->promote = 0;
						$node->language = LANGUAGE_NONE;
                        node_object_prepare($node);

                        $node->field_domain_user_mail[LANGUAGE_NONE][0]['value'] = $user_mail;
                        $node->field_domain_user_password[LANGUAGE_NONE][0]['value'] = '';
                        $node->field_domain_tenant[LANGUAGE_NONE][0]['nid'] = $tenant_id;
                        $node->field_domain_server[LANGUAGE_NONE][0]['nid'] = $server_id;
                        $node->field_domain_zimbra_id[LANGUAGE_NONE][0]['value'] = $zimbra_id;
                        $node->field_domain_mailbox_max_size[LANGUAGE_NONE][0]['value'] = 0;
                        $node->field_domain_number_of_mailboxes[LANGUAGE_NONE][0]['value'] = 0;
                        $node->field_domain_user[LANGUAGE_NONE][0]['uid'] = $user_uid;
                        node_save($node);
                    }
                }
                break;
            case 'group':
                $zimbra_dls = zimbra_list_al_dl($api);
                foreach ($zimbra_dls as $zimbra_id => $dl)
                {
                    $group_name = substr($dl->name, 0, (int) strpos($dl->name, '@'));
                    $domain_name = substr($dl->name, (int) strpos($dl->name, '@') + 1);
                    $domain = get_domain_by_name($domain_name);
                    if(!empty($group_name)
                        && ($domain->nid > 0)
                        && !mailbox_exists($group_name, $domain->nid)
                    )
                    {
						$members = zimbra_get_dl_members($api, $dl->name);

                        $user_name = $user_mail = $dl->name;
                        $user_pass = $group_name;
                        
                        $attrs = (isset($dl->a) && is_array($dl->a)) ? $dl->a : array($dl->a);
                        $display_name = $group_name;
                        foreach($attrs as $attr)
                        {
							if($attr->n === 'displayName')
							{
								$display_name = $attr->_;
							}
						}

                        $user_roles = array();
                        if( variable_get('zimbra_group_role_id', 0) != 0) $user_roles[] = variable_get('zimbra_group_role_id', 0);
                        $user_uid = auto_create_user($user_name, $user_pass, $user_mail, $user_roles);

                        $node = new stdClass();
                        $node->type = 'group';
                        $node->title = $group_name;
                        $node->promote = 0;
						$node->language = LANGUAGE_NONE;
                        node_object_prepare($node);

                        $node->field_group_fullname[LANGUAGE_NONE][0]['value'] = $display_name;
                        $node->field_group_user_password[LANGUAGE_NONE][0]['value'] = '';
                        $node->field_node_domain[LANGUAGE_NONE][0]['nid'] = $domain->nid;
                        $node->field_group_zimbra_id[LANGUAGE_NONE][0]['value'] = $zimbra_id;
                        $node->field_group_user[LANGUAGE_NONE][0]['uid'] = $user_uid;
                        foreach($members as $key => $member)
                        {
							$node->field_group_members[LANGUAGE_NONE][$key]['value'] = $member;
						}
                        node_save($node);
                    }
                }
                break;
            case 'mailbox':
                $zimbra_mailboxes = zimbra_list_all_account($api);
                foreach ($zimbra_mailboxes as $zimbra_id => $mailbox)
                {
                    $mailbox_name = substr($mailbox->name, 0, (int) strpos($mailbox->name, '@'));
                    $domain_name = substr($mailbox->name, (int) strpos($mailbox->name, '@') + 1);
                    $domain = get_domain_by_name($domain_name);
                    
                    $excludes = explode("\r\n", $server->exclude_mailbox);
                    $match = is_matches($excludes, $mailbox->name);

                    if(!empty($mailbox_name)
                        && ($domain->nid > 0)
                        && !$match
                        && !mailbox_exists($mailbox_name, $domain->nid)
                    )
                    {
                        $attrs = (isset($mailbox->a) && is_array($mailbox->a)) ? $mailbox->a : array($mailbox->a);
                        $display_name = $mailbox_name;
                        $account_status = 'active';
                        foreach($attrs as $attr)
                        {
							if($attr->n === 'displayName')
							{
								$display_name = $attr->_;
							}
							if($attr->n === 'zimbraAccountStatus')
							{
								$account_status = $attr->_;
							}
						}

                        $node = new stdClass();
                        $node->type = 'mailbox';
                        $node->title = $mailbox_name;
                        $node->promote = 0;
						$node->language = LANGUAGE_NONE;
                        node_object_prepare($node);

                        $node->field_mailbox_fullname[LANGUAGE_NONE][0]['value'] = $display_name;
                        $node->field_mailbox_password[LANGUAGE_NONE][0]['value'] = '';
                        $node->field_mailbox_status[LANGUAGE_NONE][0]['value'] = $account_status;
                        $node->field_node_domain[LANGUAGE_NONE][0]['nid'] = $domain->nid;
                        $node->field_node_group[LANGUAGE_NONE][0]['nid'] = 0;
                        $node->field_mailbox_size[LANGUAGE_NONE][0]['value'] = 0;
                        $node->field_mailbox_zimbra_id[LANGUAGE_NONE][0]['value'] = $zimbra_id;
                        node_save($node);
                    }
                }
                break;
            case 'alias':
                $zimbra_aliases = zimbra_list_all_alias($api);
                foreach ($zimbra_aliases as $zimbra_id => $alias)
                {
                    $alias_name = substr($alias->name, 0, (int) strpos($alias->name, '@'));
                    $account_name = substr($alias->account_name, 0, (int) strpos($alias->account_name, '@'));
                    $domain_name = substr($alias->name, (int) strpos($alias->name, '@') + 1);

                    $domain = get_domain_by_name($domain_name);
                    $account = get_account_by_name($account_name, $domain->nid);

                    $excludes = explode("\r\n", $server->exclude_mailbox);
                    $match = is_matches($excludes, $alias->name);

                    if($account
                        && !empty($alias_name)
                        && ($domain->nid > 0)
                        && !$match
                        && !mailbox_exists($alias_name, $domain->nid)
                    )
                    {
                        $node = new stdClass();
                        $node->type = 'alias';
                        $node->title = $alias_name;
                        $node->promote = 0;
						$node->language = LANGUAGE_NONE;
                        node_object_prepare($node);

                        $node->field_node_domain[LANGUAGE_NONE][0]['nid'] = $domain->nid;
                        $node->field_alias_mailbox[LANGUAGE_NONE][0]['nid'] = $account->nid;
                        node_save($node);
                    }
                }
                break;
        }
    }
}

/**
 * Zimbra server hook form
 */
function zimbra_server_hook_form(&$form, &$form_state)
{
    $form['field_server_service_location'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    $form['field_server_service_location']['#suffix'] = '
        <span>E.g: "https://mail.zimbra.com:7071/service/admin/soap" for Soap.</br />
        "https://mail.zimbra.com:7071/service/wsdl/ZimbraAdminService.wsdl" for WSDL</span>
    ';
    $form['field_server_admin_user'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    $form['field_server_client_type'][LANGUAGE_NONE]['#required'] = 1;

    $form['btn_test_connection'] = array(
        '#type' => 'submit',
        '#value' => t('Test connection'),
        '#ajax' => array(
            'callback' => 'ajax_server_test_connection_callback',
        ),
    );
    $form['#validate'][] = 'zimbra_server_validate';
}

/**
 * Ajax test connection to server callback
 */
function ajax_server_test_connection_callback($form, $form_state)
{
    $values = $form_state['values'];
    $location = $values['field_server_service_location'][LANGUAGE_NONE][0]['value'];
    $client = $values['field_server_client_type'][LANGUAGE_NONE][0]['value'];
    $user = $values['field_server_admin_user'][LANGUAGE_NONE][0]['value'];
	$password = $values['field_server_admin_password'][LANGUAGE_NONE][0]['password_field']['password_field'];
	$password = password_field_decrypt($password);

    $connected = zimbra_test_connection($location, $client, $user, $password);
    if($connected)
    {
        $message = 'Connection to zimbra service success!';
    }
    else
    {
        $message = 'Connection to zimbra service failed!';
    }

    $commands = array();
    $commands[] = ajax_command_alert($message);
    return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Server validation callback
 */
function zimbra_server_validate($form, &$form_state)
{
    if(is_ajax_request())
    {
        return true;
    }
    if(!form_set_error())
    {
        $values = $form_state['values'];
        $location = $values['field_server_service_location'][LANGUAGE_NONE][0]['value'];
        $client = $values['field_server_client_type'][LANGUAGE_NONE][0]['value'];
        $user = $values['field_server_admin_user'][LANGUAGE_NONE][0]['value'];
        $password = $values['field_server_admin_password'][LANGUAGE_NONE][0]['password_field']['password_field'];
        $password = password_field_decrypt($password);

        $connected = zimbra_test_connection($location, $client, $user, $password);
        if(!$connected)
        {
            form_set_error('', t('Connection to zimbra service failed!'));
        }
    }
}

/**
 * Zimbra tenant hook form
 */
function zimbra_ternant_hook_form(&$form, &$form_state)
{
    $tenant_access = false;
    if(user_access('administer zimbra'))
    {
        $tenant_access = true;
    }       
    if($tenant_access === false)
    {
        drupal_access_denied();
        exit;
    }

    $form['field_tenant_user_name'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    $form['field_tenant_user_mail'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    $form['field_tenant_mailbox_max_size'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    $form['field_tenant_number_of_mailboxes'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    $form['field_tenant_server'][LANGUAGE_NONE]['#required'] = 1;
    $form['field_tenant_base'][LANGUAGE_NONE]['#required'] = 1;

    if(arg(1) == 'add')
    {
        $form['field_tenant_user_password'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    }
    if(arg(2) == 'edit')
    {
        $form['field_tenant_user_name'][LANGUAGE_NONE][0]['value']['#attributes'] = array('readonly' => 'readonly');
        $tenant_nid = (int) arg(1);
        $server = ternant_get_server($tenant_nid);
        $form['field_tenant_server'][LANGUAGE_NONE]['#options'] = array($server->nid => $server->title);
    }

    $form['field_tenant_user'][LANGUAGE_NONE]['#options']['_none'] = t('New user');
    $form['field_tenant_user']['#access'] = 0;
    $form['#validate'][] = 'zimbra_tenant_validate';
    $form['#action'] = url($_GET['q'], array('query' => array('destination' => 'zimbra/content/list/tenant')));
}

/**
 * Validate tenant callback
 */
function zimbra_tenant_validate($form, &$form_state)
{
    if(is_ajax_request())
    {
        return true;
    }
    if(!form_set_error())
    {
        $tenant_name = trim($form_state['values']['title']);
        $nid = intval($form_state['values']['nid']);
        
        //Check existence of tenant
        $sql = "SELECT nid AS nid FROM node n WHERE n.nid != $nid AND n.type IN ('tenant') AND n.title ='$tenant_name'";
        $result = db_query($sql);
        $tenant_exist = 0;
        foreach($result as $data)
        {
            if($data->nid !='') $tenant_exist = 1;
        }
        if($tenant_exist == 1)
        {
            form_set_error('title', t('@username already exists.', array('@username' => $form['title']['#title'])));
        }

        //Check user manage tenant
        $tenant_user = $form_state['values']['field_tenant_user'][LANGUAGE_NONE][0]['uid'];
        $user_name = trim($form_state['values']['field_tenant_user_name'][LANGUAGE_NONE][0]['value']);
        $user_pass = trim($form_state['values']['field_tenant_user_password'][LANGUAGE_NONE][0]['value']);
        $user_mail = trim($form_state['values']['field_tenant_user_mail'][LANGUAGE_NONE][0]['value']);
        $tenant_base = trim($form_state['values']['field_tenant_base'][LANGUAGE_NONE][0]['value']);
        
        if( $error = user_validate_name($user_name))
        {
            form_set_error('field_tenant_user_name', $error);
        }
        elseif(empty($tenant_user) && $user_pass == '')
        {
            form_set_error('field_tenant_user_password', t('Please entered password field.'));
        }
        elseif($user_mail != '' && !valid_email_address($user_mail))
        {
            form_set_error('field_tenant_user_mail', t('The e-mail address @mail is not valid.', array('@mail' => $user_mail)));
        }
        
        if(!form_set_error())
        {               
            if(empty($tenant_user))
            {
                //Check username
                $sql = "SELECT uid, name, mail FROM users u WHERE u.name ='$user_name' OR  u.mail ='$user_mail'";
                $result = db_query($sql);
                $user_uid = 0;
                $user_name_exist = 0;
                $user_mail_exist = 0;
                foreach($result as $data)
                {
                    if($data->uid != '') $user_uid = $data->uid;
                    if($data->name == $user_name) $user_name_exist = 1;
                    if($data->mail == $user_mail) $user_mail_exist = 1;
                }

                if($user_uid == 0)
                {
                    $user_roles =array();
                    switch ($tenant_base)
                    {
                        case 'group':
                            if(variable_get('zimbra_tenant_group_role_id', 0) != 0)
                            {
                                $user_roles[] = variable_get('zimbra_tenant_group_role_id', 0);
                            }
                            break;
                        default:
                            if(variable_get('zimbra_tenant_role_id', 0) != 0)
                            {
                                $user_roles[] = variable_get('zimbra_tenant_role_id', 0);
                            }
                            break;
                    }

                    $user_uid = auto_create_user($user_name, $user_pass, $user_mail, $user_roles);
                    $form_state['values']['field_tenant_user'][LANGUAGE_NONE][0]['uid'] = $user_uid;
                }
                else
                {
                    //user already exists
                    if($user_name_exist == 1)
                    {
                        form_set_error('field_tenant_user_name', t('The name @username is already taken.', array('@username' => $user_name)));
                    }
                    if($user_mail_exist == 1)
                    {
                        form_set_error('field_tenant_user_mail', t('The e-mail address @mail is already taken.', array('@mail' => $user_mail)));
                    }
                }
                
            }
            else
            {
                $user = user_load($tenant_user);

                $sql = "SELECT uid, name, mail FROM users u WHERE (u.name ='$user_name' OR u.mail ='$user_mail') AND u.uid !=" .intval($user->uid);
                $result = db_query($sql);
                $user_exist = 0;
                $user_name_exist = 0;
                $user_mail_exist = 0;
                foreach($result as $data)
                {
                    if($data->uid != '') $user_exist = 1;
                    if($data->name == $user_name) $user_name_exist = 1;
                    if($data->mail == $user_mail) $user_mail_exist = 1;
                }
                if($user_exist == 1)
                {
                    if($user_name_exist == 1)
                    {
                        form_set_error('title', t('The name @username is already taken.', array('@username' => $user_name)));
                    }
                    if($user_mail_exist == 1)
                    {
                        form_set_error('field_tenant_user_mail', t('The e-mail address @mail is already taken.', array('@mail' => $user_mail)));
                    }
                }
                else
                {
                    //Change information
                    $edit = array();
                    if($user->name != $user_name)
                    {
                        $edit['name'] = $user_name;
                    }
                    if($user->mail != $user_mail)
                    {
                        $edit['mail'] = $user_mail;
                    }
                    if($user_pass != '')
                    {
                        $edit['pass'] = $user_pass;
                    }
                    $user_roles =array();
                    $role_id = 0;
                    switch ($tenant_base)
                    {
                        case 'group':
                            if(variable_get('zimbra_tenant_group_role_id', 0) != 0)
                            {
                                $role_id = variable_get('zimbra_tenant_group_role_id', 0);
                            }
                            break;
                        default:
                            if(variable_get('zimbra_tenant_role_id', 0) != 0)
                            {
                                $role_id = variable_get('zimbra_tenant_role_id', 0);
                            }
                            break;
                    }
                    $role = user_role_load($role_id);
                    if($role)
                    {
                        $user_roles[$role->rid] = $role->name;
                    }
                    if(count($user_roles))
                    {
                        $edit['roles'] = $user_roles;
                    }

                    if(count($edit) > 0)
                    {
                        user_save($user, $edit);
                    }
                }
            }
        }

        //Clear value of user
        if(!form_set_error())
        {
            $form_state['values']['title'] = trim($form_state['values']['title']);
            $form_state['values']['field_tenant_user_password'][LANGUAGE_NONE][0]['value'] = '';    
        }
    }
}

/**
 * Zimbra domail hook form
 */
function zimbra_domain_hook_form(&$form, &$form_state)
{
    global $user;
    //Check permission create, edit domain
    $tenants = user_get_tenant($user->uid);
    if(count($tenants) === 0)
    {
        drupal_set_message(t('Please <a href="' . url('node/add/tenant') . '">add Tenant</a> before create Domain.'), 'error');
        drupal_access_denied();
        exit;
    }
    $form['field_domain_server'][LANGUAGE_NONE]['#required'] = 1;
    $form['field_domain_tenant'][LANGUAGE_NONE]['#required'] = 1;
    $form['field_domain_user_mail'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    $form['field_domain_mailbox_max_size'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    $form['field_domain_number_of_mailboxes'][LANGUAGE_NONE][0]['value']['#required'] = 1;

    $form['field_domain_server'][LANGUAGE_NONE]['#ajax'] = array(
        'callback' => 'ajax_domain_server_callback',
    );
    $form['field_domain_tenant'][LANGUAGE_NONE]['#prefix'] = '<div id="domain-tenant-replace">';
    $form['field_domain_tenant'][LANGUAGE_NONE]['#suffix'] = '</div>';

    if(arg(1) =='add')
    {
        $form['field_domain_user_password'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    }
    $server_id = 0;
    if(arg(2) == 'edit')
    {
        $domain_id = (int) arg(1);
        $server = domain_get_server($domain_id);
        $server_id = $server->nid;
        $form['field_domain_server'][LANGUAGE_NONE]['#options'] = array($server->nid => $server->title);
    }
    if(isset($form_state['values']['field_domain_server'][LANGUAGE_NONE][0]['nid']))
    {
        $server_id = (int) $form_state['values']['field_domain_server'][LANGUAGE_NONE][0]['nid'];
    }
    if($server_id > 0)
    {
        $tenants = user_get_tenant($user->uid, $server_id);
        $domain_tenant = array();
        foreach ($tenants as $nid => $tenant)
        {
            $domain_tenant[$nid] = $tenant->title;
        }
        if(count($domain_tenant) === 0)
        {
            $domain_tenant = array('_none' => t('- None -'));
        }
        $form['field_domain_tenant'][LANGUAGE_NONE]['#options'] = $domain_tenant;
    }
    else
    {
        $form['field_domain_tenant'][LANGUAGE_NONE]['#options'] = array('_none' => t('- None -'));
    }

    $form['field_domain_user'][LANGUAGE_NONE]['#options']['_none'] = t('New user');
    $form['field_domain_user']['#access'] = 0;
    $form['#validate'][] = 'zimbra_domain_validate';
    $form['#action'] = url($_GET['q'], array('query' => array('destination' =>'zimbra/content/list/domain')));
}

/**
 * Ajax domain server dropdown callback
 */
function ajax_domain_server_callback($form, $form_state)
{
    $commands = array();
    $html = render($form['field_domain_tenant']);
    $commands[] = ajax_command_replace("#domain-tenant-replace", $html);
    return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Domain validation callback
 */
function zimbra_domain_validate($form, &$form_state)
{
    if(is_ajax_request())
    {
        return true;
    }
    if(!form_set_error())
    {
        $domain_name = trim($form_state['values']['title']);
        $nid = intval($form_state['values']['nid']);

        //Check invalid of domain
        if(!preg_match('!^[a-zA-Z0-9_.]+$!', $domain_name)
            || !preg_match('/\./', $domain_name)
            || preg_match('/\.$/', $domain_name))
        {
            form_set_error('title', t('Invalid @fieldname.', array('@fieldname' => $form['title']['#title'])));
        }

        //Check existence of domain
        $domain_exist = domain_exists($domain_name, $nid);
        if($domain_exist)
        {
            form_set_error('title', t('@domain already exists.', array('@domain' => $form['title']['#title'])));
        }

        $server_id = (int) $form_state['values']['field_domain_server'][LANGUAGE_NONE][0]['nid'];
        $api = zimbra_admin_api_by_server($server_id);

        //Check validate mailbox maxsize is avaiable of tenant
        $tenant_id = (int) $form_state['values']['field_domain_tenant'][LANGUAGE_NONE][0]['nid'];
        $mailbox_maxsize = intval($form_state['values']['field_domain_mailbox_max_size'][LANGUAGE_NONE][0]['value']);       
        $tenant_mailbox_maxsize_allow = tenant_check_quota($tenant_id, $nid);
        if($mailbox_maxsize > $tenant_mailbox_maxsize_allow)
        {
            form_set_error('field_domain_mailbox_max_size',
                t('@name: the value may be no greater than @number MB.', array(
                    '@name' => $form['field_domain_mailbox_max_size'][LANGUAGE_NONE]['#title'],
                    '@number' => $tenant_mailbox_maxsize_allow
                )
            ));
        }
        
        //Check validate number mailboxes is avaiable of tenant
        $mailbox_number = intval($form_state['values']['field_domain_number_of_mailboxes'][LANGUAGE_NONE][0]['value']);

        $tenant_mailbox_number_allow = tenant_check_number($tenant_id, $nid);
        if($mailbox_number > $tenant_mailbox_number_allow)
        {
            form_set_error('field_domain_number_of_mailboxes',
                t('@name: the value may be no greater than @number Mailbox(es).', array(
                    '@name' => $form['field_domain_number_of_mailboxes'][LANGUAGE_NONE]['#title'],
                    '@number' => $tenant_mailbox_number_allow
                )
            ));
        }

        //Check user manage domain
        $domain_user = $form_state['values']['field_domain_user'][LANGUAGE_NONE][0]['uid'];
        $user_name = $domain_name;
        $user_pass = trim($form_state['values']['field_domain_user_password'][LANGUAGE_NONE][0]['value']);
        $user_mail = trim($form_state['values']['field_domain_user_mail'][LANGUAGE_NONE][0]['value']);

        if( $error = user_validate_name($user_name))
        {
            form_set_error('title', $error);
        }
        elseif(empty($domain_user) && $user_pass == '')
        {
            form_set_error('field_domain_user_password', t('Please entered password field.'));
        }
        elseif($user_mail != '' && !valid_email_address($user_mail))
        {
            form_set_error('field_domain_user_mail', t('The e-mail address @mail is not valid.', array('@mail' => $user_mail)));
        }

        if(!form_set_error())
        {
            if(empty($domain_user))
            {
                //Check username
                $sql = "SELECT uid, name, mail FROM users u WHERE u.name = '$user_name' OR  u.mail = '$user_mail'";
                $result = db_query($sql);
                $user_uid = 0;
                $user_name_exist = 0;
                $user_mail_exist = 0;
                foreach($result as $data)
                {
                    if($data->uid != '') $user_uid = $data->uid;
                    if($data->name == $user_name) $user_name_exist = 1;
                    if($data->mail == $user_mail) $user_mail_exist = 1;
                }
                
                if($user_uid == 0)
                {
                    $user_roles = array();
                    if( variable_get('zimbra_domain_role_id', 0) != 0) $user_roles[] = variable_get('zimbra_domain_role_id', 0);
                    
                    $user_uid = auto_create_user($user_name, $user_pass, $user_mail, $user_roles);
                }
                else
                {
                    //user already exists
                    if($user_name_exist == 1)
                    {
                        form_set_error('title', t('The name @username is already taken.', array('@username' => $user_name)));
                    }
                    if($user_mail_exist == 1)
                    {
                        form_set_error('field_domain_user_mail', t('The e-mail address @mail is already taken.', array('@mail' => $user_mail)));
                    }
                }   
                $form_state['values']['field_domain_user'][LANGUAGE_NONE][0]['uid'] = $user_uid;
            }
            else
            {
                $user = user_load($domain_user);

                $sql = "SELECT uid, name, mail FROM users u WHERE (u.name = '$user_name' OR u.mail ='$user_mail') AND u.uid !=" .intval($user->uid);
                $result = db_query($sql);
                $user_exist = 0;
                $user_name_exist = 0;
                $user_mail_exist = 0;
                foreach($result as $data)
                {
                    if($data->uid !='') $user_exist = 1;
                    if($data->name == $user_name) $user_name_exist = 1;
                    if($data->mail == $user_mail) $user_mail_exist = 1;
                }
                if($user_exist == 1)
                {
                    if($user_name_exist == 1)
                        form_set_error('title', t('The name @username is already taken.', array('@username' => $user_name)));
                    if($user_mail_exist == 1)
                        form_set_error('field_domain_user_mail', t('The e-mail address @mail is already taken.', array('@mail' => $user_mail)));
                }
                else
                {
                    //Change information
                    $edit =array();
                    if($user->name != $user_name) $edit['name'] = $user_name;
                    if($user->mail != $user_mail) $edit['mail'] = $user_mail;
                    if($user_pass != '') $edit['pass'] = $user_pass;
                    if(count($edit) > 0) user_save($user, $edit);
                }               
            }
        }

        //Clear value of user
        if(!form_set_error())
        {
            $form_state['values']['title'] = trim($form_state['values']['title']);
            $form_state['values']['field_domain_user_password'][LANGUAGE_NONE][0]['value'] = '';

            $zimbra_domain = zimbra_get_domain($api, $domain_name);
            if(!$zimbra_domain)
            {
                $zimbra_domain = zimbra_create_domain($api, $domain_name);
            }
            $form_state['values']['field_domain_zimbra_id'][LANGUAGE_NONE][0]['value'] = $zimbra_domain->id;
        }
    }   
}

function zimbra_group_hook_form(&$form, &$form_state)
{
    global $user;
    drupal_add_css(drupal_get_path('module', 'zimbra_multi_tenancy') . '/zimbra_multi_tenancy.css');

    $domains = user_get_domain($user->uid);
    if( count($domains) < 1)
    {
        drupal_set_message(t('Please <a href="' . url('node/add/domain') . '">add Domain</a> before create Group.'), 'error');
        drupal_access_denied();
        exit;
    }
    $group_nid = 0;
    $domain_id = 0;
    //Check Group edit permission
    if(arg(2) == 'edit')
    {
        $group_nid = (int) arg(1);
        $domain_id = (int) domain_get_id_from_mailbox($group_nid);
        $allow_edit = 0;
        if(count($domains) > 0)
        {
            $nids = array_keys($domains);
            foreach($nids as $nid)
            {
                if($domain_id == $nid)
                {
                    $allow_edit = 1;
                    break;
                }
            }
        }
        if($allow_edit != 1)
        {
            drupal_access_denied();
            exit;
        }
        $form['title']['#access'] = 0;
    }
    else
    {
        if(isset($form_state['values']['field_node_domain'][LANGUAGE_NONE][0]['nid']))
        {
            $domain_id = (int) $form_state['values']['field_node_domain'][LANGUAGE_NONE][0]['nid'];
        }
        else
        {
            $domain_id = (int) current(array_keys($domains));
        }
    }

    $nid = (int) $form['nid']['#value'];
    $node = node_load($nid);

    $form['field_node_domain'][LANGUAGE_NONE]['#required'] = 1;
    $form['field_group_fullname'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    $form['field_node_domain'][LANGUAGE_NONE]['#ajax'] = array(
        'callback' => 'ajax_group_domain_callback',
    );

    if(arg(1) == 'add')
    {
        $form['field_group_user_password'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    }

    $accounts = user_get_account_by_domain($domain_id);
    $account_list = array();
    $members_list = array();
    
    $default_members = array();
    if($node)
    {
		$items = field_get_items('node', $node, 'field_group_members');
		foreach($items as $item)
		{
			$default_members[] = $item['value'];
		}
	}

	foreach($accounts as $key => $account)
	{
		if($key != $nid)
		{
			if(in_array($account, $default_members))
			{
				$members_list[$account] = $account;
			}
			else
			{
				$account_list[$account] = $account;
			}
		}
	}

    $form['field_group_account_list'] = array(
        '#type' =>'select',
        '#title' =>'Account list',
        '#multiple' => 1,
        '#size' => 8,
        '#weight' => 5,
        '#options' => $account_list,
        '#attributes' => array(
            'class' => array('group_account_list'),
        ),
        '#prefix' => '<div id="group-account-replace"><table cellspacing="0" cellpadding="0" border="0" class="tbl-add-remove-wrap"> <tr> <td class="td-item-left">',
        '#suffix' => '
        </td>
        <td class="td-item-center">
            <div class="form-item">
                <div class="add"><input type="button" value="' . t('=>') . '" id="members_add"></div>
                <div class="remove"><input type="button" value="' . t('<=') . '" id="members_remove"></div>
            </div>
        </td>
        <td class="td-item-right">',
    );

    $form['field_group_members_list'] = array(
        '#type' => 'select',
        '#title' => 'Members list',
        '#multiple' => 1,
        '#size' => 8,
        '#weight' => 8,
        '#options' => $members_list,
        '#attributes' => array(
            'class' => array('group_members_list'),
        ),
        '#suffix' => '</td></tr></table></div>',
    );
    $form['field_group_members_value'] = array(
        '#type' => 'hidden',
        '#default_value' => '',
        '#attributes' => array(
            'id' => 'field_group_members_value'
        ),
    );

	hide($form['field_group_members']);
    if(arg(2) == 'edit')
    {
        $domain = node_load($domain_id);
        $form['field_node_domain'][LANGUAGE_NONE]['#options'] = array($domain->nid => $domain->title);
    }
    else
    {
        $group_domains = array();
        foreach ($domains as $nid => $domain)
        {
            $group_domains[$nid] = $domain->title;
        }
        $form['field_node_domain'][LANGUAGE_NONE]['#options'] = $group_domains;
    }

    $form['field_group_user'][LANGUAGE_NONE]['#options']['_none'] = t('New user');
    $form['field_group_user']['#access'] = 0;
    $form['#validate'][] = 'zimbra_group_validate';
    $form['#action'] = url($_GET['q'], array('query' => array('destination' =>'zimbra/content/list/group')));
}

function ajax_group_domain_callback($form, $form_state)
{
    $commands = array();
    $html = render($form['field_group_account_list']) . render($form['field_group_members_list']);
    $commands[] = ajax_command_replace("#group-account-replace", $html);
    return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Validate group
 */
function zimbra_group_validate($form, &$form_state)
{
    global $user;
    if(is_ajax_request())
    {
        return true;
    }
    if(!form_set_error())
    {
        $nid = intval($form_state['values']['nid']);
        $mailbox_name = trim($form_state['values']['title']);
        $group_fullname = $form_state['values']['field_group_fullname'][LANGUAGE_NONE][0]['value'];
        $group_user_uid = $form_state['values']['field_group_user'][LANGUAGE_NONE][0]['uid'];
        $mailbox_domain = $form_state['values']['field_node_domain'][LANGUAGE_NONE][0]['nid'];

        $mail_domain = node_load($mailbox_domain);
        $mail_address = $mailbox_name .'@' .$mail_domain->title;
        $user_name = $user_mail = $mail_address;
        $user_pass = trim($form_state['values']['field_group_user_password'][LANGUAGE_NONE][0]['value']);

		$members = explode('|', $form_state['values']['field_group_members_value']);
		if(is_array($members))
		{
			$form_state['values']['field_group_members'][LANGUAGE_NONE] = array();
			foreach($members as $key => $member)
			{
				$form_state['values']['field_group_members'][LANGUAGE_NONE][$key]['value'] = $member;
			}
		}

        //Check invalid of Mailbox name
        if(!preg_match('!^[a-zA-Z0-9_\.]+$!', $mailbox_name))
        {
            form_set_error('title', t('The @fieldname must contain only letters, punctuation, numbers, and underscores.', array('@fieldname' =>$form['title']['#title'])));
        }

        //Check existence of mailbox.
        $mailbox_exist = mailbox_exists($mailbox_name, $mailbox_domain, $nid);
        if($mailbox_exist == 1)
        {
            form_set_error('title', t('Group or Mailbox or Alias already exists.', array('@username' => $form['title']['#title'])));
        }

        if($error = user_validate_name($user_name))
        {
            form_set_error('title', $error);
        }
        elseif(empty($group_user_uid) && $user_pass == '')
        {
            form_set_error('field_domain_user_password', t('Please entered password field.'));
        }
        elseif(!empty($user_mail) && !valid_email_address($user_mail))
        {
            form_set_error('field_domain_user_mail', t('The e-mail address @mail is not valid.', array('@mail' => $user_mail)));
        }

        if(!form_set_error())
        {
            if(empty($group_user_uid))
            {
                //Check username
                $sql = "SELECT uid, name, mail FROM users u WHERE u.name = '$user_name' OR  u.mail = '$user_mail'";
                $result = db_query($sql);
                $user_uid = 0;
                $user_name_exist = 0;
                $user_mail_exist = 0;
                foreach($result as $data)
                {
                    if($data->uid != '') $user_uid = $data->uid;
                    if($data->name == $user_name) $user_name_exist = 1;
                    if($data->mail == $user_mail) $user_mail_exist = 1;
                }

                if($user_uid == 0)
                {
                    $user_roles = array();
                    if( variable_get('zimbra_group_role_id', 0) != 0) $user_roles[] = variable_get('zimbra_group_role_id', 0);
                    
                    $user_uid = auto_create_user($user_name, $user_pass, $user_mail, $user_roles);
                }
                else
                {
                    //user already exists
                    if($user_name_exist == 1)
                    {
                        form_set_error('title', t('The name @username is already taken.', array('@username' => $user_name)));
                    }
                    if($user_mail_exist == 1)
                    {
                        form_set_error('field_domain_user_mail', t('The e-mail address @mail is already taken.', array('@mail' => $user_mail)));
                    }
                }   
                $form_state['values']['field_group_user'][LANGUAGE_NONE][0]['uid'] = $user_uid;
            }
            else
            {
                $group_user = user_load($group_user_uid);

                $sql = "SELECT uid, name, mail FROM users u WHERE (u.name ='$user_name' OR u.mail ='$user_mail') AND u.uid !=" .intval($group_user->uid);
                $result = db_query($sql);
                $user_exist = 0;
                $user_name_exist = 0;
                $user_mail_exist = 0;
                foreach($result as $data)
                {
                    if($data->uid !='') $user_exist = 1;
                    if($data->name == $user_name) $user_name_exist = 1;
                    if($data->mail == $user_mail) $user_mail_exist = 1;
                }
                if($user_exist == 1)
                {
                    if($user_name_exist == 1)
                        form_set_error('title', t('The name @username is already taken.', array('@username' => $user_name)));
                    if($user_mail_exist == 1)
                        form_set_error('title', t('The e-mail address @mail is already taken.', array('@mail' => $user_mail)));
                }
                else
                {
                    //Change information
                    $edit =array();
                    if($group_user->name != $user_name) $edit['name'] = $user_name;
                    if($group_user->mail != $user_mail) $edit['mail'] = $user_mail;
                    if($user_pass != '') $edit['pass'] = $user_pass;
                    if(count($edit) > 0)
                    {
                        user_save($group_user, $edit);
                    }
                }
            }
        }

        $server_id = server_get_id_from_domain($mailbox_domain);
        $api = zimbra_admin_api_by_server($server_id);

        if(!form_set_error())
        {
            $form_state['values']['title'] = trim($form_state['values']['title']);
            $form_state['values']['field_group_user_password'][LANGUAGE_NONE][0]['value'] = '';

            $zimbra_dl = zimbra_get_dl($api, $mail_address);
            if(!$zimbra_dl)
            {
                $zimbra_dl = zimbra_create_dl($api, $mail_address, array(
                    'displayName' => $group_fullname,
                    'description' => $group_fullname,
                ));
            }
            else
            {
                zimbra_modify_dl($api, $zimbra_dl->id, array(
                    'displayName' => $group_fullname,
                    'description' => $group_fullname,
                ));
            }
            $form_state['values']['field_group_zimbra_id'][LANGUAGE_NONE][0]['value'] = $zimbra_dl->id;

            $zimbra_dlm = isset($zimbra_dl->dlm) ? $zimbra_dl->dlm : array();
            $zimbra_members = is_array($zimbra_dlm) ? $zimbra_dlm : array($zimbra_dlm);
            $remove_members = array();
            foreach ($zimbra_members as $member)
            {
                if(!in_array($member, $members))
                {
                    $remove_members[] = $member;
                }
            }
            if(count($remove_members))
            {
                zimbra_remove_dl_members($api, $zimbra_dl->id, $remove_members);
            }

            if(count($members))
            {
                $add_members = array();
                foreach ($members as $member)
                {
                    if(!in_array($member, $zimbra_members))
                    {
                        $add_members[] = $member;
                    }
                }
                zimbra_add_dl_members($api, $zimbra_dl->id, $add_members);
            }
        }
    }
}

function zimbra_alias_hook_form(&$form, &$form_state)
{
    global $user;
    $mailboxes = user_get_mailbox($user->uid);
    $domains = user_get_domain($user->uid);

    $domain_id = 0;
    if( count($domains) === 0)
    {
        drupal_set_message(t('Please <a href="' . url('node/add/domain') . '">add Domain</a> before create Alias.'), 'error');
        drupal_access_denied();
        exit;
    }
    //Check Alias edit permission
    if(arg(2) == 'edit')
    {
        $domain_id = domain_get_id_from_mailbox(arg(1));
        $allow_edit = 0;
        if(count($domains) > 0)
        {
            foreach($domains as $key => $value)
            {
                if($domain_id == $key)
                {
                    $allow_edit = 1;
                    break;
                }
            }
        }
        if($allow_edit != 1)
        {
            drupal_access_denied();
            exit;
        }
    }
    else
    {
        if(isset($form_state['values']['field_node_domain'][LANGUAGE_NONE][0]['nid']))
        {
            $domain_id = (int) $form_state['values']['field_node_domain'][LANGUAGE_NONE][0]['nid'];
        }
        else
        {
            $domain_id = (int) current(array_keys($domains));
        }
    }

    $form['field_node_domain'][LANGUAGE_NONE]['#required'] = 1;
    $form['field_alias_mailbox'][LANGUAGE_NONE]['#required'] = 1;

    if(arg(2) == 'edit')
    {
        $domain = node_load($domain_id);
        $form['field_node_domain'][LANGUAGE_NONE]['#options'] = array($domain->nid => $domain->title);
    }
    else
    {
        $alias_domains = array();
        foreach ($domains as $nid => $domain)
        {
            $alias_domains[$nid] = $domain->title;
        }
        $form['field_node_domain'][LANGUAGE_NONE]['#options'] = $alias_domains;
    }
    $form['field_node_domain'][LANGUAGE_NONE]['#ajax'] = array(
        'callback' => 'ajax_alias_domain_callback',
    );

    $groups = domain_get_group($domain_id);
    $alias_groups = array();
    foreach ($groups as $nid => $group)
    {
        $alias_groups[$nid] = $group->title . '@' . $group->domain->title;
    }
    $form['field_node_group'][LANGUAGE_NONE]['#options'] = $alias_groups;
    $form['field_node_group'][LANGUAGE_NONE]['#prefix'] = '<div id="alias-group-replace">';
    $form['field_node_group'][LANGUAGE_NONE]['#suffix'] = '</div>';

    $alias_mailboxes = array();
    $domain_mailboxes = domain_get_mailbox($domain_id);
    foreach ($domain_mailboxes as $nid => $mailbox)
    {
        $alias_mailboxes[$nid] = $mailbox->title . '@' . $mailbox->domain->title;
    }
    $form['field_alias_mailbox'][LANGUAGE_NONE]['#options'] = $alias_mailboxes;
    $form['field_alias_mailbox'][LANGUAGE_NONE]['#prefix'] = '<div id="alias-mailbox-replace">';
    $form['field_alias_mailbox'][LANGUAGE_NONE]['#suffix'] = '</div>';

    if(count($mailboxes) == 0)
    {
        $form['field_alias_mailbox'][LANGUAGE_NONE]['#description'] = t('Please <a href="' . url('node/add/mailbox') . '">add Mailbox</a> before create Alias.');
    }
    $form['#validate'][] = 'zimbra_alias_validate';
    $form['#action'] = url($_GET['q'], array('query' => array('destination' => 'zimbra/content/list/alias')));
}

/**
 * Ajax alias domain dropdown callback
 */
function ajax_alias_domain_callback($form, $form_state)
{
    $commands = array();
    $commands[] = ajax_command_replace("#alias-mailbox-replace", render($form['field_alias_mailbox'][LANGUAGE_NONE]));
    $commands[] = ajax_command_replace("#alias-group-replace", render($form['field_node_group'][LANGUAGE_NONE]));
    return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Validate alias
 */
function zimbra_alias_validate($form, &$form_state)
{
    if(is_ajax_request())
    {
        return true;
    }
    if(!form_set_error())
    {
        $mailbox_name = trim($form_state['values']['title']);
        $mailbox_domain = $form_state['values']['field_node_domain'][LANGUAGE_NONE][0]['nid'];
        $nid = intval($form_state['values']['nid']);

        //Check invalid of Mailbox name
        if(!preg_match('!^[a-zA-Z0-9_\.]+$!', $mailbox_name))
        {
            form_set_error('title', t('The @fieldname must contain only letters, punctuation, numbers, and underscores.', array('@fieldname' => $form['title']['#title'])));
        }

        //Check existence of mailbox.
        $mailbox_exist = mailbox_exists($mailbox_name, $mailbox_domain, $nid);
        if($mailbox_exist == 1)
        {
            form_set_error('title', t('Group or Mailbox or Alias already exists.', array('@username' => $form['title']['#title'])));
        }
        
        if(!form_set_error())
        {
            $form_state['values']['title'] = trim($form_state['values']['title']);
            $mail_domain = node_load($mailbox_domain);
            $alias_address = $mailbox_name . '@' . $mail_domain->title;

            $server_id = server_get_id_from_domain($mailbox_domain);
            $api = zimbra_admin_api_by_server($server_id);

            $mapping_id = $form_state['values']['field_alias_mailbox'][LANGUAGE_NONE][0]['nid'];
            $mapping_node = node_load($mapping_id);
            $zimbra_id = $mapping_node->field_mailbox_zimbra_id[LANGUAGE_NONE][0]['value'];

            if($nid == 0)
            {
                zimbra_add_account_alias($api, $zimbra_id, $alias_address);
            }
            else
            {
                $alias_node = node_load($nid);
                $alias_name = $alias_node->title;
                $alias = $alias_name . '@' . $mail_domain->title;
                zimbra_remove_account_alias($api, $alias, $zimbra_id);

                zimbra_add_account_alias($api, $zimbra_id, $alias_address);
            }
        }
    }
}

function zimbra_mailbox_hook_form(&$form, &$form_state)
{
    global $user;
    $domains = user_get_domain($user->uid);
    if(count($domains) === 0)
    {
        drupal_set_message(t('Please <a href="' . url('node/add/domain') . '">add Domain</a> before create Mailbox.'), 'error');
        drupal_access_denied();
        exit;
    }
    
    $domain_id = 0;
    //Check Mailbox edit permission
    if(arg(2) == 'edit')
    {
        $domain_id = domain_get_id_from_mailbox(arg(1));
        $allow_edit = 0;
        if(count($domains) > 0)
        {
            foreach($domains as $key => $value)
            {
                if($domain_id == $key)
                {
                    $allow_edit = 1;
                    break;
                }
            }
        }
        if($allow_edit != 1)
        {
            drupal_access_denied();
            exit;
        }
    }
    else
    {
        if(isset($form_state['values']['field_node_domain'][LANGUAGE_NONE][0]['nid']))
        {
            $domain_id = (int) $form_state['values']['field_node_domain'][LANGUAGE_NONE][0]['nid'];
        }
        else
        {
            $domain_id = (int) current(array_keys($domains));
        }
    }

    $form['field_mailbox_password'][LANGUAGE_NONE][0]['value']['#default_value'] = '';
	$form['field_mailbox_fullname'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    $form['field_mailbox_status'][LANGUAGE_NONE]['#required'] = 1;

    $form['field_node_domain'][LANGUAGE_NONE]['#required'] = 1;
    $form['field_mailbox_size'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    if(arg(1) == 'add' || arg(3) == 'reset-password')
    {
        $form['field_mailbox_password'][LANGUAGE_NONE][0]['value']['#required'] = 1;
    }

    $form['field_node_domain'][LANGUAGE_NONE]['#ajax'] = array(
        'callback' => 'ajax_mailbox_domain_callback',
    );

    if(arg(2) == 'edit')
    {
        $domain = node_load($domain_id);
        $form['field_node_domain'][LANGUAGE_NONE]['#options'] = array($domain->nid => $domain->title);
    }
    else
    {
        $mailbox_domains = array();
        foreach ($domains as $nid => $domain)
        {
            $mailbox_domains[$nid] = $domain->title;
        }
        $form['field_node_domain'][LANGUAGE_NONE]['#options'] = $mailbox_domains;
    }

    $form['field_node_group'][LANGUAGE_NONE]['#prefix'] = '<div id="mailbox-group-replace">';
    $form['field_node_group'][LANGUAGE_NONE]['#suffix'] = '</div>';

    $groups = domain_get_group($domain_id);
    $mailbox_groups = array();
    foreach ($groups as $nid => $group)
    {
        $mailbox_groups[$nid] = $group->title . '@' . $group->domain->title;
    }
    $form['field_node_group'][LANGUAGE_NONE]['#options'] = $mailbox_groups;

    $form['#validate'][] = 'zimbra_mailbox_validate';      
    $form['#action'] = url($_GET['q'], array('query' => array('destination' => 'zimbra/content/list/mailbox')));
}

function ajax_mailbox_domain_callback($form, $form_state)
{
    $commands = array();
    $commands[] = ajax_command_replace("#mailbox-group-replace", render($form['field_node_group'][LANGUAGE_NONE]));
    return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Validate mailbox callback
 */
function zimbra_mailbox_validate($form, &$form_state)
{
    if(is_ajax_request())
    {
        return true;
    }
    if(!form_set_error())
    {
        $nid = intval($form_state['values']['nid']);
        $mailbox_name = trim($form_state['values']['title']);
        $domain_id = $form_state['values']['field_node_domain'][LANGUAGE_NONE][0]['nid'];
        $group_id = $form_state['values']['field_node_group'][LANGUAGE_NONE][0]['nid'];
        $mail_password = $form_state['values']['field_mailbox_password'][LANGUAGE_NONE][0]['value'];
        $mailbox_maxsize = intval($form_state['values']['field_mailbox_size'][LANGUAGE_NONE][0]['value']);

        //Check invalid of Mailbox name
        if(!preg_match('!^[a-zA-Z0-9_\.]+$!', $mailbox_name))
        {
            form_set_error('title', t('The @fieldname must contain only letters, punctuation, numbers, and underscores.', array('@fieldname' => $form['title']['#title'])));
        }

        //Check existence of mailbox.
        $mailbox_exist = mailbox_exists($mailbox_name, $domain_id, $nid);
        if($mailbox_exist == 1)
        {
            form_set_error('title', t('Group or Mailbox or Alias already exists.', array('@username' => $form['title']['#title'])));
        }       

        //Check validate mailbox maxsize is avaiable of Domain
        $domain_mailbox_maxsize_allow = domain_check_quota($domain_id, $nid);
        if($mailbox_maxsize > $domain_mailbox_maxsize_allow)
        {
            form_set_error('field_mailbox_size',
                t('@name: the value may be no greater than @number MB.', array(
                    '@name' => $form['field_mailbox_size'][LANGUAGE_NONE]['#title'],
                    '@number' => $domain_mailbox_maxsize_allow
                )
            ));
        }

        //Check validate number mailboxes is avaiable of Domain
        $domain_mailbox_maxsize_allow = domain_check_number($domain_id, $nid);
        if($domain_mailbox_maxsize_allow <= 0 && arg(1) == 'add')
        {           
            form_set_error('title', t('Number of Mailbox is limited.'));
        }

        if(!form_set_error())
        {
            $form_state['values']['title'] = trim($form_state['values']['title']);
            $mail_domain = normalize_domain(node_load($domain_id));
            $mail_address = $mailbox_name .'@' .$mail_domain->title;
            $mail_fullname = trim($form_state['values']['field_mailbox_fullname'][LANGUAGE_NONE][0]['value']);

            $server_id = server_get_id_from_domain($domain_id);
            $api = zimbra_admin_api_by_server($server_id);

            if($nid ==0)
            {
                $account = zimbra_get_account($api, $mail_address);
                if(!$account)
                {
                    $attrs = array(
                        'displayName' => $mail_fullname,
                        'sn' => $mail_fullname,
                    );
                    $attrs['zimbraMailQuota'] = $mailbox_maxsize * 1024 * 1024;
                    $account = zimbra_create_account($api, $mail_address, $mail_password, $attrs);
                }
                if($account instanceof Exception)
                {
                    $message = $account->getMessage();
                    form_set_error('title', t($message));
                }
                else
                {
                    $form_state['values']['field_mailbox_zimbra_id'][LANGUAGE_NONE][0]['value'] = $account->id;
                }
            }
            else
            {
                //Change mailbox information                
                $old_node = node_load($nid);
                $old_domain = node_load($old_node->field_node_domain[LANGUAGE_NONE][0]['nid']);
                $old_address = $old_node->title .'@' .$old_domain->title;
                $zimbra_id = $form_state['values']['field_mailbox_zimbra_id'][LANGUAGE_NONE][0]['value'];
                $mailbox_status = $form_state['values']['field_mailbox_status'][LANGUAGE_NONE][0]['value'];

                $attrs = array(
                    'displayName' => $mail_fullname,
                    'sn' => $mail_fullname,
                );
                $attrs['zimbraMailQuota'] = $mailbox_maxsize * 1024 * 1024;
                $attrs['zimbraAccountStatus'] = $mailbox_status;

                if(empty($zimbra_id))
                {
                    $account = zimbra_get_account($api, $old_address);
                    if(!$account)
                    {
                        $account = zimbra_create_account($api, $mail_address, $mail_password, $attrs);
                    }
                    if($account instanceof Exception)
                    {
                        $message = $account->getMessage();
                        form_set_error('title', t($message));
                    }
                    else
                    {
                        $form_state['values']['field_mailbox_zimbra_id'][LANGUAGE_NONE][0]['value'] = $account->id;
                    }
                }
                else
                {
                    zimbra_modify_account($api, $zimbra_id, $attrs);
                }
                //Change password of Mailbox
                if($mail_password !== '' )
                {
                    $result = zimbra_set_password($api, $zimbra_id, $mail_password);
                    if(isset($result->message))
                    {
                        $message = $result->message;
                        form_set_error('field_mailbox_password', t($message));
                    }
                    if($result instanceof Exception)
                    {
                        $message = $account->getMessage();
                        form_set_error('field_mailbox_password', t($message));
                    }
                }
                //Rename Mailbox
                if($old_address !== $mail_address)
                {
                    $result = zimbra_rename_account($api, $zimbra_id, $mail_address);
                    if($result instanceof Exception)
                    {
                        $message = $account->getMessage();
                        form_set_error('title', t($message));
                    }
                }
            }
            $mail_group = normalize_group(node_load($group_id));
            if($mail_group->nid)
            {
                $zimbra_dl = zimbra_get_dl($api, $mail_group->title.'@' .$mail_group->domain->title);

                $zimbra_dlm = isset($zimbra_dl->dlm) ? $zimbra_dl->dlm : array();
                $zimbra_members = is_array($zimbra_dlm) ? $zimbra_dlm : array($zimbra_dlm);
                $add_members = array();
                if(!in_array($mail_address, $zimbra_members))
                {
                    $add_members[] = $mail_address;
                }
                if(count($add_members))
                {
                    zimbra_add_dl_members($api, $zimbra_dl->id, $add_members);
                }
            }
        }
    }   
}
