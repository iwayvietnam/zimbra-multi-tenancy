<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * DeleteCosRequest request class
 * Delete a Class of Service (COS)
 */
class DeleteCosRequest extends SoapRequest {

  /**
   * Constructor method for DeleteCosRequest
   * @param string $id Zimbra ID
   * @return self
   */
  public function __construct($id) {
    parent::__construct();
    $this->id = trim($id);
  }
}
