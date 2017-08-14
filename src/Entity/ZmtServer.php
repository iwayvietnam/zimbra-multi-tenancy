<?php

namespace Drupal\zmt\Entity;

use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\user\UserInterface;

/**
 * Defines the Zimbra Server entity.
 *
 * @ingroup zmt
 *
 * @ContentEntityType(
 *   id = "zmt_server",
 *   label = @Translation("Zimbra Server"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\zmt\ZmtServerListBuilder",
 *
 *     "form" = {
 *       "default" = "Drupal\zmt\Form\ZmtServerForm",
 *       "add" = "Drupal\zmt\Form\ZmtServerForm",
 *       "edit" = "Drupal\zmt\Form\ZmtServerForm",
 *       "delete" = "Drupal\zmt\Form\ZmtServerDeleteForm"
 *     },
 *     "access" = "Drupal\zmt\ZmtEntityAccessControlHandler"
 *   },
 *   base_table = "zmt_server",
 *   fieldable = FALSE,
 *   translatable = TRUE,
 *   admin_permission = "administer zmt_server",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "name",
 *     "uuid" = "uuid",
 *     "uid" = "user_id",
 *     "langcode" = "langcode"
 *   },
 *   links = {
 *     "canonical" = "/zmt/server/{zmt_server}",
 *     "add-form" = "/zmt/server/add",
 *     "edit-form" = "/zmt/server/{zmt_server}/edit",
 *     "delete-form" = "/zmt/server/{zmt_server}/delete",
 *     "collection" = "/zmt/server"
 *   }
 * )
 */
class ZmtServer extends ContentEntityBase implements ZmtServerInterface {

  use EntityChangedTrait;

  /**
   * {@inheritdoc}
   */
  public static function preCreate(EntityStorageInterface $storage_controller, array &$values) {
    parent::preCreate($storage_controller, $values);
    $values += [
      'user_id' => \Drupal::currentUser()->id(),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function getName() {
    return $this->get('name')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setName($name) {
    $this->set('name', $name);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getCreatedTime() {
    return $this->get('created')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setCreatedTime($timestamp) {
    $this->set('created', $timestamp);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getOwner() {
    return $this->get('user_id')->entity;
  }

  /**
   * {@inheritdoc}
   */
  public function getOwnerId() {
    return $this->get('user_id')->target_id;
  }

  /**
   * {@inheritdoc}
   */
  public function setOwnerId($uid) {
    $this->set('user_id', $uid);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function setOwner(UserInterface $account) {
    $this->set('user_id', $account->id());
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $fields = parent::baseFieldDefinitions($entity_type);

    $fields['name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Name'))
      ->setDescription(t('The name of the Zimbra Server.'))
      ->setRequired(TRUE)
      ->setSettings([
        'max_length' => 255,
        'text_processing' => 0,
      ])
      ->setDefaultValue('')
      ->setDisplayOptions('view', [
        'label' => 'above',
        'type' => 'string',
      ])
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['service_location'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Service Location'))
      ->setDescription(t('The service location of the Zimbra Server.'))
      ->setRequired(TRUE)
      ->setSettings([
        'max_length' => 255,
        'text_processing' => 0,
      ])
      ->setDefaultValue('')
      ->setDisplayOptions('view', [
        'label' => 'above',
        'type' => 'string',
      ])
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['admin_user'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Admin User'))
      ->setDescription(t('The admin user of the Zimbra Server.'))
      ->setRequired(TRUE)
      ->setSettings([
        'max_length' => 255,
        'text_processing' => 0,
      ])
      ->setDefaultValue('')
      ->setDisplayOptions('view', [
        'label' => 'above',
        'type' => 'string',
      ])
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['admin_password'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Admin Password'))
      ->setDescription(t('The admin password of the Zimbra Server.'))
      ->setSettings([
        'max_length' => 255,
        'text_processing' => 0,
      ])
      ->setDefaultValue('')
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
      ])
      ->setDisplayConfigurable('form', TRUE);

    $fields['exclude_mailbox'] = BaseFieldDefinition::create('string_long')
      ->setLabel(t('Exclude Mailbox'))
      ->setDescription(t('The exclude mailbox of the server.'))
      ->setSettings([
        'type' => 'text',
      ])
      ->setDefaultValue('')
      ->setDisplayOptions('form', [
        'type' => 'string_textarea',
      ])
      ->setDisplayConfigurable('form', TRUE);

    $fields['delete_domain'] = BaseFieldDefinition::create('boolean')
      ->setLabel(t('Delete Domain'))
      ->setDescription(t('Allow delete domain on zimbra server.'))
      ->setDisplayOptions('form', [
        'type' => 'boolean_checkbox',
      ])
      ->setDisplayConfigurable('form', TRUE);

    $fields['delete_dl'] = BaseFieldDefinition::create('boolean')
      ->setLabel(t('Delete DL'))
      ->setDescription(t('Allow delete dl on zimbra server.'))
      ->setDisplayOptions('form', [
        'type' => 'boolean_checkbox',
      ])
      ->setDisplayConfigurable('form', TRUE);

    $fields['delete_account'] = BaseFieldDefinition::create('boolean')
      ->setLabel(t('Delete Account'))
      ->setDescription(t('Allow delete account on zimbra server.'))
      ->setDisplayOptions('form', [
        'type' => 'boolean_checkbox',
      ])
      ->setDisplayConfigurable('form', TRUE);

    $fields['delete_alias'] = BaseFieldDefinition::create('boolean')
      ->setLabel(t('Delete Alias'))
      ->setDescription(t('Allow delete alias on zimbra server.'))
      ->setDisplayOptions('form', [
        'type' => 'boolean_checkbox',
      ])
      ->setDisplayConfigurable('form', TRUE);

    $fields['user_id'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Authored by'))
      ->setDescription(t('The user ID of author of the Zimbra Server.'))
      ->setRevisionable(TRUE)
      ->setSetting('target_type', 'user')
      ->setSetting('handler', 'default')
      ->setTranslatable(TRUE);

    $fields['auth_token'] = BaseFieldDefinition::create('string_long')
      ->setLabel(t('Auth Token'))
      ->setDescription(t('The auth token of the server.'))
      ->setSettings([
        'type' => 'text',
      ]);

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Created'))
      ->setDescription(t('The Unix timestamp when the server was created.'));

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The Unix timestamp when the server was most recently saved.'));

    return $fields;
  }

}
