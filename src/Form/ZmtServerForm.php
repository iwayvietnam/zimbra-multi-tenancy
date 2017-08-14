<?php

namespace Drupal\zmt\Form;

use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Form controller for Zimbra Server edit forms.
 *
 * @ingroup zmt
 */
class ZmtServerForm extends ContentEntityForm {

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    /* @var $entity \Drupal\zmt\Entity\ZmtAccount */
    $form = parent::buildForm($form, $form_state);

    $form['actions']['submit']['#suffix'] = Link::fromTextAndUrl(t('Cancel'), Url::fromRoute('entity.zmt_server.collection'))->toString();

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    parent::save($form, $form_state);
    $form_state->setRedirect('entity.zmt_server.collection');
  }

}
