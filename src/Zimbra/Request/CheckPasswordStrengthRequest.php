<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * CheckPasswordStrengthRequest request class
 * Check password strength
 */
class CheckPasswordStrengthRequest extends SoapRequest {
  
  /**
   * Constructor method for CountAccountRequest
   * @param string $id Zimbra ID
   * @param string $password Passowrd to check
   * @return self
   */
  public function __construct($id, $password) {
    parent::__construct();
    $this->id = trim($id);
    $this->password = trim($password);
  }
}
