<?php

/**
 * Defines zmt alias entity class.
 */
class ZmtAliasEntity extends Entity {

  /**
   * The alias ID.
   *
   * @var integer
   */
  public $id;

  /**
   * The {zmt_domain}.id of the alias.
   *
   * @var integer
   */
  public $domain_id = 0;

  /**
   * The name of the alias.
   *
   * @var string
   */
  public $name;

  /**
   * The target account of the alias.
   *
   * @var integer
   */
  public $account_id = 0;

  /**
   * The zimbra target id of the alias.
   *
   * @var string
   */
  public $zimbra_target_id;

  /**
   * The {users}.uid that created the alias.
   *
   * @var integer
   */
  public $uid = 0;

  /**
   * The Unix timestamp when the alias was created..
   *
   * @var integer
   */
  public $created = 0;

  /**
   * The Unix timestamp when the alias was most recently saved.
   *
   * @var integer
   */
  public $changed = 0;

  /**
   * Create a new alias instance.
   */
  function __construct(array $values = array()) {
    parent::__construct($values);

    // Set the created time to now.
    if (empty($this->created)) {
      $this->created = REQUEST_TIME;
    }
  }

  /**
   * Implements EntityInterface::id().
   */
  function id() {
    return !empty($this->id) ? $this->id : NULL;
  }

  /**
   * Implements EntityInterface::entityType().
   */
  public function entityType() {
    return 'zmt_alias';
  }

  /**
   * Implements EntityInterface::label().
   */
  public function label() {
    return $this->name;
  }

  /**
   * Implements EntityInterface::uri().
   */
  function uri() {
    return array(
      'path' => 'zmt/alias/' . $this->id,
    );
  }
}
