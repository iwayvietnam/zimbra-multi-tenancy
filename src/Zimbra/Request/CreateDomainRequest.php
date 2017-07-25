<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * CreateDomainRequest request class
 * Create domain
 */
class CreateDomainRequest extends SoapRequest {

  /**
   * Constructor method for CreateDomainRequest
   * @param string $name The name
   * @param array  $attrs
   * @return self
   */
  public function __construct($name, array $attrs = []) {
    parent::__construct();
    $this->name = trim($name);
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}
