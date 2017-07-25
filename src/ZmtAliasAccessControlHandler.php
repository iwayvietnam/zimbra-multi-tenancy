<?php

namespace Drupal\zmt;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Alias entity.
 *
 * @see \Drupal\zmt\Entity\ZmtAlias.
 */
class ZmtAliasAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\zmt\Entity\ZmtAliasInterface $entity */
    switch ($operation) {
      case 'view':
        if (!$entity->isPublished()) {
          return AccessResult::allowedIfHasPermission($account, 'view unpublished alias entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view published alias entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit alias entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete alias entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add alias entities');
  }

}
