<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * ModifyAccountRequest request class
 * Modify an account
 */
class ModifyAccountRequest extends SoapRequest {

  /**
   * Constructor method for ModifyAccountRequest
   * @param string $id
   *   Zimbra ID
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
