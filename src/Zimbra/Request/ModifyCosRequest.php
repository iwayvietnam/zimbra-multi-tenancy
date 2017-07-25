<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * ModifyCosRequest request class
 * Modify Class of Service (COS) attributes
 */
class ModifyCosRequest extends SoapRequest {

  /**
   * Constructor method for ModifyCosRequest
   * @param string $id Zimbra ID
   * @param array  $attrs
   * @return self
   */
  public function __construct($id = NULL, array $attrs = []) {
    parent::__construct();
    $this->id = [['_content' => trim($id)]];
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}
