<?php

/**
 * Defines zmt group entity class.
 */
class ZmtGroupEntity extends Entity {

  /**
   * The group ID.
   *
   * @var integer
   */
  public $id;

  /**
   * The {zmt_domain}.id of the group.
   *
   * @var integer
   */
  public $domain_id = 0;

  /**
   * The name of the group.
   *
   * @var string
   */
  public $name;

  /**
   * The full name of the group.
   *
   * @var string
   */
  public $full_name;

  /**
   * The zimbra identify of the group.
   *
   * @var string
   */
  public $zimbra_id;

  /**
   * The {users}.uid representation of the group.
   *
   * @var integer
   */
  public $group_uid = 0;

  /**
   * The {users}.uid that created the group.
   *
   * @var integer
   */
  public $uid = 0;

  /**
   * The Unix timestamp when the group was created..
   *
   * @var integer
   */
  public $created = 0;

  /**
   * The Unix timestamp when the group was most recently saved.
   *
   * @var integer
   */
  public $changed = 0;

  /**
   * Create a new group instance.
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
    return 'zmt_group';
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
      'path' => 'zmt/group/' . $this->id,
    );
  }
}

/**
 * Defines zmt group member entity class.
 */
class ZmtGroupMemberEntity extends Entity {

  /**
   * Group member is account.
   */
  const GROUP_MEMBER_ACCOUNT = 0;

  /**
   * Group member is alias.
   */
  const GROUP_MEMBER_ALIAS = 1;

  /**
   * Group member is distribution list.
   */
  const GROUP_MEMBER_DL = 2;

  /**
   * The ID of group member.
   *
   * @var integer
   */
  public $id;

  /**
   * The name of group member.
   *
   * @var string
   */
  public $name;

  /**
   * The {zmt_group}.id of group member.
   *
   * @var integer
   */
  public $group_id = 0;

  /**
   * The member id of the group member.
   *
   * @var integer
   */
  public $member_id = 0;

  /**
   * The member type of the group member.
   *
   * @var integer
   */
  public $type = 0;

  /**
   * The {users}.uid that created the group member.
   *
   * @var integer
   */
  public $uid = 0;

  /**
   * The Unix timestamp when the group member was created..
   *
   * @var integer
   */
  public $created = 0;

  /**
   * Create a new group instance.
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
    return 'zmt_group_member';
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
      'path' => 'zmt/group/' . $this->group_id,
    );
  }
}
