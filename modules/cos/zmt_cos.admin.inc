<?php

/**
 * List all zimbra COS page callback.
 */
function zmt_cos_list_all() {
  global $user;

  drupal_set_title('Class of Services');
  $header = array();
  $header[] = array('data' => t('No.'));
  $header[] = array('data' => t('Name'), 'field' => 'c.name');
  $header[] = array('data' => t('Mailbox quota'), 'field' => 'c.mail_quota');

  $query = db_select('zmt_cos', 'c')->extend('PagerDefault')->extend('TableSort');
  if (!(user_access('administer zmt') || user_access('administer any zmt_cos'))) {
    $query->condition('c.uid', $user->uid);
  }

  $item_per_page = variable_get('zimbra_item_per_page', 20);
  $result = $query
    ->fields('c')
    ->orderByHeader($header)
    ->limit($item_per_page)
    ->execute();

  $destination = drupal_get_destination();

  $rows = array();
  foreach ($result as $key => $cos) {
    $row = array();
    $row[] = $key + 1 + (pager_find_page() * $item_per_page);
    $row[] = array(
      'data' => $cos->name,
      'id' => $cos->name,
    );
    $row[] = array(
      'data' => $cos->mail_quota . ' MB',
      'id' => $cos->mail_quota,
    );
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(
      array(
        'data' => t('There are no COS available.'),
        'colspan' => count($header)
      ),
    );
  }

  $build['cos_table']  = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  $build['cos_pager'] = array('#theme' => 'pager');

  return $build;
}
