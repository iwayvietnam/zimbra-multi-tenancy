<?php

namespace Drupal\zmt\Entity;

use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Entity\EntityChangedInterface;
use Drupal\user\EntityOwnerInterface;

/**
 * Provides an interface for defining Domain entities.
 *
 * @ingroup zmt
 */
interface ZmtDomainInterface extends  ContentEntityInterface, EntityChangedInterface, EntityOwnerInterface {

  // Add get/set methods for your configuration properties here.

  /**
   * Gets the Domain name.
   *
   * @return string
   *   Name of the Domain.
   */
  public function getName();

  /**
   * Sets the Domain name.
   *
   * @param string $name
   *   The Domain name.
   *
   * @return \Drupal\zmt\Entity\ZmtDomainInterface
   *   The called Domain entity.
   */
  public function setName($name);

  /**
   * Gets the Domain creation timestamp.
   *
   * @return int
   *   Creation timestamp of the Domain.
   */
  public function getCreatedTime();

  /**
   * Sets the Domain creation timestamp.
   *
   * @param int $timestamp
   *   The Domain creation timestamp.
   *
   * @return \Drupal\zmt\Entity\ZmtDomainInterface
   *   The called Domain entity.
   */
  public function setCreatedTime($timestamp);

}
