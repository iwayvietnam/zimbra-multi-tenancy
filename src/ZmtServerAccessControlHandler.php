<?php

namespace Drupal\zmt;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Zimbra Server entity.
 *
 * @see \Drupal\zmt\Entity\ZmtServer.
 */
class ZmtServerAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\zmt\Entity\ZmtServerInterface $entity */
    switch ($operation) {
      case 'view':
        if (!$entity->isPublished()) {
          return AccessResult::allowedIfHasPermission($account, 'view unpublished zimbra server entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view published zimbra server entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit zimbra server entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete zimbra server entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add zimbra server entities');
  }

}
