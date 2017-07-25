<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * GetQuotaUsageRequest request class
 * Get Quota Usage
 */
class GetQuotaUsageRequest extends SoapRequest {

  /**
   * Sort by enums
   *
   * @var array
   */
  private static $_sortByEnums = array(
    'percentUsed',
    'totalUsed',
    'quotaLimit',
  );

  /**
   * Constructor method for GetQuotaUsage
   * @param string $domain Domain - the domain name to limit the search to
   * @param bool $allServers Ưhether to fetch quota usage for all domain accounts from across all mailbox servers, default is false, applicable when domain attribute is specified
   * @param int $limit Limit - the number of accounts to return (0 is default and means all)
   * @param int $offset Offset - the starting offset (0, 25, etc)
   * @param $sortBy SortBy - valid values: "percentUsed", "totalUsed", "quotaLimit"
   * @param bool $sortAscending Whether to sort in ascending order 0 (false) is default, so highest quotas are returned first
   * @param bool $refresh Refresh - whether to always recalculate the data even when cached values are available. 0 (false) is the default.
   * @return self
   */
  public function __construct(
    $domain = null,
    $allServers = null,
    $limit = null,
    $offset = null,
    $sortBy = null,
    $sortAscending = null,
    $refresh = null
  ) {
    parent::__construct();
    if (NULL !== $domain) {
      $this->domain = trim($domain);
    }
    if (NULL !== $allServers) {
      $this->allServers = (bool) $allServers;
    }
    if (NULL !== $limit) {
        $this->limit = (int) $limit;
    }
    if (NULL !== $offset) {
      $this->offset = (int) $offset;
    }
    if (in_array($sortBy, self::$_sortByEnums)) {
      $this->sortBy = trim($sortBy);
    }
    if (NULL !== $sortAscending) {
      $this->sortAscending = (bool) $sortAscending;
    }
    if (NULL !== $refresh) {
      $this->refresh = (bool) $refresh;
    }
  }
}
