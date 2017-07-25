<?php

namespace Drupal\zmt\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Account entities.
 *
 * @ingroup zmt
 */
interface ZmtAccountInterface extends  ContentEntityInterface, EntityChangedInterface, EntityOwnerInterface {

  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Account name.
   *
   * @return string
   *   Name of the Account.
   */
  public function getName();

  /**
   * Sets the Account name.
   *
   * @param string $name
   *   The Account name.
   *
   * @return \Drupal\zmt\Entity\ZmtAccountInterface
   *   The called Account entity.
   */
  public function setName($name);

  /**
   * Gets the Account creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Account.
   */
  public function getCreatedTime();

  /**
   * Sets the Account creation timestamp.
   *
   * @param int $timestamp
   *   The Account creation timestamp.
   *
   * @return \Drupal\zmt\Entity\ZmtAccountInterface
   *   The called Account entity.
   */
  public function setCreatedTime($timestamp);

  /**
   * Returns the Account published status indicator.
   *
   * Unpublished Account are only visible to restricted users.
   *
   * @return bool
   *   TRUE if the Account is published.
   */
  public function isPublished();

  /**
   * Sets the published status of a Account.
   *
   * @param bool $published
   *   TRUE to set this Account to published, FALSE to set it to unpublished.
   *
   * @return \Drupal\zmt\Entity\ZmtAccountInterface
   *   The called Account entity.
   */
  public function setPublished($published);

}
