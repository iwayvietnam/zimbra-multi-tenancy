<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * DeleteDistributionListRequest request class
 * Delete a distribution list
 */
class DeleteDistributionListRequest extends SoapRequest {

  /**
   * Constructor method for DeleteDistributionListRequest
   * @param string $id Zimbra ID
   * @return self
   */
  public function __construct($id) {
    parent::__construct();
    $this->id = trim($id);
  }
}
