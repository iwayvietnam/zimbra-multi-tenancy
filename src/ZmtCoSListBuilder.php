<?php

namespace Drupal\zmt;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Link;

/**
 * Defines a class to build a listing of Class of Service entities.
 *
 * @ingroup zmt
 */
class ZmtCoSListBuilder extends EntityListBuilder {


  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['id'] = $this->t('Class of Service ID');
    $header['name'] = $this->t('Name');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var $entity \Drupal\zmt\Entity\ZmtCOS */
    $row['id'] = $entity->id();
    $row['name'] = Link::createFromRoute(
      $entity->label(),
      'entity.zmt_cos.edit_form',
      ['zmt_cos' => $entity->id()]
    );
    return $row + parent::buildRow($entity);
  }

}
