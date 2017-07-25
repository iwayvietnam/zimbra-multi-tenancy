<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * DeleteAccountRequest request class
 * Deletes the account with the given id
 */
class DeleteAccountRequest extends SoapRequest {

  /**
   * Constructor method for DeleteAccountRequest
   * @param string $id Zimbra ID
   * @return self
   */
  public function __construct($id) {
    parent::__construct();
    $this->id = trim($id);
  }
}
