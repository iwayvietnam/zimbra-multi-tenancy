<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * CreateCosRequest request class
 * Create class of service
 */
class CreateCosRequest extends SoapRequest {

  /**
   * Constructor method for CreateCosRequest
   * @param string $name The name
   * @param array  $attrs
   * @return self
   */
  public function __construct($name, array $attrs = []) {
    parent::__construct();
    $this->name = [
      [
        '_content' => trim($name)
      ]
    ];
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}
