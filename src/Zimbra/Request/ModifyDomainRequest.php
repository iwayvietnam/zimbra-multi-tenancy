<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * ModifyDomainRequest request class
 * Modify attributes for a domain
 */
class ModifyDomainRequest extends SoapRequest {

  /**
   * Constructor method for ModifyDomainRequest
   * @param string $id Zimbra ID
   * @param array  $attrs
   * @return self
   */
  public function __construct($id, array $attrs = []) {
    parent::__construct();
    $this->id = trim($id);
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}
