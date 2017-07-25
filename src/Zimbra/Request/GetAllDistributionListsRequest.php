<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;
use Drupal\zmt\Zimbra\Struct\DomainSelector;

/**
 * GetAllDistributionListsRequest request class
 * Get all distribution lists that match the selection criteria
 */
class GetAllDistributionListsRequest extends SoapRequest {

  /**
   * Constructor method for GetAllDistributionListsRequest
   * @param  DomainSelector $domain
   * @return self
   */
  public function __construct(DomainSelector $domain = NULL) {
    parent::__construct();
    if ($domain instanceof DomainSelector) {
        $this->domain = $domain;
    }
  }
}
