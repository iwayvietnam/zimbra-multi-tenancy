<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * DeleteDomainRequest request class
 * Delete a domain
 */
class DeleteDomainRequest extends SoapRequest {

  /**
   * Constructor method for DeleteDomainRequest
   * @param string $id Zimbra ID
   * @return self
   */
  public function __construct($id) {
    parent::__construct();
    $this->id = trim($id);
  }
}
