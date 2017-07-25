<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;
use Drupal\zmt\Zimbra\Struct\DomainSelector;

/**
 * GetDomainRequest request class
 * Get information about a domain
 */
class GetDomainRequest extends SoapRequest {

  /**
   * Constructor method for GetDomainRequest
   * @param  DomainSelector $domain
   *   Specify the domain.
   * @param  bool $applyConfig
   *   Apply config flag
   * @param  string $attrs
   *   Comma separated list of attributes
   * @return self
   */
  public function __construct(DomainSelector $domain = NULL, $applyConfig = NULL, $attrs = NULL) {
    parent::__construct();
    if ($domain instanceof DomainSelector) {
      $this->domain = $domain;
    }
    if (NULL !== $applyConfig) {
      $this->applyConfig = (bool) $applyConfig;
    }
    if (NULL !== $attrs) {
      $this->attrs = trim($attrs);
    }
  }
}
