<?php

/**
 * Defines zmt cos entity class.
 */
class ZmtCosEntity extends Entity {

  /**
   * The cos ID.
   *
   * @var integer
   */
  public $id;

  /**
   * The {zmt_server}.id of zimbra server.
   *
   * @var integer
   */
  public $server_id;

  /**
   * The cos name (bundle).
   *
   * @var string
   */
  public $name;

  /**
   * The description of the cos.
   *
   * @var string
   */
  public $description;

  /**
   * The mail quota of the cos.
   *
   * @var integer
   */
  public $mail_quota = 0;

  /**
   * The default max account number of the cos.
   *
   * @var integer
   */
  public $max_account = 0;

  /**
   * The zimbra identify for class of service.
   *
   * @var string
   */
  public $zimbra_id = 0;

  /**
   * The {users}.uid that created the cos.
   *
   * @var integer
   */
  public $uid = 0;

  /**
   * The Unix timestamp when the cos was created..
   *
   * @var integer
   */
  public $created = 0;

  /**
   * The Unix timestamp when the cos was most recently saved.
   *
   * @var integer
   */
  public $changed = 0;

  /**
   * Create a new cos instance.
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
    return 'zmt_cos';
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
      'path' => 'zmt/cos/' . $this->id,
    );
  }
}
