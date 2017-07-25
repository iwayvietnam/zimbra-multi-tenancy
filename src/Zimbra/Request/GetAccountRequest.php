<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;
use Drupal\zmt\Zimbra\Struct\AccountSelector;

/**
 * GetAccountRequest request class
 * Get attributes related to an account
 */
class GetAccountRequest extends SoapRequest {

  /**
   * Constructor method for GetAccount
   * @param  AccountSelector $account
   * @param  bool $applyCos
   *   Flag whether or not to apply class of service (COS) rules
   * @param  string $attrs
   *   Comma separated list of attributes
   * @return self
   */
  public function __construct(AccountSelector $account = NULL, $applyCos = NULL, $attrs = NULL) {
    parent::__construct();
    if ($account instanceof AccountSelector) {
      $this->account = $account;
    }
    if (NULL !== $applyCos) {
      $this->applyCos = (bool) $applyCos;
    }
    if (NULL !== $attrs) {
      $this->attrs = trim($attrs);
    }
  }
}
