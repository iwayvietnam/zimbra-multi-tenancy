<?php

namespace Drupal\zmt;

use Drupal\Core\Entity\EntityAccessControlHandler;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Access\AccessResult;

/**
 * Access controller for the Domain entity.
 *
 * @see \Drupal\zmt\Entity\ZmtDomain.
 */
class ZmtDomainAccessControlHandler extends EntityAccessControlHandler {

  /**
   * {@inheritdoc}
   */
  protected function checkAccess(EntityInterface $entity, $operation, AccountInterface $account) {
    /** @var \Drupal\zmt\Entity\ZmtDomainInterface $entity */
    switch ($operation) {
      case 'view':
        if (!$entity->isPublished()) {
          return AccessResult::allowedIfHasPermission($account, 'view unpublished domain entities');
        }
        return AccessResult::allowedIfHasPermission($account, 'view published domain entities');

      case 'update':
        return AccessResult::allowedIfHasPermission($account, 'edit domain entities');

      case 'delete':
        return AccessResult::allowedIfHasPermission($account, 'delete domain entities');
    }

    // Unknown operation, no opinion.
    return AccessResult::neutral();
  }

  /**
   * {@inheritdoc}
   */
  protected function checkCreateAccess(AccountInterface $account, array $context, $entity_bundle = NULL) {
    return AccessResult::allowedIfHasPermission($account, 'add domain entities');
  }

}
