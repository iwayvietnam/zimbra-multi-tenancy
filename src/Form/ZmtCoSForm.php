<?php

namespace Drupal\zmt\Form;

use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Form controller for Class of Service edit forms.
 *
 * @ingroup zmt
 */
class ZmtCoSForm extends ContentEntityForm {

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    /* @var $entity \Drupal\zmt\Entity\ZmtCoS */
    $form = parent::buildForm($form, $form_state);

    $entity = $this->entity;

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $entity = &$this->entity;

    $status = parent::save($form, $form_state);

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created the %label Class of Service.', [
          '%label' => $entity->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved the %label Class of Service.', [
          '%label' => $entity->label(),
        ]));
    }
    $form_state->setRedirect('entity.zmt_cos.canonical', ['zmt_cos' => $entity->id()]);
  }

}
