<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * CreateDistributionListRequest request class
 * Create a distribution list
 */
class CreateDistributionListRequest extends SoapRequest {

  /**
   * Constructor method for CreateDistributionListRequest
   * @param string $name
   *   Name for distribution list
   * @param bool   $dynamic
   *   If 1 (true) then create a dynamic distribution list
   * @param array  $attrs
   * @return self
   */
  public function __construct($name, $dynamic = NULL, array $attrs = []) {
    parent::__construct();
    $this->name = trim($name);
    if (NULL !== $dynamic) {
      $this->dynamic = (bool) $dynamic;
    }
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}
