<?php

/**
 * Defines zmt server entity class.
 */
class ZmtServerEntity extends Entity {

  /**
   * The server ID.
   *
   * @var integer
   */
  public $id;

  /**
   * The server name (bundle).
   *
   * @var string
   */
  public $name;

  /**
   * The service location of the server.
   *
   * @var string
   */
  public $service_location;

  /**
   * The admin user of the server.
   *
   * @var string
   */
  public $admin_user;

  /**
   * The admin password of the server.
   *
   * @var string
   */
  public $admin_password;

  /**
   * The exclude mailbox of the server.
   *
   * @var string
   */
  public $exclude_mailbox;

  /**
   * Allow delete domain on zimbra server.
   *
   * @var integer
   */
  public $delete_domain = 0;

  /**
   * Allow delete dl on zimbra server.
   *
   * @var integer
   */
  public $delete_dl = 0;

  /**
   * Allow delete account on zimbra server.
   *
   * @var integer
   */
  public $delete_account = 0;

  /**
   * Allow delete alias on zimbra server.
   *
   * @var integer
   */
  public $delete_alias = 0;

  /**
   * The {users}.uid that created the server.
   *
   * @var integer
   */
  public $uid = 0;

  /**
   * The Unix timestamp when the server was created..
   *
   * @var integer
   */
  public $created = 0;

  /**
   * The Unix timestamp when the server was most recently saved.
   *
   * @var integer
   */
  public $changed = 0;

  /**
   * Create a new server instance.
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
    return 'zmt_server';
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
      'path' => 'zmt/server/' . $this->id,
    );
  }
}
