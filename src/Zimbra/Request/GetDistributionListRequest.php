<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;
use Drupal\zmt\Zimbra\Struct\DistributionListSelector;

/**
 * GetDistributionListRequest request class
 * Get a Distribution List
 */
class GetDistributionListRequest extends SoapRequest {

  /**
   * Constructor method for GetDistributionListRequest
   * @param  DistributionListSelector $dl
   *   Distribution List
   * @param  int $limit
   *   The maximum number of accounts to return (0 is default and means all)
   * @param  int $offset
   *   The starting offset (0, 25 etc)
   * @param  bool $sortAscending
   *   Flag whether to sort in ascending order 1 (true) is the default
   * @param  array $attrs
   * @return self
   */
  public function __construct(
    DistributionListSelector $dl = NULL,
    $limit = NULL,
    $offset = NULL,
    $sortAscending = NULL,
    array $attrs = []
  ) {
    parent::__construct();
    if ($dl instanceof DistributionListSelector) {
      $this->dl = $dl;
    }
    if (NULL !== $limit) {
      $this->limit = (int) $limit;
    }
    if (NULL !== $offset) {
      $this->offset = (int) $offset;
    }
    if (NULL !== $sortAscending) {
      $this->sortAscending = (bool) $sortAscending;
    }
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}
