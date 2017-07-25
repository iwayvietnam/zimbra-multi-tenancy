<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * CreateAccountRequest request class
 * Create account
 */
class CreateAccountRequest extends SoapRequest {

  /**
   * Constructor method for CreateAccountRequest
   * @param string $name
   *   New account's name. Must include domain (uid@name), and domain specified in name must exist
   * @param string $password
   *   New account's password
   * @param array  $attrs
   * @return self
   */
  public function __construct($name, $password, array $attrs = []) {
    parent::__construct();
    $this->name = trim($name);
    $this->password = trim($password);
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}
