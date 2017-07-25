<?php

namespace Drupal\zmt\Zimbra;

/**
 * Interface for soap struct.
 */
interface SoapStructInterface extends \ArrayAccess {

  /**
   * Sets value
   *
   * @param  string $value
   * @return self
   */
  function setValue($value);

  /**
   * Gets value
   *
   * @return string
   */
  function getValue();

  /**
   * Sets namespace
   *
   * @param  string $value
   * @return self
   */
  function setNamespace($namespace);

  /**
   * Gets namespace
   *
   * @return string
   */
  function getNamespace() ;

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  function toArray($name = NULL);
}
