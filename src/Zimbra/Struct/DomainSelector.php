<?php

namespace Drupal\zmt\Zimbra\Struct;

use Drupal\zmt\Zimbra\SoapStruct;

/**
 * DomainSelector struct class.
 */
class DomainSelector extends SoapStruct {

  /**
   * Domain by enums
   *
   * @var array
   */
  private static $_byEnums = array(
    'foreignName',
    'id',
    'krb5Realm',
    'name',
    'virtualHostname',
  );

  /**
   * Constructor method for CosSelector
   * @param  string $by
   * @param  string $value
   * @return self
   */
  public function __construct($by = 'name', $value = NULL) {
    parent::__construct(trim($value));
    $this->by = in_array($by, self::$_byEnums) ? $by : 'name';
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = 'domain') {
    if (!in_array($this->by, self::$_byEnums)) {
      $this->by = 'name';
    }
    return parent::toArray($name);
  }
}
