<?php

/**
 * Defines zmt domain entity class.
 */
class ZmtDomainEntity extends Entity {

  /**
   * The domain ID.
   *
   * @var integer
   */
  public $id;

  /**
   * The {zmt_server}.id of zimbra server.
   *
   * @var integer
   */
  public $server_id = 0;

  /**
   * The {zmt_tenant}.id of zimbra server.
   *
   * @var integer
   */
  public $tenant_id = 0;

  /**
   * The name of the domain.
   *
   * @var string
   */
  public $name;

  /**
   * The status of the domain.
   *
   * @var string
   */
  public $status;

  /**
   * The zimbra identify of the domain.
   *
   * @var string
   */
  public $zimbra_id;

  /**
   * The {users}.uid representation of the domain.
   *
   * @var string
   */
  public $domain_uid = 0;

  /**
   * The {users}.uid that created the domain.
   *
   * @var integer
   */
  public $uid = 0;

  /**
   * The Unix timestamp when the domain was created..
   *
   * @var integer
   */
  public $created = 0;

  /**
   * The Unix timestamp when the domain was most recently saved.
   *
   * @var integer
   */
  public $changed = 0;

  /**
   * Create a new domain instance.
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
    return 'zmt_domain';
  }

  /**
   * Implements EntityInterface::label().
   */
  public function label() {
    return $this->full_name;
  }

  /**
   * Implements EntityInterface::uri().
   */
  function uri() {
    return array(
      'path' => 'zmt/domain/' . $this->id,
    );
  }
}

/**
 * Defines zmt domain cos mapping entity class.
 */
class ZmtDomainCosEntity extends Entity {

  /**
   * The ID of domain cos mapping.
   *
   * @var integer
   */
  public $id;

  /**
   * The {zmt_domain}.id of domain cos mapping.
   *
   * @var integer
   */
  public $domain_id;

  /**
   * The {zmt_cos}.id of domain cos mapping.
   *
   * @var integer
   */
  public $cos_id;

  /**
   * The max account number of domain cos mapping.
   *
   * @var integer
   */
  public $max_account;

  /**
   * The {users}.uid that created the domain cos mapping.
   *
   * @var integer
   */
  public $uid = 0;

  /**
   * The Unix timestamp when the domain cos mapping was created..
   *
   * @var integer
   */
  public $created = 0;

  /**
   * Create a new domain instance.
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
    return 'zmt_domain_cos';
  }

  /**
   * Implements EntityInterface::label().
   */
  public function label() {
    return '';
  }

  /**
   * Implements EntityInterface::uri().
   */
  function uri() {
    return array(
      'path' => 'zmt/domain/' . $this->domain_id,
    );
  }
}
