<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;
use Drupal\zmt\Zimbra\Struct\CosSelector;

/**
 * GetCosRequest request class
 * Get Class Of Service (COS)
 */
class GetCosRequest extends SoapRequest {

  /**
   * Constructor method for GetCosRequest
   * @param  Cos $cos
   *   Specify Class Of Service (COS)
   * @param  string $attrs
   *   Comma separated list of attributes
   * @return self
   */
  public function __construct(CosSelector $cos = NULL, $attrs = NULL) {
    parent::__construct();
    if ($cos instanceof CosSelector) {
        $this->cos = $cos;
    }
    if (NULL !== $attrs) {
        $this->attrs = trim($attrs);
    }
  }
}
