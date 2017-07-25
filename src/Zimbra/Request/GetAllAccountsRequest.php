<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;
use Drupal\zmt\Zimbra\Struct\ServerSelector;
use Drupal\zmt\Zimbra\Struct\DomainSelector;

/**
 * GetAllAccountsRequest request class
 * Get All servers matching the selectin criteria
 */
class GetAllAccountsRequest extends SoapRequest {

  /**
   * Constructor method for GetAllAccountsRequest
   * @param  ServerSelector $server
   * @param  DomainSelector $domain
   * @return self
   */
  public function __construct(ServerSelector $server = NULL, DomainSelector $domain = NULL) {
    parent::__construct();
    if ($server instanceof ServerSelector) {
      $this->server = $server;
    }
    if ($domain instanceof DomainSelector) {
      $this->domain = $domain;
    }
  }
}
