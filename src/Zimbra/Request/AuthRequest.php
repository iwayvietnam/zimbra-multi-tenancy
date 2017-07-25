<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;
use Drupal\zmt\Zimbra\Struct\AccountSelector;

/**
 * AuthRequest request class
 * Authenticate for administration
 */
class AuthRequest extends SoapRequest {

  /**
   * Constructor method for Auth
   * @param string  $name
   *   Name. Only one of {auth-name} or <account> can be specified
   * @param string  $password
   *   Password - must be present if not using AuthToken
   * @param string  $authToken
   *   An authToken can be passed instead of account/password/name to validate an existing auth authToken.
   * @param AccountSelector $account
   *   The account
   * @param string  $virtualHost
   *   Virtual host
   * @param bool    $persistAuthTokenCookie
   *   Controls whether the auth authToken cookie in the response should be persisted when the browser exits.
   * @return self
   */
  public function __construct($name = NULL,
    $password = NULL,
    $authToken = NULL,
    AccountSelector $account = NULL,
    $virtualHost = NULL,
    $persistAuthTokenCookie = NULL
  ) {
    parent::__construct();
    if (NULL !== $name) {
      $this->name = trim($name);
    }
    if (NULL !== $password) {
      $this->password = trim($password);
    }
    if (NULL !== $authToken) {
      $this->authToken = [
        [
          '_content' => trim($authToken)
        ]
      ];
    }
    if ($account instanceof AccountSelector) {
      $this->account = $account;
    }
    if (NULL !== $virtualHost) {
      $this->virtualHost = trim($virtualHost);
    }
    if (NULL !== $persistAuthTokenCookie) {
      $this->persistAuthTokenCookie = (bool) $persistAuthTokenCookie;
    }
  }
}
