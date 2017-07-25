<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * ModifyDistributionListRequest request class
 * Modify attributes for a Distribution List
 */
class ModifyDistributionListRequest extends SoapRequest {

  /**
   * Constructor method for ModifyDistributionListRequet
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
