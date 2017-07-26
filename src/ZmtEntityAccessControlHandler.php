<?php

namespace Drupal\zmt;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;
use Drupal\user\EntityOwnerInterface;

/**
 * Access controller for the Zmt entities.
 */
class ZmtEntityAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    $result = AccessResult::allowedIfHasPermissions($account, [
      'administer zmt',
      'administer any ' . $entity->getEntityTypeId(),
    ], 'OR');
    if ($result->isAllowed()) {
      return $result;
    }
    else {
      if ($entity instanceof EntityOwnerInterface) {
        if ($entity->getOwnerId() == $account->id()) {
          return AccessResult::allowedIfHasPermission($account, 'administer own ' . $entity->getEntityTypeId());
        }
      }
      return AccessResult::neutral();
    }
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermissions($account, [
      'administer zmt',
      'administer any ' . $this->entityTypeId,
      'administer own ' . $this->entityTypeId,
    ], 'OR');
  }

}
