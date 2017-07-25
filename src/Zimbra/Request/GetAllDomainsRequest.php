<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * GetAllDomainsRequest request class
 * Get all domains
 */
class GetAllDomainsRequest extends SoapRequest {

  /**
   * Constructor method for GetAllDomainsRequest
   * @param  bool $applyConfig
   *   Apply config flag
   * @return self
   */
  public function __construct($applyConfig = NULL) {
    parent::__construct();
    if (NULL !== $applyConfig) {
      $this->applyConfig = (bool) $applyConfig;
    }
  }
}
