<?php

namespace Drupal\zmt\Zimbra\Struct;

use Drupal\zmt\Zimbra\SoapStruct;

/**
 * ServerSelector struct class.
 */
class ServerSelector extends SoapStruct {

  /**
   * Server by enums
   *
   * @var array
   */
  private static $_byEnums = array(
    'id',
    'name',
    'serviceHostname',
  );

  /**
   * Constructor method for ServerSelector
   * @param  string $by
   *   Selects the meaning of {server-key}
   * @param  string $value
   *   Key for choosing server
   * @return self
   */
  public function __construct($by, $value = NULL) {
    parent::__construct(trim($value));
    $this->by = in_array($by, self::$_byEnums) ? $by : 'name';
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = 'server') {
    if (!in_array($this->by, self::$_byEnums)) {
      $this->by = 'name';
    }
    return parent::toArray($name);
  }
}
