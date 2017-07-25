<?php

namespace Drupal\zmt;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Class of Service entity.
 *
 * @see \Drupal\zmt\Entity\ZmtCOS.
 */
class ZmtCoSAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\zmt\Entity\ZmtCOSInterface $entity */
    switch ($operation) {
      case 'view':
        if (!$entity->isPublished()) {
          return AccessResult::allowedIfHasPermission($account, 'view unpublished class of service entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view published class of service entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit class of service entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete class of service entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add class of service entities');
  }

}
