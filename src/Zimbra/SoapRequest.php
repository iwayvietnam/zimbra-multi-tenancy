<?php

namespace Drupal\zmt\Zimbra;

/**
 * Soap request struct class.
 */
abstract class SoapRequest extends SoapStruct {

  /**
   * SoapRequest constructor
   *
   * @param  string $value
   * @return self
   */
  public function __construct($value = NULL) {
    parent::__construct($value);
    $this->setNamespace('urn:zimbraAdmin');
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = NULL) {
    $name = empty($name) ? $this->className() : $name;
    return parent::toArray($name);
  }
}
