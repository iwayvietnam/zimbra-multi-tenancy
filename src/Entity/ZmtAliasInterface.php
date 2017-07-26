<?php

namespace Drupal\zmt\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Alias entities.
 *
 * @ingroup zmt
 */
interface ZmtAliasInterface extends  ContentEntityInterface, EntityChangedInterface, EntityOwnerInterface {

  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Alias name.
   *
   * @return string
   *   Name of the Alias.
   */
  public function getName();

  /**
   * Sets the Alias name.
   *
   * @param string $name
   *   The Alias name.
   *
   * @return \Drupal\zmt\Entity\ZmtAliasInterface
   *   The called Alias entity.
   */
  public function setName($name);

  /**
   * Gets the Alias creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Alias.
   */
  public function getCreatedTime();

  /**
   * Sets the Alias creation timestamp.
   *
   * @param int $timestamp
   *   The Alias creation timestamp.
   *
   * @return \Drupal\zmt\Entity\ZmtAliasInterface
   *   The called Alias entity.
   */
  public function setCreatedTime($timestamp);

}
