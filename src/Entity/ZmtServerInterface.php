<?php

namespace Drupal\zmt\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Zimbra Server entities.
 *
 * @ingroup zmt
 */
interface ZmtServerInterface extends ContentEntityInterface, EntityChangedInterface, EntityOwnerInterface {

  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Zimbra Server name.
   *
   * @return string
   *   Name of the Zimbra Server.
   */
  public function getName();

  /**
   * Sets the Zimbra Server name.
   *
   * @param string $name
   *   The Zimbra Server name.
   *
   * @return \Drupal\zmt\Entity\ZmtServerInterface
   *   The called Zimbra Server entity.
   */
  public function setName($name);

  /**
   * Gets the Zimbra Server creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Zimbra Server.
   */
  public function getCreatedTime();

  /**
   * Sets the Zimbra Server creation timestamp.
   *
   * @param int $timestamp
   *   The Zimbra Server creation timestamp.
   *
   * @return \Drupal\zmt\Entity\ZmtServerInterface
   *   The called Zimbra Server entity.
   */
  public function setCreatedTime($timestamp);

}
