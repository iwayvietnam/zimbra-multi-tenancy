<?php

namespace Drupal\zmt\Zimbra\Struct;

use Drupal\zmt\Zimbra\SoapStruct;

/**
 * KeyValuePair struct class.
 */
class KeyValuePair extends SoapStruct {

  /**
   * Constructor method for KeyValuePair
   * @param string $key
   * @param string $value
   * @return self
   */
  public function __construct($key, $value = NULL){
    parent::__construct($value);
    $this->n = $key;
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = 'a') {
    return parent::toArray($name);
  }
}
