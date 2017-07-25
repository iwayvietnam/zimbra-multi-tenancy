<?php

namespace Drupal\zmt;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityListBuilder;
use Drupal\Core\Link;

/**
 * Defines a class to build a listing of Alias entities.
 *
 * @ingroup zmt
 */
class ZmtAliasListBuilder extends EntityListBuilder {


  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['id'] = $this->t('Alias ID');
    $header['name'] = $this->t('Name');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    /* @var $entity \Drupal\zmt\Entity\ZmtAlias */
    $row['id'] = $entity->id();
    $row['name'] = Link::createFromRoute(
      $entity->label(),
      'entity.zmt_alias.edit_form',
      ['zmt_alias' => $entity->id()]
    );
    return $row + parent::buildRow($entity);
  }

}
