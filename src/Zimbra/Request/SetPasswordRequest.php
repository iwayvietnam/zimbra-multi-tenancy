<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * SetPasswordRequest request class
 * Set password
 */
class SetPasswordRequest extends SoapRequest {

  /**
   * Constructor method for SetPasswordRequest
   * @param string $id Zimbra ID
   * @param string $newPassword New Password
   * @return self
   */
  public function __construct($id, $newPassword) {
    parent::__construct();
    $this->id = trim($id);
    $this->newPassword = trim($newPassword);
  }
}
