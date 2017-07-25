<?php

namespace Drupal\zmt;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Group entity.
 *
 * @see \Drupal\zmt\Entity\ZmtGroup.
 */
class ZmtGroupAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\zmt\Entity\ZmtGroupInterface $entity */
    switch ($operation) {
      case 'view':
        if (!$entity->isPublished()) {
          return AccessResult::allowedIfHasPermission($account, 'view unpublished group entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view published group entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit group entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete group entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add group entities');
  }

}
