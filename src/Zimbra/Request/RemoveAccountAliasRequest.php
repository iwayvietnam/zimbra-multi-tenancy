<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * RemoveAccountAliasRequest request class
 * Remove Account Alias
 */
class RemoveAccountAliasRequest extends SoapRequest {

  /**
   * Constructor method for RemoveAccountAliasRequest
   * @param string $alias Alias
   * @param string $id Zimbra ID
   * @return self
   */
  public function __construct($alias, $id = NULL) {
    parent::__construct();
    $this->alias = trim($alias);
    if (NULL !== $id) {
      $this->id = trim($id);
    }
  }
}
