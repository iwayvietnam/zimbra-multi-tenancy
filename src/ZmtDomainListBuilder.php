<?php

namespace Drupal\zmt;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Link;

/**
 * Defines a class to build a listing of Domain entities.
 *
 * @ingroup zmt
 */
class ZmtDomainListBuilder extends EntityListBuilder {


  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['id'] = $this->t('Domain ID');
    $header['name'] = $this->t('Name');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var $entity \Drupal\zmt\Entity\ZmtDomain */
    $row['id'] = $entity->id();
    $row['name'] = Link::createFromRoute(
      $entity->label(),
      'entity.zmt_domain.edit_form',
      ['zmt_domain' => $entity->id()]
    );
    return $row + parent::buildRow($entity);
  }

}
