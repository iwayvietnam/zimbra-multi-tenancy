<?php

/**
 * Implements hook_views_default_views().
 */
function zimbra_multi_tenancy_views_default_views() {
  return array(
    'zimbra_server_view' => _zimbra_server_view(),
    'zimbra_tenant_view' => _zimbra_tenant_view(),
    'zimbra_domain_view' => _zimbra_domain_view(),
    'zimbra_group_view' => _zimbra_group_view(),
    'zimbra_alias_view' => _zimbra_alias_view(),
    'zimbra_mailbox_view' => _zimbra_mailbox_view(),
  );
}

/**
 * Define zimbra server view.
 */
function _zimbra_server_view(){
  $view = new view();
  $view->name = 'zimbra_server_view';
  $view->description = 'Display a list of zimbra admin.';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Zimbra Server';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Zimbra servers';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    3 => '3',
    (int) variable_get('zimbra_zimbra_role_id', 4) => variable_get('zimbra_zimbra_role_id', 4),
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_tags'] = array(
    0 => 'zimbra_server_filter',
  );
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'title' => 'title',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['content'] = '<ul class="action-links"><li><a href="../node/add/server?destination=zimbra/server">Add new Server</a></li></ul>';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'There are no servers available. <a href="../node/add/server?destination=zimbra/server">Add new Server</a>';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Global: View result counter */
  $handler->display->display_options['fields']['counter']['id'] = 'counter';
  $handler->display->display_options['fields']['counter']['table'] = 'views';
  $handler->display->display_options['fields']['counter']['field'] = 'counter';
  $handler->display->display_options['fields']['counter']['label'] = 'No.';
  $handler->display->display_options['fields']['counter']['counter_start'] = '1';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Field: Content: Service location */
  $handler->display->display_options['fields']['field_server_service_location']['id'] = 'field_server_service_location';
  $handler->display->display_options['fields']['field_server_service_location']['table'] = 'field_data_field_server_service_location';
  $handler->display->display_options['fields']['field_server_service_location']['field'] = 'field_server_service_location';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'pull data';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'zimbra/pull-data/[nid]?destination=zimbra/server';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = '';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'edit';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'node/[nid]/edit?destination=zimbra/server';
  $handler->display->display_options['fields']['nothing_1']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = '';
  $handler->display->display_options['fields']['nothing_2']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = 'delete';
  $handler->display->display_options['fields']['nothing_2']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_2']['alter']['path'] = 'node/[nid]/delete?destination=zimbra/server';
  $handler->display->display_options['fields']['nothing_2']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_3']['id'] = 'nothing_3';
  $handler->display->display_options['fields']['nothing_3']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_3']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_3']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing_3']['alter']['text'] = '[nothing] <br> [nothing_1] <br> [nothing_2]';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'server' => 'server',
  );

  /* Display: Zimbra server page */
  $handler = $view->new_display('page', 'Zimbra server page', 'zimbra_server_page');
  $handler->display->display_options['path'] = 'zimbra/server';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Zimbra servers';
  $handler->display->display_options['menu']['weight'] = '1';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $translatables['zimbra_server_view'] = array(
    t('Master'),
    t('Zimbra servers'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('« first'),
    t('‹ previous'),
    t('next ›'),
    t('last »'),
    t('<ul class="action-links"><li><a href="../node/add/server?destination=zimbra/server">Add new Server</a></li></ul>'),
    t('There are no servers available. <a href="../node/add/server?destination=zimbra/server">Add new Server</a>'),
    t('No.'),
    t('Name'),
    t('Service location'),
    t('pull data'),
    t('edit'),
    t('delete'),
    t('Operations'),
    t('[nothing] <br> [nothing_1] <br> [nothing_2]'),
    t('Zimbra server page'),
  );

  return $view;
}
/**
 * Define zimbra tenant view.
 */
function _zimbra_tenant_view(){
  $view = new view();
  $view->name = 'zimbra_tenant_view';
  $view->description = 'Display a list of zimbra tenant';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Zimbra Tenant';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Zimbra tenants';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    3 => '3',
    (int) variable_get('zimbra_zimbra_role_id', 4) => variable_get('zimbra_zimbra_role_id', 4),
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_tags'] = array(
    0 => 'zimbra_tenant_filter',
  );
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'nid' => 'nid',
    'counter' => 'counter',
    'title' => 'title',
    'field_tenant_user_name' => 'field_tenant_user_name',
    'field_tenant_mailbox_max_size' => 'field_tenant_mailbox_max_size',
    'field_tenant_number_of_mailboxes' => 'field_tenant_number_of_mailboxes',
    'nothing' => 'nothing',
    'nothing_1' => 'nothing_1',
    'nothing_2' => 'nothing_2',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'nid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'counter' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_tenant_user_name' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_tenant_mailbox_max_size' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_tenant_number_of_mailboxes' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_1' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_2' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['content'] = '<ul class="action-links"><li><a href="../node/add/tenant?destination=zimbra/tenant">Add new Tenant</a></li></ul>';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'There are no tenants available. <a href="../node/add/tenant?destination=zimbra/tenant">Add new Tenant</a>';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Global: View result counter */
  $handler->display->display_options['fields']['counter']['id'] = 'counter';
  $handler->display->display_options['fields']['counter']['table'] = 'views';
  $handler->display->display_options['fields']['counter']['field'] = 'counter';
  $handler->display->display_options['fields']['counter']['label'] = 'No.';
  $handler->display->display_options['fields']['counter']['counter_start'] = '1';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Content: User name */
  $handler->display->display_options['fields']['field_tenant_user_name']['id'] = 'field_tenant_user_name';
  $handler->display->display_options['fields']['field_tenant_user_name']['table'] = 'field_data_field_tenant_user_name';
  $handler->display->display_options['fields']['field_tenant_user_name']['field'] = 'field_tenant_user_name';
  /* Field: Content: Max mailbox quota */
  $handler->display->display_options['fields']['field_tenant_mailbox_max_size']['id'] = 'field_tenant_mailbox_max_size';
  $handler->display->display_options['fields']['field_tenant_mailbox_max_size']['table'] = 'field_data_field_tenant_mailbox_max_size';
  $handler->display->display_options['fields']['field_tenant_mailbox_max_size']['field'] = 'field_tenant_mailbox_max_size';
  $handler->display->display_options['fields']['field_tenant_mailbox_max_size']['settings'] = array(
    'thousand_separator' => '.',
    'prefix_suffix' => 1,
  );
  /* Field: Content: Max mailbox number */
  $handler->display->display_options['fields']['field_tenant_number_of_mailboxes']['id'] = 'field_tenant_number_of_mailboxes';
  $handler->display->display_options['fields']['field_tenant_number_of_mailboxes']['table'] = 'field_data_field_tenant_number_of_mailboxes';
  $handler->display->display_options['fields']['field_tenant_number_of_mailboxes']['field'] = 'field_tenant_number_of_mailboxes';
  $handler->display->display_options['fields']['field_tenant_number_of_mailboxes']['settings'] = array(
    'thousand_separator' => '.',
    'prefix_suffix' => 1,
  );
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'node/[nid]/edit?destination=zimbra/tenant';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = '';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'node/[nid]/delete?destination=zimbra/tenant';
  $handler->display->display_options['fields']['nothing_1']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing] [nothing_1]';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'tenant' => 'tenant',
  );
  /* Filter criterion: Content: Server (field_tenant_server) */
  $handler->display->display_options['filters']['field_tenant_server_nid']['id'] = 'field_tenant_server_nid';
  $handler->display->display_options['filters']['field_tenant_server_nid']['table'] = 'field_data_field_tenant_server';
  $handler->display->display_options['filters']['field_tenant_server_nid']['field'] = 'field_tenant_server_nid';
  $handler->display->display_options['filters']['field_tenant_server_nid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_tenant_server_nid']['expose']['operator_id'] = 'field_tenant_server_nid_op';
  $handler->display->display_options['filters']['field_tenant_server_nid']['expose']['label'] = 'Server';
  $handler->display->display_options['filters']['field_tenant_server_nid']['expose']['operator'] = 'field_tenant_server_nid_op';
  $handler->display->display_options['filters']['field_tenant_server_nid']['expose']['identifier'] = 'field_tenant_server_nid';
  $handler->display->display_options['filters']['field_tenant_server_nid']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['field_tenant_server_nid']['expose']['remember_roles'] = array(
    2 => '2',
  );

  /* Display: Zimbra tenant page */
  $handler = $view->new_display('page', 'Zimbra tenant page', 'zimbra_tenant_page');
  $handler->display->display_options['path'] = 'zimbra/tenant';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Zimbra tenants';
  $handler->display->display_options['menu']['weight'] = '2';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $translatables['zimbra_tenant_view'] = array(
    t('Master'),
    t('Zimbra tenants'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('« first'),
    t('‹ previous'),
    t('next ›'),
    t('last »'),
    t('<ul class="action-links"><li><a href="../node/add/tenant?destination=zimbra/tenant">Add new Tenant</a></li></ul>'),
    t('There are no tenants available. <a href="../node/add/tenant?destination=zimbra/tenant">Add new Tenant</a>'),
    t('No.'),
    t('Name'),
    t('User name'),
    t('Max mailbox quota'),
    t('Max mailbox number'),
    t('edit'),
    t('delete'),
    t('Operations'),
    t('[nothing] [nothing_1]'),
    t('Server'),
    t('Zimbra tenant page'),
  );

  return $view;
}

/**
 * Define zimbra domain view.
 */
function _zimbra_domain_view(){
  $view = new view();
  $view->name = 'zimbra_domain_view';
  $view->description = 'Display a list of zimbra domain';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Zimbra Domain';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Zimbra domains';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    3 => '3',
    (int) variable_get('zimbra_zimbra_role_id', 4) => variable_get('zimbra_zimbra_role_id', 4),
    (int) variable_get('zimbra_tenant_role_id', 5) => variable_get('zimbra_tenant_role_id', 5),
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['disable_sql_rewrite'] = TRUE;
  $handler->display->display_options['query']['options']['query_tags'] = array(
    0 => 'zimbra_domain_filter',
  );
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'nid' => 'nid',
    'counter' => 'counter',
    'title' => 'title',
    'field_domain_user' => 'field_domain_user',
    'field_domain_mailbox_max_size' => 'field_domain_mailbox_max_size',
    'field_domain_number_of_mailboxes' => 'field_domain_number_of_mailboxes',
    'nothing' => 'nothing',
    'nothing_1' => 'nothing_1',
    'nothing_2' => 'nothing_2',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'nid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'counter' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_domain_user' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_domain_mailbox_max_size' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_domain_number_of_mailboxes' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_1' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_2' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['content'] = '<ul class="action-links"><li><a href="../node/add/domain?destination=zimbra/domain">Add new Domain</a></li></ul>';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'There are no domains available. <a href="../node/add/domain?destination=zimbra/domain">Add new Domain</a>';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Global: View result counter */
  $handler->display->display_options['fields']['counter']['id'] = 'counter';
  $handler->display->display_options['fields']['counter']['table'] = 'views';
  $handler->display->display_options['fields']['counter']['field'] = 'counter';
  $handler->display->display_options['fields']['counter']['label'] = 'No.';
  $handler->display->display_options['fields']['counter']['counter_start'] = '1';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Content: Domain user */
  $handler->display->display_options['fields']['field_domain_user']['id'] = 'field_domain_user';
  $handler->display->display_options['fields']['field_domain_user']['table'] = 'field_data_field_domain_user';
  $handler->display->display_options['fields']['field_domain_user']['field'] = 'field_domain_user';
  $handler->display->display_options['fields']['field_domain_user']['type'] = 'user_reference_plain';
  /* Field: Content: Max mailbox quota */
  $handler->display->display_options['fields']['field_domain_mailbox_max_size']['id'] = 'field_domain_mailbox_max_size';
  $handler->display->display_options['fields']['field_domain_mailbox_max_size']['table'] = 'field_data_field_domain_mailbox_max_size';
  $handler->display->display_options['fields']['field_domain_mailbox_max_size']['field'] = 'field_domain_mailbox_max_size';
  $handler->display->display_options['fields']['field_domain_mailbox_max_size']['settings'] = array(
    'thousand_separator' => '.',
    'prefix_suffix' => 1,
  );
  /* Field: Content: Max mailbox number  */
  $handler->display->display_options['fields']['field_domain_number_of_mailboxes']['id'] = 'field_domain_number_of_mailboxes';
  $handler->display->display_options['fields']['field_domain_number_of_mailboxes']['table'] = 'field_data_field_domain_number_of_mailboxes';
  $handler->display->display_options['fields']['field_domain_number_of_mailboxes']['field'] = 'field_domain_number_of_mailboxes';
  $handler->display->display_options['fields']['field_domain_number_of_mailboxes']['settings'] = array(
    'thousand_separator' => '.',
    'prefix_suffix' => 1,
  );
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'node/[nid]/edit?destination=zimbra/domain';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = '';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'zimbra/[nid]/delete?destination=zimbra/domain';
  $handler->display->display_options['fields']['nothing_1']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing] [nothing_1]';
  $handler->display->display_options['fields']['nothing_2']['element_label_colon'] = FALSE;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'domain' => 'domain',
  );
  /* Filter criterion: Content: Server (field_domain_server) */
  $handler->display->display_options['filters']['field_domain_server_nid']['id'] = 'field_domain_server_nid';
  $handler->display->display_options['filters']['field_domain_server_nid']['table'] = 'field_data_field_domain_server';
  $handler->display->display_options['filters']['field_domain_server_nid']['field'] = 'field_domain_server_nid';
  $handler->display->display_options['filters']['field_domain_server_nid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_domain_server_nid']['expose']['operator_id'] = 'field_domain_server_nid_op';
  $handler->display->display_options['filters']['field_domain_server_nid']['expose']['label'] = 'Server';
  $handler->display->display_options['filters']['field_domain_server_nid']['expose']['operator'] = 'field_domain_server_nid_op';
  $handler->display->display_options['filters']['field_domain_server_nid']['expose']['identifier'] = 'field_domain_server_nid';
  $handler->display->display_options['filters']['field_domain_server_nid']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['field_domain_server_nid']['expose']['remember_roles'] = array(
    2 => '2',
  );

  /* Display: Zimbra domain page */
  $handler = $view->new_display('page', 'Zimbra domain page', 'zimbra_domain_page');
  $handler->display->display_options['exposed_block'] = TRUE;
  $handler->display->display_options['path'] = 'zimbra/domain';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Zimbra domains';
  $handler->display->display_options['menu']['weight'] = '3';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $translatables['zimbra_domain_view'] = array(
    t('Master'),
    t('Zimbra domains'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('« first'),
    t('‹ previous'),
    t('next ›'),
    t('last »'),
    t('<ul class="action-links"><li><a href="../node/add/domain?destination=zimbra/domain">Add new Domain</a></li></ul>'),
    t('There are no domains available. <a href="../node/add/domain?destination=zimbra/domain">Add new Domain</a>'),
    t('No.'),
    t('Name'),
    t('Domain user'),
    t('Max mailbox quota'),
    t('Max mailbox number '),
    t('edit'),
    t('delete'),
    t('Operations'),
    t('[nothing] [nothing_1]'),
    t('Server'),
    t('Zimbra domain page'),
  );

  return $view;
}

/**
 * Define zimbra group view.
 */
function _zimbra_group_view(){
  $view = new view();
  $view->name = 'zimbra_group_view';
  $view->description = 'Display a list of zimbra group';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Zimbra Group';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Zimbra groups';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    3 => '3',
    (int) variable_get('zimbra_zimbra_role_id', 4) => variable_get('zimbra_zimbra_role_id', 4),
    (int) variable_get('zimbra_tenant_role_id', 5) => variable_get('zimbra_tenant_role_id', 5),
    (int) variable_get('zimbra_tenant_group_role_id', 6) => variable_get('zimbra_tenant_group_role_id', 6),
    (int) variable_get('zimbra_domain_role_id', 7) => variable_get('zimbra_domain_role_id', 7),
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_tags'] = array(
    0 => 'zimbra_group_filter',
  );
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'nid' => 'nid',
    'counter' => 'counter',
    'title' => 'title',
    'field_node_domain' => 'field_node_domain',
    'field_group_fullname' => 'field_group_fullname',
    'nothing' => 'nothing',
    'nothing_1' => 'nothing_1',
    'nothing_2' => 'nothing_2',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'nid' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'counter' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_node_domain' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_group_fullname' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_1' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_2' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['content'] = '<ul class="action-links"><li><a href="../node/add/group?destination=zimbra/group">Add new Group</a></li></ul>';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'There are no groups available. <a href="../node/add/group?destination=zimbra/group">Add new Group</a>';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Content: Domain */
  $handler->display->display_options['fields']['field_node_domain']['id'] = 'field_node_domain';
  $handler->display->display_options['fields']['field_node_domain']['table'] = 'field_data_field_node_domain';
  $handler->display->display_options['fields']['field_node_domain']['field'] = 'field_node_domain';
  $handler->display->display_options['fields']['field_node_domain']['label'] = '';
  $handler->display->display_options['fields']['field_node_domain']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_node_domain']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_node_domain']['type'] = 'node_reference_plain';
  /* Field: Global: View result counter */
  $handler->display->display_options['fields']['counter']['id'] = 'counter';
  $handler->display->display_options['fields']['counter']['table'] = 'views';
  $handler->display->display_options['fields']['counter']['field'] = 'counter';
  $handler->display->display_options['fields']['counter']['label'] = 'No.';
  $handler->display->display_options['fields']['counter']['counter_start'] = '1';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['title']['alter']['text'] = '[title]@[field_node_domain]';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Content: Fullname */
  $handler->display->display_options['fields']['field_group_fullname']['id'] = 'field_group_fullname';
  $handler->display->display_options['fields']['field_group_fullname']['table'] = 'field_data_field_group_fullname';
  $handler->display->display_options['fields']['field_group_fullname']['field'] = 'field_group_fullname';
  $handler->display->display_options['fields']['field_group_fullname']['label'] = 'Full name';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'node/[nid]/edit?destination=zimbra/group';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = '';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'zimbra/[nid]/delete?destination=zimbra/group';
  $handler->display->display_options['fields']['nothing_1']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing] [nothing_1]';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'group' => 'group',
  );
  /* Filter criterion: Content: Domain (field_node_domain) */
  $handler->display->display_options['filters']['field_node_domain_nid']['id'] = 'field_node_domain_nid';
  $handler->display->display_options['filters']['field_node_domain_nid']['table'] = 'field_data_field_node_domain';
  $handler->display->display_options['filters']['field_node_domain_nid']['field'] = 'field_node_domain_nid';
  $handler->display->display_options['filters']['field_node_domain_nid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['operator_id'] = 'field_node_domain_nid_op';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['label'] = 'Domain';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['operator'] = 'field_node_domain_nid_op';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['identifier'] = 'field_node_domain_nid';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['remember_roles'] = array(
    2 => '2',
  );

  /* Display: Zimbra group page */
  $handler = $view->new_display('page', 'Zimbra group page', 'zimbra_group_page');
  $handler->display->display_options['exposed_block'] = TRUE;
  $handler->display->display_options['path'] = 'zimbra/group';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Zimbra groups';
  $handler->display->display_options['menu']['weight'] = '4';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $translatables['zimbra_group_view'] = array(
    t('Master'),
    t('Zimbra groups'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('« first'),
    t('‹ previous'),
    t('next ›'),
    t('last »'),
    t('<ul class="action-links"><li><a href="../node/add/group?destination=zimbra/group">Add new Group</a></li></ul>'),
    t('There are no groups available. <a href="../node/add/group?destination=zimbra/group">Add new Group</a>'),
    t('No.'),
    t('Name'),
    t('[title]@[field_node_domain]'),
    t('Full name'),
    t('edit'),
    t('delete'),
    t('Operations'),
    t('[nothing] [nothing_1]'),
    t('Domain'),
    t('Zimbra group page'),
  );

  return $view;
}

/**
 * Define zimbra alias view.
 */
function _zimbra_alias_view(){
  $view = new view();
  $view->name = 'zimbra_alias_view';
  $view->description = 'Display a list of zimbra alias';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Zimbra Alias';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Zimbra aliases';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    3 => '3',
    (int) variable_get('zimbra_zimbra_role_id', 4) => variable_get('zimbra_zimbra_role_id', 4),
    (int) variable_get('zimbra_tenant_role_id', 5) => variable_get('zimbra_tenant_role_id', 5),
    (int) variable_get('zimbra_tenant_group_role_id', 6) => variable_get('zimbra_tenant_group_role_id', 6),
    (int) variable_get('zimbra_domain_role_id', 7) => variable_get('zimbra_domain_role_id', 7),
    (int) variable_get('zimbra_group_role_id', 8) => variable_get('zimbra_group_role_id', 8),
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_tags'] = array(
    0 => 'zimbra_alias_filter',
  );
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'nid' => 'nid',
    'counter' => 'counter',
    'title' => 'title',
    'field_node_domain' => 'field_node_domain',
    'field_alias_mailbox' => 'field_alias_mailbox',
    'nothing' => 'nothing',
    'nothing_1' => 'nothing_1',
    'nothing_2' => 'nothing_2',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'nid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'counter' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_node_domain' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_alias_mailbox' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_1' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_2' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['content'] = '<ul class="action-links"><li><a href="../node/add/alias?destination=zimbra/alias">Add new Alias</a></li></ul>';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'There are no aliases available. <a href="../node/add/alias?destination=zimbra/alias">Add new Alias</a>';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Content: Domain */
  $handler->display->display_options['fields']['field_node_domain']['id'] = 'field_node_domain';
  $handler->display->display_options['fields']['field_node_domain']['table'] = 'field_data_field_node_domain';
  $handler->display->display_options['fields']['field_node_domain']['field'] = 'field_node_domain';
  $handler->display->display_options['fields']['field_node_domain']['label'] = '';
  $handler->display->display_options['fields']['field_node_domain']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_node_domain']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_node_domain']['type'] = 'node_reference_plain';
  /* Field: Global: View result counter */
  $handler->display->display_options['fields']['counter']['id'] = 'counter';
  $handler->display->display_options['fields']['counter']['table'] = 'views';
  $handler->display->display_options['fields']['counter']['field'] = 'counter';
  $handler->display->display_options['fields']['counter']['label'] = 'No.';
  $handler->display->display_options['fields']['counter']['counter_start'] = '1';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Alias';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['title']['alter']['text'] = '[title]@[field_node_domain]';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Content: Mapping mailbox */
  $handler->display->display_options['fields']['field_alias_mailbox']['id'] = 'field_alias_mailbox';
  $handler->display->display_options['fields']['field_alias_mailbox']['table'] = 'field_data_field_alias_mailbox';
  $handler->display->display_options['fields']['field_alias_mailbox']['field'] = 'field_alias_mailbox';
  $handler->display->display_options['fields']['field_alias_mailbox']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['field_alias_mailbox']['alter']['text'] = '[field_alias_mailbox]@[field_node_domain]';
  $handler->display->display_options['fields']['field_alias_mailbox']['type'] = 'node_reference_plain';
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'node/[nid]/edit?destination=zimbra/alias';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = '';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'zimbra/[nid]/delete?destination=zimbra/alias';
  $handler->display->display_options['fields']['nothing_1']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing] [nothing_1]';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'alias' => 'alias',
  );
  /* Filter criterion: Content: Domain (field_node_domain) */
  $handler->display->display_options['filters']['field_node_domain_nid']['id'] = 'field_node_domain_nid';
  $handler->display->display_options['filters']['field_node_domain_nid']['table'] = 'field_data_field_node_domain';
  $handler->display->display_options['filters']['field_node_domain_nid']['field'] = 'field_node_domain_nid';
  $handler->display->display_options['filters']['field_node_domain_nid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['operator_id'] = 'field_node_domain_nid_op';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['label'] = 'Domain';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['operator'] = 'field_node_domain_nid_op';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['identifier'] = 'field_node_domain_nid';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['remember_roles'] = array(
    2 => '2',
  );

  /* Display: Zimbra alias page */
  $handler = $view->new_display('page', 'Zimbra alias page', 'zimbra_alias_page');
  $handler->display->display_options['exposed_block'] = TRUE;
  $handler->display->display_options['path'] = 'zimbra/alias';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Zimbra aliases';
  $handler->display->display_options['menu']['weight'] = '6';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $translatables['zimbra_alias_view'] = array(
    t('Master'),
    t('Zimbra aliases'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('« first'),
    t('‹ previous'),
    t('next ›'),
    t('last »'),
    t('<ul class="action-links"><li><a href="../node/add/alias?destination=zimbra/alias">Add new Alias</a></li></ul>'),
    t('There are no aliases available. <a href="../node/add/alias?destination=zimbra/alias">Add new Alias</a>'),
    t('No.'),
    t('Alias'),
    t('[title]@[field_node_domain]'),
    t('Mapping mailbox'),
    t('[field_alias_mailbox]@[field_node_domain]'),
    t('edit'),
    t('delete'),
    t('Operations'),
    t('[nothing] [nothing_1]'),
    t('Domain'),
    t('Zimbra alias page'),
  );

  return $view;
}

/**
 * Define zimbra mailbox view.
 */
function _zimbra_mailbox_view(){
  $view = new view();
  $view->name = 'zimbra_mailbox_view';
  $view->description = 'Display a list of zimbra mailbox';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Zimbra Mailbox';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Zimbra mailboxes';
  $handler->display->display_options['use_ajax'] = TRUE;
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    3 => '3',
    (int) variable_get('zimbra_zimbra_role_id', 4) => variable_get('zimbra_zimbra_role_id', 4),
    (int) variable_get('zimbra_tenant_role_id', 5) => variable_get('zimbra_tenant_role_id', 5),
    (int) variable_get('zimbra_tenant_group_role_id', 6) => variable_get('zimbra_tenant_group_role_id', 6),
    (int) variable_get('zimbra_domain_role_id', 7) => variable_get('zimbra_domain_role_id', 7),
    (int) variable_get('zimbra_group_role_id', 8) => variable_get('zimbra_group_role_id', 8),
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_tags'] = array(
    0 => 'zimbra_mailbox_filter',
  );
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['reset_button'] = TRUE;
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'nid' => 'nid',
    'counter' => 'counter',
    'title' => 'title',
    'field_node_domain' => 'field_node_domain',
    'field_mailbox_status' => 'field_mailbox_status',
    'field_mailbox_size' => 'field_mailbox_size',
    'nothing' => 'nothing',
    'nothing_1' => 'nothing_1',
    'nothing_2' => 'nothing_2',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'nid' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'counter' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_node_domain' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_mailbox_status' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_mailbox_size' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_1' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'nothing_2' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Header: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['content'] = '<ul class="action-links"><li><a href="../node/add/mailbox?destination=zimbra/mailbox">Add new Mailbox</a></li><li><a href="import-mailbox?destination=zimbra/mailbox">Import Mailbox(es)</a></li></ul>';
  $handler->display->display_options['header']['area']['format'] = 'filtered_html';
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['empty'] = TRUE;
  $handler->display->display_options['empty']['area']['content'] = 'There are no mailboxes available. <a href="../node/add/mailbox?destination=zimbra/mailbox">Add new Mailbox</a>';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  /* Field: Content: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Field: Content: Domain */
  $handler->display->display_options['fields']['field_node_domain']['id'] = 'field_node_domain';
  $handler->display->display_options['fields']['field_node_domain']['table'] = 'field_data_field_node_domain';
  $handler->display->display_options['fields']['field_node_domain']['field'] = 'field_node_domain';
  $handler->display->display_options['fields']['field_node_domain']['label'] = '';
  $handler->display->display_options['fields']['field_node_domain']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_node_domain']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['field_node_domain']['type'] = 'node_reference_plain';
  /* Field: Global: View result counter */
  $handler->display->display_options['fields']['counter']['id'] = 'counter';
  $handler->display->display_options['fields']['counter']['table'] = 'views';
  $handler->display->display_options['fields']['counter']['field'] = 'counter';
  $handler->display->display_options['fields']['counter']['label'] = 'No.';
  $handler->display->display_options['fields']['counter']['counter_start'] = '1';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Name';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = TRUE;
  $handler->display->display_options['fields']['title']['alter']['text'] = '[title]@[field_node_domain]';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Field: Content: Status */
  $handler->display->display_options['fields']['field_mailbox_status']['id'] = 'field_mailbox_status';
  $handler->display->display_options['fields']['field_mailbox_status']['table'] = 'field_data_field_mailbox_status';
  $handler->display->display_options['fields']['field_mailbox_status']['field'] = 'field_mailbox_status';
  /* Field: Content: Quota */
  $handler->display->display_options['fields']['field_mailbox_size']['id'] = 'field_mailbox_size';
  $handler->display->display_options['fields']['field_mailbox_size']['table'] = 'field_data_field_mailbox_size';
  $handler->display->display_options['fields']['field_mailbox_size']['field'] = 'field_mailbox_size';
  $handler->display->display_options['fields']['field_mailbox_size']['settings'] = array(
    'thousand_separator' => '.',
    'prefix_suffix' => 1,
  );
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['text'] = 'edit';
  $handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing']['alter']['path'] = 'node/[nid]/edit?destination=zimbra/mailbox';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_1']['id'] = 'nothing_1';
  $handler->display->display_options['fields']['nothing_1']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_1']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_1']['label'] = '';
  $handler->display->display_options['fields']['nothing_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['text'] = 'delete';
  $handler->display->display_options['fields']['nothing_1']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['nothing_1']['alter']['path'] = 'zimbra/[nid]/delete?destination=zimbra/mailbox';
  $handler->display->display_options['fields']['nothing_1']['element_label_colon'] = FALSE;
  /* Field: Global: Custom text */
  $handler->display->display_options['fields']['nothing_2']['id'] = 'nothing_2';
  $handler->display->display_options['fields']['nothing_2']['table'] = 'views';
  $handler->display->display_options['fields']['nothing_2']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing_2']['label'] = 'Operations';
  $handler->display->display_options['fields']['nothing_2']['alter']['text'] = '[nothing] [nothing_1]';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'mailbox' => 'mailbox',
  );
  /* Filter criterion: Content: Domain (field_node_domain) */
  $handler->display->display_options['filters']['field_node_domain_nid']['id'] = 'field_node_domain_nid';
  $handler->display->display_options['filters']['field_node_domain_nid']['table'] = 'field_data_field_node_domain';
  $handler->display->display_options['filters']['field_node_domain_nid']['field'] = 'field_node_domain_nid';
  $handler->display->display_options['filters']['field_node_domain_nid']['exposed'] = TRUE;
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['operator_id'] = 'field_node_domain_nid_op';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['label'] = 'Domain';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['operator'] = 'field_node_domain_nid_op';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['identifier'] = 'field_node_domain_nid';
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['remember'] = TRUE;
  $handler->display->display_options['filters']['field_node_domain_nid']['expose']['remember_roles'] = array(
    2 => '2',
  );

  /* Display: Zimbra mailbox page */
  $handler = $view->new_display('page', 'Zimbra mailbox page', 'zimbra_mailbox_page');
  $handler->display->display_options['exposed_block'] = TRUE;
  $handler->display->display_options['path'] = 'zimbra/mailbox';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Zimbra mailboxes';
  $handler->display->display_options['menu']['weight'] = '5';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['menu']['context_only_inline'] = 0;
  $translatables['zimbra_mailbox_view'] = array(
    t('Master'),
    t('Zimbra mailboxes'),
    t('more'),
    t('Apply'),
    t('Reset'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Items per page'),
    t('- All -'),
    t('Offset'),
    t('« first'),
    t('‹ previous'),
    t('next ›'),
    t('last »'),
    t('<ul class="action-links"><li><a href="../node/add/mailbox?destination=zimbra/mailbox">Add new Mailbox</a></li><li><a href="import-mailbox?destination=zimbra/mailbox">Import Mailbox(es)</a></li></ul>'),
    t('There are no mailboxes available. <a href="../node/add/mailbox?destination=zimbra/mailbox">Add new Mailbox</a>'),
    t('No.'),
    t('Name'),
    t('[title]@[field_node_domain]'),
    t('Status'),
    t('Quota'),
    t('edit'),
    t('delete'),
    t('Operations'),
    t('[nothing] [nothing_1]'),
    t('Domain'),
    t('Zimbra mailbox page'),
  );

  return $view;
}
