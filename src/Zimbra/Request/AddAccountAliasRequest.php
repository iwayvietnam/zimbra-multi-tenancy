<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * AddAccountAlias request class
 * Add an alias for the account
 */
class AddAccountAliasRequest extends SoapRequest {

  /**
   * Constructor method for AddAccountAliasRequest
   * @param  string $id Zimbra ID
   * @param  string $alias Alias
   * @return self
   */
  public function __construct($id, $alias) {
    parent::__construct();
    $this->id = trim($id);
    $this->alias = trim($alias);
  }
}
