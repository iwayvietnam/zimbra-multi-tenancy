<?php

/**
 * Display list command
 */
function zimbra_list_command(){
	$output ='';		
	
  	$header = array(
    	'stt' => t('No.'),
    	'cid' => array('data' => t('Command ID'), 'field' => 'c.cid', 'sort' => 'desc'),
    	'command' => array('data' => t('Command'), 'field' => 'c.command'),
    	'notes' => array('data' => t('Notes'), 'field' => 'c.notes'),
    	'status' => array('data' => t('Status'), 'field' => 'c.status'),
    	'deads' => array('data' => t('Number of deads'), 'field' => 'c.number_of_deads'),
    	'created' => array('data' => t('Time created'), 'field' => 'c.created'),
  	);	
	
  	$query = db_select('zimbra_queuedcommand', 'c')->extend('PagerDefault')->extend('TableSort');
  
	$query->fields('c', array('cid', 'command', 'notes', 'status', 'number_of_deads', 'created' )); 
    
    $row_limit =50;
  	$result = $query
    	->limit($row_limit)
    	->orderByHeader($header)
    	->execute();

	$rows = array();
	$current_page =isset($_GET['page'])? intval($_GET['page']) :0;
	$i =$current_page*$row_limit ;
	$destination = drupal_get_destination();
	foreach($result as $data){
		$i++;
				
		$row =array();
    	$row['stt'] = $i;
    	$row['cid'] = $data ->cid;
    	$row['command'] = $data ->command;
    	$row['notes'] = $data ->notes;
    	$row['status'] = $data ->status;
    	$row['deads'] = $data ->number_of_deads;
    	$row['created'] = ($data ->created !='')? date('d/m/Y H:i:s', $data ->created) : '';
    	$rows[] = $row;
	}
	
	$output .=theme('table', array('header' => $header, 'rows' => $rows,'empty' => t('No log messages available.'))) .theme('pager');
	return $output;
}


/**
 * Form builder function for module settings.
 */
function zimbra_multi_tenancy_settings(){
  $form['zimbra_service_address'] = array(
    '#type' => 'textfield',
    '#title' => t('Zimbra admin server address'),
    '#default_value' => variable_get('zimbra_service_address', 'https://localhost:7071/service/admin/soap'),
    '#description' => t('Zimbra admin server address.'),
	'#require' => 1,
	'#weight' => -10,
  );
  $form['zimbra_client_type'] = array(
    '#type' => 'select',
    '#title' => t('Zimbra client type'),
    '#options' => array(
    	'curl' => t('CURL'),
    	'http' => t('HTTP'),
    	'soap' => t('Soap'),
    	'socket' => t('Socket'),
    	'wsdl' => t('WSDL'),
	),
    '#default_value' => variable_get('zimbra_client_type', 'soap'),
    '#description' => t('Zimbra client type.'),
	'#require' => 1,
	'#weight' => -9,
  );
  $form['zimbra_admin_user'] = array(
    '#type' => 'textfield',
    '#title' => t('Admin user in zimbra server'),
    '#default_value' => variable_get('zimbra_admin_user', ''),
    '#description' => t('User in zimbra server.'),
	'#require' => 1,
	'#weight' => -8,
  );
  $form['zimbra_admin_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Admin password in zimbra server'),
    '#default_value' => variable_get('zimbra_admin_password', ''),
    '#description' => t('Admin password in zimbra server.'),
	'#require' => 1,
	'#weight' => -7,
  );
  $form['zimbra_domain_server'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain for automatic create new user'),
    '#default_value' => variable_get('zimbra_domain_server', 'iwayvietnam.com'),
    '#description' => t('Domain for automatic create new user. Do not input "@" character.'),
	'#require' => 1,
	'#weight' => -6,
  );

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
    '#title' => t('Role manage Tenant'),
    '#default_value' => variable_get('zimbra_tenant_role_id', 5),
  	'#options' => $roles,
    '#weight' => -4,
  );
  $form['zimbra_domain_role_id'] = array(
  	'#type' => 'select',
    '#title' => t('Role manage Domain'),
    '#default_value' => variable_get('zimbra_domain_role_id', 6),
  	'#options' => $roles,
    '#weight' => -3,
  );  
  
  $form = system_settings_form($form);
  return $form;
}

function zimbra_multi_tenancy_settings_validate($form, &$form_state){
	//Check invalid of Tenant name
  $values =$form_state['values'];
  $zimbra_domain_server =strtolower($form_state['values']['zimbra_domain_server']);
  if(!preg_match('!^[a-z0-9_.-]+$!', $zimbra_domain_server)){
	form_set_error('zimbra_domain_server', t('The @fieldname must contain only lowercase letters, numbers, and underscores.', array('@fieldname' =>$form['zimbra_domain_server']['#title'])));
  }
}


function zimbra_page_addcontent(){
	$type =arg(3);
	
	if($type ==''){
	  $item = menu_get_item();
	  $content = system_admin_menu_block($item);
	  
	  if (count($content) == 1) {
		$item = array_shift($content);
		drupal_goto($item['href']);
	  }
	  return theme('node_add_list', array('content' => $content));
	}else{
		drupal_goto('node/add/' .$type);
		return '';
	}
}

function zimbra_page_content(){
	if(arg(0) =='admin' && arg(1) =='zimbra' && arg(2) =='list'){
		$url_reditect ='zimbra/content/list';
		$url_reditect .=(arg(3) !='')? '/' .arg(3):'';
		drupal_goto($url_reditect);
	}
	
	$output ='';
	$zimbra_type =arg(3);
	$additem =1;
	if(arg(2) =='list' || arg(0) =='zimbra') $additem =0;
	
	switch($zimbra_type){
	case 'tenant':
		$output =page_list_tenant();
		break;
	case 'domain':
		$output =page_list_domain();
		break;
	case 'group':
		$output =page_list_group();
		break;
	case 'alias':
		$output =page_list_alias();
		break;
	case 'mailbox':
		$output =page_list_mailbox();
		break;
	default:
		$output =page_content_general($additem);
		break;			
	}
	
	return $output;
}

function page_content_general( $additem =1){
	global $user;
	$output ='';
	$menu_items =array();
	if(user_access('create tenant content'))//(count(user_access_list_tenant($user ->uid)) >0) 
		$menu_items[] =l(t('List of @type', array('@type' => 'Tenant')),'zimbra/content/list/tenant');
	if(user_access('create domain content'))//(count(user_access_list_domain($user ->uid)) >0) 
		$menu_items[] =l(t('List of @type', array('@type' => 'Domain')),'zimbra/content/list/domain');
	if(user_access('create group content'))//(count(user_access_list_domain($user ->uid)) >0) 
		$menu_items[] =l(t('List of @type', array('@type' => 'Group')),'zimbra/content/list/group');
	if(user_access('create alias content'))//(count(user_access_list_domain($user ->uid)) >0) 
		$menu_items[] =l(t('List of @type', array('@type' => 'Alias')),'zimbra/content/list/alias');
	if(user_access('create mailbox content'))//(count(user_access_list_domain($user ->uid)) >0) 
		$menu_items[] =l(t('List of @type', array('@type' => 'Mailbox')),'zimbra/content/list/mailbox');
	
	if( $additem ==1){
		if(user_access('create tenant content')) $menu_items[] =l('Add new Tenant','node/add/tenant');
		if(user_access('create domain content')) $menu_items[] =l('Add new Domain','node/add/domain');
		if(user_access('create group content')) $menu_items[] =l('Add new Group','node/add/group');
		if(user_access('create alias content')) $menu_items[] =l('Add new Alias','node/add/alias');
		if(user_access('create mailbox content')) $menu_items[] =l('Add new Mailbox','node/add/mailbox');
	}	
	
	$i =0;
	foreach($menu_items as $value){
		$i++;
		$output .='<li class="item mnu-item-' .$i .'">' .$value .'</li>';
	}	
		
	if($output !='') $output ='<ul class="zimbra-general-page">' .$output .'</ul>';
	return $output;	
}

function page_list_tenant(){
	global $user;
	$output ='';
	drupal_set_title('Tenant list');
		
	$iWhere ="n.type IN ('tenant')";	
	//Check permission of user
	if(!user_access('administer zimbra')){
		$iWhere .= " AND ftu.field_tenant_user_uid =" .$user->uid;
	}
	
  	$header = array(
    	'stt' => t('No.'),
    	'title' => array('data' => t('Tenant name'), 'field' => 'n.title', 'sort' => 'asc'),
    	'user' => t('Tenant user'),
    	'maxsize' => array('data' => t('Quota of Mailbox'), 'field' => 'fms.field_tenant_mailbox_max_size_value'),
    	'number' => array('data' => t('Number of Mailbox'), 'field' => 'fnm.field_tenant_number_of_mailboxes_value'),
    	'operations' => t('Operations'),
  	);	
	
  	$query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort');
	$query->leftJoin('field_data_field_tenant_user', 'ftu', 'n.nid = ftu.entity_id');
	$query->leftJoin('field_data_field_tenant_mailbox_max_size', 'fms', 'n.nid = fms.entity_id');
	$query->leftJoin('field_data_field_tenant_number_of_mailboxes', 'fnm', 'n.nid = fnm.entity_id');
  
	$query->fields('n', array('nid')); 
	$query->addField('n', 'title', 'tenant_name');
	$query->addField('ftu', 'field_tenant_user_uid', 'tenant_user_uid');
	$query->addField('fms', 'field_tenant_mailbox_max_size_value', 'mailbox_max_size');
	$query->addField('fnm', 'field_tenant_number_of_mailboxes_value', 'number_of_mailboxes');
	    
    $query ->where($iWhere);
    
    $row_limit =50;
  	$result = $query
    	->limit($row_limit)
    	->orderByHeader($header)
    	->execute();

	$rows = array();
	$i =0;
	$destination = drupal_get_destination();
	foreach($result as $data){
		$i++;
		
		$tenant_user =user_load($data ->tenant_user_uid);
		$tenant_user_name =(isset($tenant_user ->name))?$tenant_user ->name :'';
				
		$mailbox_max_size =$data ->mailbox_max_size;
		$number_of_mailboxes =$data ->number_of_mailboxes;
		$operations ='<ul class="links inline">		
			<li class="delete last"><a href="' .url('node/' .$data ->nid .'/edit', array('query' =>$destination)) .'">' .t('edit') .'</a></li>
		</ul>';
		
		$row =array();
		$row['stt'] =$i;
    	$row['title'] = $data ->tenant_name;
    	$row['user'] = $tenant_user_name;
    	$row['maxsize'] = ($mailbox_max_size =='')? '': t('@number MB', array('@number' =>$mailbox_max_size));
    	$row['number'] = $number_of_mailboxes;
    	$row['operations'] = $operations;
    	$rows[] = $row;
	}
	
	if(user_access('create tenant content')) 
		$output .='<ul class="action-links"><li><a href="' .url('node/add/tenant') .'">' .t('Add new Tenant') .'</a></li></ul>';
		
	$output .=theme('table', array('header' => $header, 'rows' => $rows)) .theme('pager');
	return $output;
}

function page_list_domain(){
	global $user;
	$output ='';
	drupal_set_title('Domain list');
	
	$iWhere ="n.type IN ('domain')";
	//Check permission of user
	if(!user_access('administer zimbra')){
		$user_get_tenant =user_get_tenant( $user ->uid);		
		if(count($user_get_tenant) >0){
			$iDomain ='';
			foreach($user_get_tenant as $key =>$value){
				if($iDomain !='') $iDomain .=",";
				$iDomain .= $key;
			}
			$iWhere .=" AND fti.field_domain_tenant_id_nid IN ($iDomain)";
		}else{
			$iWhere .= " AND ftu.field_domain_user_uid =" .$user->uid;
		}
	}
	
  	$header = array(
    	'stt' => t('No.'),
    	'title' => array('data' => t('Domain name'), 'field' => 'n.title', 'sort' => 'asc'),
    	'user' => t('Domain user'),
    	'maxsize' => array('data' => t('Quota of Mailbox'), 'field' => 'fms.field_domain_mailbox_max_size_value'),
    	'number' => array('data' => t('Number of Mailbox'), 'field' => 'fnm.field_domain_number_of_mailboxes_value'),
    	'operations' => t('Operations'),
  	);	
	
  	$query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort');
	$query->leftJoin('field_data_field_domain_user', 'ftu', 'n.nid = ftu.entity_id');
	$query->leftJoin('field_data_field_domain_mailbox_max_size', 'fms', 'n.nid = fms.entity_id');
	$query->leftJoin('field_data_field_domain_number_of_mailboxes', 'fnm', 'n.nid = fnm.entity_id');
	$query->leftJoin('field_data_field_domain_tenant_id', 'fti', 'n.nid = fti.entity_id');
  
	$query->fields('n', array('nid')); 
	$query->addField('n', 'title', 'domain_name');
	$query->addField('ftu', 'field_domain_user_uid', 'domain_user_uid');
	$query->addField('fms', 'field_domain_mailbox_max_size_value', 'mailbox_max_size');
	$query->addField('fnm', 'field_domain_number_of_mailboxes_value', 'number_of_mailboxes');
	$query->addField('fti', 'field_domain_tenant_id_nid', 'domain_tenant_id_nid');
	    
    $query ->where($iWhere);
    
    $row_limit =50;
  	$result = $query
    	->limit($row_limit)
    	->orderByHeader($header)
    	->execute();

	$rows = array();
	$i =0;
	$destination = drupal_get_destination();
	foreach($result as $data){
		$i++;
		
		$domain_user =user_load($data ->domain_user_uid);
		$domain_user_name =(isset($domain_user ->name))?$domain_user ->name :'';
				
		$mailbox_max_size = ($data ->mailbox_max_size =='')? '': t('@number MB', array('@number' =>$data ->mailbox_max_size));
		$number_of_mailboxes =$data ->number_of_mailboxes;
		$operations ='<ul class="links inline">		
			<li class="delete last"><a href="' .url('node/' .$data ->nid .'/edit', array('query' =>$destination)) .'">' .t('edit') .'</a></li>
		</ul>';
		
		$row =array();
		$row['stt'] =$i;
    	$row['title'] = $data ->domain_name;
    	$row['user'] = $domain_user_name;
    	$row['maxsize'] = $mailbox_max_size;
    	$row['number'] = $number_of_mailboxes;
    	$row['operations'] = $operations;
    	$rows[] = $row;
	}
	
	if(user_access('create domain content')) 
		$output .='<ul class="action-links"><li><a href="' .url('node/add/domain') .'">' .t('Add new Domain') .'</a></li></ul>';
		
	$output .=theme('table', array('header' => $header, 'rows' => $rows)) .theme('pager');
	return $output;
}

function page_list_group(){
	global $user;
	$output ='';
	drupal_set_title('Group list');
	
	$iWhere ="n.type IN ('group')";
	
	//Check permission of user
	if(!user_access('administer zimbra')){
		$user_domain =user_get_domain($user ->uid);
		
		$iDomain ='';
		if(count($user_domain) >0){
			foreach( $user_domain as $key=>$value){
				if($iDomain !='') $iDomain .=',';
				$iDomain .=$key;
			}
		}
		if($iDomain =='') $iDomain ="''";
		$iWhere .=" AND fdi.field_node_domain_id_nid IN ($iDomain)";
	}
	
	
  	$header = array(
    	'stt' => t('No.'),
    	'title' => array('data' => t('Group'), 'field' => 'n.title', 'sort' => 'asc'),
    	'fullname' => array('data' => t('Group fullname'), 'field' => 'fgf.field_group_fullname_value'),
    	'operations' => t('Operations'),
  	);	
	
  	$query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort');
	$query->Join('field_data_field_node_domain_id', 'fdi', 'n.nid = fdi.entity_id');
	$query->leftJoin('field_data_field_group_fullname', 'fgf', 'n.nid = fgf.entity_id');
  
	$query->fields('n', array('nid')); 
	$query->addField('n', 'title', 'groupname');
	$query->addField('fgf', 'field_group_fullname_value', 'fullname');
	$query->addField('fdi', 'field_node_domain_id_nid', 'domain_id');
	    
    $query ->where($iWhere);
        
    $row_limit =50;
  	$result = $query
    	->limit($row_limit)
    	->orderByHeader($header)
    	->execute();

	$rows = array();
	$i =0;
	$destination = drupal_get_destination();
	foreach($result as $data){
		$i++;
		$domain_node =node_load($data ->domain_id);
		$domain_name =isset($domain_node ->title)? $domain_node ->title: '';
		
		$group_address =$data ->groupname .'@' .$domain_name;
		$group_fullname =$data ->fullname;
		
		$operations ='<ul class="links inline">		
			<li class="delete last"><a href="' .url('node/' .$data ->nid .'/edit', array('query' =>$destination)) .'">' .t('edit') .'</a></li>
		</ul>';
		
		$row =array();
		$row['stt'] =$i;
    	$row['title'] = $group_address;
    	$row['fullname'] = $group_fullname;
    	$row['operations'] = $operations;
    	$rows[] = $row;
	}
	
	if(user_access('create group content')) 
		$output .='<ul class="action-links"><li><a href="' .url('node/add/group') .'">' .t('Add new Group') .'</a></li></ul>';
		
	$output .=theme('table', array('header' => $header, 'rows' => $rows)) .theme('pager');
	return $output;
}

function page_list_alias(){
	global $user;
	$output ='';
	drupal_set_title('Alias list');
	
	$iWhere ="n.type IN ('alias')";
	
	//Check permission of user
	if(!user_access('administer zimbra')){
		$user_domain =user_get_domain($user ->uid);
		
		$iDomain ='';
		if(count($user_domain) >0){
			foreach( $user_domain as $key=>$value){
				if($iDomain !='') $iDomain .=',';
				$iDomain .=$key;
			}
		}
		if($iDomain =='') $iDomain ="''";
		$iWhere .=" AND fdi.field_node_domain_id_nid IN ($iDomain)";
	}
		
  	$header = array(
    	'stt' => t('No.'),
    	'title' => array('data' => t('Alias'), 'field' => 'n.title', 'sort' => 'asc'),
    	'mapping' => t('Mapping mailbox'),
    	'operations' => t('Operations'),
  	);	
	
  	$query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort');
	$query->Join('field_data_field_node_domain_id', 'fdi', 'n.nid = fdi.entity_id');
	$query->leftJoin('field_data_field_alias_mapping_mailbox', 'fam', 'n.nid = fam.entity_id');
  
	$query->fields('n', array('nid')); 
	$query->addField('n', 'title', 'aliasname');
	$query->addField('fam', 'field_alias_mapping_mailbox_nid', 'mapping_id');
	$query->addField('fdi', 'field_node_domain_id_nid', 'domain_id');
	    
    $query ->where($iWhere);
        
    $row_limit =50;
  	$result = $query
    	->limit($row_limit)
    	->orderByHeader($header)
    	->execute();

	$rows = array();
	$i =0;
	$destination = drupal_get_destination();
	foreach($result as $data){
		$i++;
		$domain_node =node_load($data ->domain_id);
		$domain_name =isset($domain_node ->title)? $domain_node ->title: '';
		
		$alias_address =$data ->aliasname .'@' .$domain_name;
		$alias_mapping ='';
		$mailbox_info =mailbox_get_info($data ->mapping_id);
		if(isset($mailbox_info['mail'])) $alias_mapping =$mailbox_info['mail'];
		
		$operations ='<ul class="links inline">		
			<li class="delete last"><a href="' .url('node/' .$data ->nid .'/edit', array('query' =>$destination)) .'">' .t('edit') .'</a></li>
		</ul>';
		
		$row =array();
		$row['stt'] =$i;
    	$row['title'] = $alias_address;
    	$row['mapping'] = $alias_mapping;
    	$row['operations'] = $operations;
    	$rows[] = $row;
	}
	
	if(user_access('create alias content')) 
		$output .='<ul class="action-links"><li><a href="' .url('node/add/alias') .'">' .t('Add new Alias') .'</a></li></ul>';
		
	$output .=theme('table', array('header' => $header, 'rows' => $rows)) .theme('pager');
	return $output;
}

function page_list_mailbox(){
	global $user;
	$output ='';
	drupal_set_title('Mailbox list');
	
	$iWhere ="n.type IN ('mailbox')";
	
	//Check permission of user
	if(!user_access('administer zimbra')){
		$user_domain =user_get_domain($user ->uid);
		
		$iDomain ='';
		if(count($user_domain) >0){
			foreach( $user_domain as $key=>$value){
				if($iDomain !='') $iDomain .=',';
				$iDomain .=$key;
			}
		}
		if($iDomain =='') $iDomain ="''";
		$iWhere .=" AND fdi.field_node_domain_id_nid IN ($iDomain)";
	}
		
  	$header = array(
    	'stt' => t('No.'),
    	'title' => array('data' => t('Email address'), 'field' => 'n.title', 'sort' => 'asc'),
    	'fullname' => array('data' => t('Mailbox fullname'), 'field' => 'fmf.field_mailbox_fullname_value'),
    	'maxsize' => array('data' => t('Mailbox size'), 'field' => 'fms.field_mailbox_size_value'),
    	'operations' => t('Operations'),
  	);	
	
  	$query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort');
	$query->Join('field_data_field_node_domain_id', 'fdi', 'n.nid = fdi.entity_id');
	$query->leftJoin('field_data_field_mailbox_fullname', 'fmf', 'n.nid = fmf.entity_id');
	$query->leftJoin('field_data_field_mailbox_size', 'fms', 'n.nid = fms.entity_id');
  
	$query->fields('n', array('nid')); 
	$query->addField('n', 'title', 'mailbox');
	$query->addField('fmf', 'field_mailbox_fullname_value', 'fullname');
	$query->addField('fms', 'field_mailbox_size_value', 'mailbox_size');
	$query->addField('fdi', 'field_node_domain_id_nid', 'domain_id');
	    
    $query ->where($iWhere);
    
    $row_limit =50;
  	$result = $query
    	->limit($row_limit)
    	->orderByHeader($header)
    	->execute();

	$rows = array();
	$i =0;
	$destination = drupal_get_destination();
	foreach($result as $data){
		$i++;
		$domain_node =node_load($data ->domain_id);
		$domain_name =isset($domain_node ->title)? $domain_node ->title: '';
		
		$mailbox_address =$data ->mailbox .'@' .$domain_name;
		$mailbox_fullname =$data ->fullname;
		$mailbox_size =($data ->mailbox_size =='')? '': t('@number MB', array('@number' =>$data ->mailbox_size));
		
		$operations ='<ul class="links inline">		
			<li class="delete first"><a href="' .url('node/' .$data ->nid .'/edit', array('query' =>$destination)) .'">' .t('edit') .'</a></li>
			<li class="reset last"><a href="' .url('node/' .$data ->nid .'/edit/reset-password', array('query' =>$destination)) .'">' .t('reset password') .'</a></li>
		</ul>';
		
		$row =array();
		$row['stt'] =$i;
    	$row['title'] = $mailbox_address;
    	$row['fullname'] = $mailbox_fullname;
    	$row['maxsize'] = $mailbox_size;
    	$row['operations'] = $operations;
    	$rows[] = $row;
	}
	
	if(user_access('create mailbox content')) 
		$output .='<ul class="action-links"><li><a href="' .url('node/add/mailbox') .'">' .t('Add new Mailbox') .'</a></li></ul>';
		
	$output .=theme('table', array('header' => $header, 'rows' => $rows)) .theme('pager');
	return $output;
}







