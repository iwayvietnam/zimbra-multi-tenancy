<?php

/**
 * Defines zmt tenant entity class.
 */
class ZmtTenantEntity extends Entity {

  /**
   * The tenant ID.
   *
   * @var integer
   */
  public $id;

  /**
   * The full name of the tenant.
   *
   * @var string
   */
  public $full_name;

  /**
   * The {users}.uid representation of the tenant.
   *
   * @var string
   */
  public $tenant_uid = 0;

  /**
   * The {zmt_server}.id of zimbra server.
   *
   * @var integer
   */
  public $server_id = 0;

  /**
   * The {users}.uid that created the tenant.
   *
   * @var integer
   */
  public $uid = 0;

  /**
   * The Unix timestamp when the tenant was created..
   *
   * @var integer
   */
  public $created = 0;

  /**
   * The Unix timestamp when the tenant was most recently saved.
   *
   * @var integer
   */
  public $changed = 0;

  /**
   * Create a new tenant instance.
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
    return 'zmt_tenant';
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
      'path' => 'zmt/tenant/' . $this->id,
    );
  }
}

/**
 * Defines zmt tenant cos mapping entity class.
 */
class ZmtTenantCosEntity extends Entity {

  /**
   * The ID of tenant cos mapping.
   *
   * @var integer
   */
  public $id;

  /**
   * The {zmt_tenant}.id of tenant cos mapping.
   *
   * @var integer
   */
  public $tenant_id;

  /**
   * The {zmt_cos}.id of tenant cos mapping.
   *
   * @var integer
   */
  public $cos_id;

  /**
   * The max account number of tenant cos mapping.
   *
   * @var integer
   */
  public $max_account;

  /**
   * The {users}.uid that created the tenant cos mapping.
   *
   * @var integer
   */
  public $uid = 0;

  /**
   * The Unix timestamp when the tenant cos mapping was created..
   *
   * @var integer
   */
  public $created = 0;

  /**
   * Create a new tenant instance.
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
    return 'zmt_tenant_cos';
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
      'path' => 'zmt/tenant/' . $this->tenant_id,
    );
  }
}
