<?php

namespace Drupal\zmt\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Class of Service entities.
 *
 * @ingroup zmt
 */
interface ZmtCoSInterface extends  ContentEntityInterface, EntityChangedInterface, EntityOwnerInterface {

  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Class of Service name.
   *
   * @return string
   *   Name of the Class of Service.
   */
  public function getName();

  /**
   * Sets the Class of Service name.
   *
   * @param string $name
   *   The Class of Service name.
   *
   * @return \Drupal\zmt\Entity\ZmtCoSInterface
   *   The called Class of Service entity.
   */
  public function setName($name);

  /**
   * Gets the Class of Service creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Class of Service.
   */
  public function getCreatedTime();

  /**
   * Sets the Class of Service creation timestamp.
   *
   * @param int $timestamp
   *   The Class of Service creation timestamp.
   *
   * @return \Drupal\zmt\Entity\ZmtCoSInterface
   *   The called Class of Service entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns the Class of Service published status indicator.
   *
   * Unpublished Class of Service are only visible to restricted users.
   *
   * @return bool
   *   TRUE if the Class of Service is published.
   */
  public function isPublished();

  /**
   * Sets the published status of a Class of Service.
   *
   * @param bool $published
   *   TRUE to set this Class of Service to published, FALSE to set it to unpublished.
   *
   * @return \Drupal\zmt\Entity\ZmtCoSInterface
   *   The called Class of Service entity.
   */
  public function setPublished($published);

}
