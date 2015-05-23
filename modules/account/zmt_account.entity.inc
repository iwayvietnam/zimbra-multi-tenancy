<?php

/**
 * Defines zmt account entity class.
 */
class ZmtAccountEntity extends Entity {

  /**
   * The account ID.
   *
   * @var integer
   */
  public $id;

  /**
   * The {zmt_domain}.id of zimbra server.
   *
   * @var integer
   */
  public $domain_id = 0;

  /**
   * The {cos_id}.id of zimbra server.
   *
   * @var integer
   */
  public $cos_id = 0;

  /**
   * The name of the account.
   *
   * @var string
   */
  public $name;

  /**
   * The title of the account.
   *
   * @var string
   */
  public $title;

  /**
   * The full name of the account.
   *
   * @var string
   */
  public $full_name;

  /**
   * The status of the account.
   *
   * @var string
   */
  public $status;

  /**
   * The telephone of the account.
   *
   * @var string
   */
  public $telephone;

  /**
   * The mobile of the account.
   *
   * @var string
   */
  public $mobile;

  /**
   * The company of the account.
   *
   * @var string
   */
  public $company;

  /**
   * The zimbra_id of the account.
   *
   * @var string
   */
  public $zimbra_id;

  /**
   * The {users}.uid that created the account.
   *
   * @var integer
   */
  public $uid = 0;

  /**
   * The Unix timestamp when the account was created..
   *
   * @var integer
   */
  public $created = 0;

  /**
   * The Unix timestamp when the account was most recently saved.
   *
   * @var integer
   */
  public $changed = 0;

  /**
   * Create a new account instance.
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
    return 'zmt_account';
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
      'path' => 'zmt/account/' . $this->id,
    );
  }
}
