<?php

namespace Drupal\zmt\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Group entities.
 *
 * @ingroup zmt
 */
interface ZmtGroupInterface extends  ContentEntityInterface, EntityChangedInterface, EntityOwnerInterface {

  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Group name.
   *
   * @return string
   *   Name of the Group.
   */
  public function getName();

  /**
   * Sets the Group name.
   *
   * @param string $name
   *   The Group name.
   *
   * @return \Drupal\zmt\Entity\ZmtGroupInterface
   *   The called Group entity.
   */
  public function setName($name);

  /**
   * Gets the Group creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Group.
   */
  public function getCreatedTime();

  /**
   * Sets the Group creation timestamp.
   *
   * @param int $timestamp
   *   The Group creation timestamp.
   *
   * @return \Drupal\zmt\Entity\ZmtGroupInterface
   *   The called Group entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns the Group published status indicator.
   *
   * Unpublished Group are only visible to restricted users.
   *
   * @return bool
   *   TRUE if the Group is published.
   */
  public function isPublished();

  /**
   * Sets the published status of a Group.
   *
   * @param bool $published
   *   TRUE to set this Group to published, FALSE to set it to unpublished.
   *
   * @return \Drupal\zmt\Entity\ZmtGroupInterface
   *   The called Group entity.
   */
  public function setPublished($published);

}
