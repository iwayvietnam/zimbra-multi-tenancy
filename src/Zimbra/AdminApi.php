<?php

namespace Drupal\zmt\Zimbra\Struct;

use Drupal\zmt\Zimbra\Struct\AccountSelector;
use Drupal\zmt\Zimbra\Struct\DistributionListSelector;
use Drupal\zmt\Zimbra\Struct\DomainSelector;
use Drupal\zmt\Zimbra\Struct\CosSelector;
use Drupal\zmt\Zimbra\Struct\ServerSelector;

/**
 * AdminApi is a class which allows to connect Zimbra API administration functions via SOAP using http protocol
 */
class ZimbraAdminApi implements AdminApiInterface {

  /**
   * The Zimbra api soap location
   * @var string
   */
  private $_location;

  /**
   * Zimbra soap client
   * @var HttpSoapClient 
   */
  private $_client;
  
  /**
   * ZimbraAdminApi constructor.
   *
   * @param string $location
   *   The Zimbra api soap location.
   * @return self.
   */
  function __construct($location) {
    $this->_location = $location;
    $this->_client = new HttpSoapClient($this->_location);
  }

  /**
   * Get Zimbra api soap client.
   *
   * @return ClientInterface
   */
  public function client() {
    return $this->_client;
  }

  /**
   * Get Zimbra api soap location.
   *
   * @return string
   */
  public function location() {
    return $this->_location;
  }

  /**
   * Add an alias for the account.
   * Access: domain admin sufficient.
   * Note: this request is by default proxied to the account's home server.
   *
   * @param  string $id
   *   Value of zimbra identify.
   * @param  string $alias
   *   Account alias.
   * @return SoapResponse
   */
  public function addAccountAlias($id, $alias) {
    $req = new \Drupal\zmt\Zimbra\Request\AddAccountAliasRequest(
      $id, $alias
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Adding members to a distribution list.
   * Access: domain admin sufficient.
   *
   * @param  string $id
   *   Value of zimbra identify.
   * @param  array  $dlm
   *   Distribution list members.
   * @return SoapResponse
   */
  public function addDistributionListMember($id, array $dlm){
    $req = new \Drupal\zmt\Zimbra\Request\AddDistributionListMemberRequest(
      $id, $dlm
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Authenticate for an adminstration account.
   *
   * @param string  $name
   *   Name. Only one of {auth-name} or <account> can be specified
   * @param string  $password
   *   Password - must be present if not using AuthToken
   * @param string  $authToken
   *   An authToken can be passed instead of account/password/name to validate an existing auth token.
   * @param AccountSelector $account
   *  The account
   * @param string  $virtualHost
   *   Virtual host
   * @param bool    $persistAuthTokenCookie
   *   Controls whether the auth token cookie in the response should be persisted when the browser exits.
   * @return authentication token
   */
  public function auth(
    $name = NULL,
    $password = NULL,
    $authToken = NULL,
    AccountSelector $account = NULL,
    $virtualHost = NULL,
    $persistAuthTokenCookie = NULL
  ) {
    $req = new \Drupal\zmt\Zimbra\Request\AuthRequest(
      $name, $password, $authToken, $account,
      $virtualHost, $persistAuthTokenCookie
    );
    $result = $this->_client->doRequest($req);
    $authToken = NULL;
    if (!empty($result->authToken)){
      if (isset($result->authToken[0]->_content)) {
        $authToken =  $result->authToken[0]->_content;
      }
      $this->_client->setAuthToken($authToken);
    }
    return $authToken;
  }

  /**
   * Authenticate for an adminstration account.
   *
   * @param  string $name
   *   Name. Only one of {auth-name} or <account> can be specified
   * @param  string $password
   *   The user password.
   * @param  string $vhost
   *   Virtual-host is used to determine the domain of the account name.
   * @return authentication token
   */
  public function authByName($name, $password, $vhost = NULL) {
    return $this->auth($name, $password, NULL, NULL, $vhost, TRUE);
  }

  /**
   * Authenticate for an adminstration account.
   *
   * @param  AccountSelector $account
   *   The user account.
   * @param  string $password
   *   The user password.
   * @param  string $vhost
   *   Virtual-host is used to determine the domain of the account name.
   * @return authentication token
   */
  public function authByAccount(AccountSelector $account, $password, $vhost = NULL) {
    return $this->auth(NULL, $password, NULL, $account, $vhost, TRUE);
  }

  /**
   * Authenticate for an adminstration account.
   *
   * @param  string $name
   *   Name. Only one of {auth-name} or <account> can be specified
   * @param  string $token
   *   The authentication token.
   * @param  string $vhost
   *   Virtual-host is used to determine the domain of the account name.
   * @return authentication token.
   */
  public function authByToken($name, $token, $vhost = NULL) {
    return $this->auth($name, NULL, $token, NULL, $vhost, TRUE);
  }

  /**
   * Check password strength.
   *
   * @param string $id Zimbra ID
   * @param string $password Passowrd to check
   * @return mix.
   */
  public function checkPasswordStrength($id, $password) {
    $req = new \Drupal\zmt\Zimbra\Request\CheckPasswordStrengthRequest(
      $id, $password
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Create account.
   * Notes:
   *   1. accounts without passwords can't be logged into.
   *   2. name must include domain (uid@name), and domain specified in name must exist.
   *   3. default value for zimbraAccountStatus is "active".
   * Access: domain admin sufficient.
   *
   * @param  string $name
   *   New account's name. Must include domain (uid@name), and domain specified in name must exist.
   * @param  string $password
   *   New account's password.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  public function createAccount($name, $password, array $attrs = array()) {
    $req = new \Drupal\zmt\Zimbra\Request\CreateAccountRequest(
      $name, $password, $attrs
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Create a Class of Service (COS).
   * Notes:
   *   1. Extra attrs: description, zimbraNotes.
   *
   * @param  string $name
   *   Class of Service name.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  public function createCos($name, array $attrs = array()) {
    $req = new \Drupal\zmt\Zimbra\Request\CreateCosRequest(
      $name, $attrs
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Create a distribution list.
   * Notes:
   *   1. dynamic - create a dynamic distribution list.
   *   2. Extra attrs: description, zimbraNotes.
   *
   * @param  string $name
   *   Name for distribution list.
   * @param  bool   $dynamic
   *   If 1 (true) then create a dynamic distribution list.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  public function createDistributionList($name, $dynamic = NULL, array $attrs = array()) {
    $req = new \Drupal\zmt\Zimbra\Request\CreateDistributionListRequest(
      $name, $dynamic, $attrs
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Create a domain.
   * Note:
   *   1. Extra attrs: description, zimbraNotes.
   *
   * @param  string $name
   *   Name of new domain.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  public function createDomain($name, array $attrs = array()) {
    $req = new \Drupal\zmt\Zimbra\Request\CreateDomainRequest(
      $name, $attrs
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Deletes the account with the given id.
   * Notes:
   *   1. If the request is sent to the server on which the mailbox resides,
   *      the mailbox is deleted as well.
   *   1. this request is by default proxied to the account's home server.
   *
   * @param  string $id
   *   Zimbra identify.
   * @return SoapResponse
   */
  public function deleteAccount($id) {
    $req = new \Drupal\zmt\Zimbra\Request\DeleteAccountRequest(
      $id
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Delete a Class of Service (COS).
   *
   * @param  string $id
   *   Zimbra identify.
   * @return SoapResponse
   */
  public function deleteCos($id) {
    $req = new \Drupal\zmt\Zimbra\Request\DeleteCosRequest(
      $id
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Delete a distribution list.
   * Access: domain admin sufficient.
   *
   * @param  string $id
   *   Zimbra ID for distribution list.
   * @return SoapResponse
   */
  public function deleteDistributionList($id) {
    $req = new \Drupal\zmt\Zimbra\Request\DeleteDistributionListRequest(
      $id
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Delete a domain.
   *
   * @param  string $id
   *   Zimbra ID for domain.
   * @return SoapResponse
   */
  public function deleteDomain($id) {
    $req = new \Drupal\zmt\Zimbra\Request\DeleteDomainRequest(
      $id
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Get attributes related to an account.
   * {request-attrs} - comma-seperated list of attrs to return 
   * Note: this request is by default proxied to the account's home server 
   * Access: domain admin sufficient
   *
   * @param  AccountSelector $account
   *   The name used to identify the account.
   * @param  bool    $applyCos
   *   Flag whether or not to apply class of service (COS) rules.
   * @param  string  $attrs
   *   Comma separated list of attributes.
   * @return SoapResponse
   */
  public function getAccount(AccountSelector $account = NULL, $applyCos = NULL, $attrs = NULL) {
    $req = new \Drupal\zmt\Zimbra\Request\GetAccountRequest(
      $account, $applyCos, $attrs
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Get All accounts matching the selectin criteria.
   * Access: domain admin sufficient
   *
   * @param  ServerSelector $server
   *   The server selector.
   * @param  DomainSelector $domain
   *   The domain selector.
   * @return SoapResponse
   */
  public function getAllAccounts(ServerSelector $server = NULL, DomainSelector $domain = NULL) {
    $req = new \Drupal\zmt\Zimbra\Request\GetAllAccountsRequest(
      $server, $domain
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Get all classes of service (COS).
   *
   * @return SoapResponse
   */
  public function getAllCos() {
    $req = new \Drupal\zmt\Zimbra\Request\GetAllCosRequest();
    return $this->_client->doRequest($req);
  }

  /**
   * Get all calendar resources that match the selection criteria.
   * Access: domain admin sufficient.
   *
   * @param  DomainSelector $domain
   *   The domain selector.
   * @return SoapResponse
   */
  public function getAllDistributionLists(DomainSelector $domain = NULL) {
    $req = new \Drupal\zmt\Zimbra\Request\GetAllDistributionListsRequest(
      $domain
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Get all domains.
   *
   * @param  bool $applyConfig
   *   Apply config flag.
   * @return SoapResponse
   */
  public function getAllDomains($applyConfig = NULL) {
    $req = new \Drupal\zmt\Zimbra\Request\GetAllDomainsRequest(
      $applyConfig
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Get Class Of Service (COS).
   *
   * @param  CosSelector $cos
   *   The name used to identify the COS.
   * @param  string $attrs
   *   Comma separated list of attributes.
   * @return SoapResponse
   */
  public function getCos(CosSelector $cos = NULL, $attrs = NULL) {
    $req = new \Drupal\zmt\Zimbra\Request\GetCosRequest($cos, $attrs);
    return $this->_client->doRequest($req);
  }

  /**
   * Get a Distribution List.
   *
   * @param  DistributionListSelector $dl
   *   The name used to identify the distribution list.
   * @param  integer  $limit
   *   The maximum number of accounts to return (0 is default and means all).
   * @param  integer  $offset
   *   The starting offset (0, 25 etc).
   * @param  bool     $sortAscending
   *   Flag whether to sort in ascending order 1 (true) is the default.
   * @param  array    $attrs
   *   Attributes.
   * @return SoapResponse
   */
  function getDistributionList(
    DistributionListSelector $dl = NULL,
    $limit = NULL,
    $offset = NULL,
    $sortAscending = NULL,
    array $attrs = array()
  ) {
    $req = new \Drupal\zmt\Zimbra\Request\GetDistributionListRequest(
      $dl, $limit, $offset, $sortAscending, $attrs
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Get information about a domain.
   * 
   * @param  DomainSelector $domain
   *   The name used to identify the domain.
   * @param  bool   $applyConfig
   *   Apply config flag.
   *   True, then certain unset attrs on a domain will get their values from the global config.
   *   False, then only attributes directly set on the domain will be returned.
   * @param  string $attrs
   *   Attributes.
   * @return SoapResponse
   */
  function getDomain(
    DomainSelector $domain = NULL,
    $applyConfig = NULL,
    $attrs = NULL
  ) {
    $req = new \Drupal\zmt\Zimbra\Request\GetDomainRequest(
      $domain, $applyConfig, $attrs
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Get Quota Usage
   * 
   * @param string $domain Domain - the domain name to limit the search to
   * @param bool $allServers Ưhether to fetch quota usage for all domain accounts from across all mailbox servers, default is false, applicable when domain attribute is specified
   * @param int $limit Limit - the number of accounts to return (0 is default and means all)
   * @param int $offset Offset - the starting offset (0, 25, etc)
   * @param $sortBy SortBy - valid values: "percentUsed", "totalUsed", "quotaLimit"
   * @param bool $sortAscending Whether to sort in ascending order 0 (false) is default, so highest quotas are returned first
   * @param bool $refresh Refresh - whether to always recalculate the data even when cached values are available. 0 (false) is the default.
   * @return SoapResponse
   */
  public function getQuotaUsage(
    $domain = null,
    $allServers = null,
    $limit = null,
    $offset = null,
    $sortBy = null,
    $sortAscending = null,
    $refresh = null
  ) {
    $req = new GetQuotaUsageRequest(
      $domain, $allServers, $limit, $offset, $sortBy, $sortAscending, $refresh
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Modify an account.
   * 
   * @param  string $id
   *   Zimbra ID of account.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  public function modifyAccount($id, array $attrs = array()) {
    $req = new \Drupal\zmt\Zimbra\Request\ModifyAccountRequest($id, $attrs);
    return $this->_client->doRequest($req);
  }

  /**
   * Modify Class of Service (COS) attributes.
   * Note: an empty attribute value removes the specified attr.
   * 
   * @param  string $id
   *   Zimbra ID.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  public function modifyCos($id, array $attrs = array()) {
    $req = new \Drupal\zmt\Zimbra\Request\ModifyCosRequest($id, $attrs);
    return $this->_client->doRequest($req);
  }

  /**
   * Modify attributes for a Distribution List.
   * Notes: an empty attribute value removes the specified attr.
   * Access: domain admin sufficient.
   * 
   * @param  string $id
   *   Zimbra ID.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  public function modifyDistributionList($id, array $attrs = array()) {
    $req = new \Drupal\zmt\Zimbra\Request\ModifyDistributionListRequest(
      $id, $attrs
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Modify attributes for a domain.
   * Note: an empty attribute value removes the specified attr.
   * 
   * @param  string $id
   *   Zimbra ID.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  public function modifyDomain($id, array $attrs = array()) {
    $req = new \Drupal\zmt\Zimbra\Request\ModifyDomainRequest(
      $id, $attrs
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Remove Account Alias.
   * Access: domain admin sufficient.
   * Note: this request is by default proxied to the account's home server.
   * 
   * @param  string $alias
   *   Account alias.
   * @param  string $id
   *   Zimbra ID.
   * @return SoapResponse
   */
  public function removeAccountAlias($alias, $id = NULL) {
    $req = new \Drupal\zmt\Zimbra\Request\RemoveAccountAliasRequest(
      $alias, $id
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Remove Distribution List Member.
   * Unlike add, remove of a non-existent member causes an exception and no modification to the list. 
   * Access: domain admin sufficient.
   * 
   * @param  string $id
   *   Zimbra ID
   * @param  array  $dlm
   *   Members.
   * @return SoapResponse
   */
  public function removeDistributionListMember($id, array $dlm) {
    $req = new \Drupal\zmt\Zimbra\Request\RemoveDistributionListMemberRequest(
      $id, $dlm
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Search directory.
   * Access: domain admin sufficient (though a domain admin can't specify "domains" as a type).
   * 
   * @param  string  $query
   *   Query string - should be an LDAP-style filter string (RFC 2254).
   * @param  integer $maxResults
   *   Maximum results that the backend will attempt to fetch from the directory before returning an account.
   *   TOO_MANY_SEARCH_RESULTS error.
   * @param  integer $limit
   *   The maximum number of accounts to return (0 is default and means all).
   * @param  integer $offset
   *   The starting offset (0, 25, etc).
   * @param  string  $domain
   *   The domain name to limit the search to.
   * @param  bool    $applyCos
   *   Flag whether or not to apply the COS policy to account.
   *   Specify 0 (false) if only requesting attrs that aren't inherited from COS.
   * @param  bool    $applyConfig
   *   Whether or not to apply the global config attrs to account.
   *   Specify 0 (false) if only requesting attrs that aren't inherited from global config.
   * @param  string   $types
   *   Comma-separated list of types to return.
   *   Legal values are: accounts|distributionlists|aliases|resources|domains|coses. (default is accounts)
   * @param  string  $sortBy
   *   Name of attribute to sort on. Default is the account name.
   * @param  bool    $sortAscending
   *   Whether to sort in ascending order. Default is 1 (true).
   * @param  bool    $countOnly
   *   Whether response should be count only. Default is 0 (false).
   * @param  array   $attrs
   *   Comma separated list of attributes
   * @return SoapResponse
   */
  public function searchDirectory(
    $query = NULL,
    $maxResults = NULL,
    $limit = NULL,
    $offset = NULL,
    $domain = NULL,
    $applyCos = NULL,
    $applyConfig = NULL,
    $types = NULL,
    $sortBy = NULL,
    $sortAscending = NULL,
    $countOnly = NULL,
    $attrs = NULL
  ) {
    $req = new \Drupal\zmt\Zimbra\Request\SearchDirectoryRequest(
      $query, $maxResults, $limit, $offset, $domain, $applyCos,
      $applyConfig, $types, $sortBy, $sortAscending, $countOnly, $attrs
    );
    return $this->_client->doRequest($req);
  }

  /**
   * Set Password.
   * Access: domain admin sufficient.
   * Note: this request is by default proxied to the account's home server.
   * 
   * @param  string $id
   *   Zimbra ID.
   * @param  string $newPassword
   *   New password.
   * @return SoapResponse
   */
  public function setPassword($id, $newPassword) {
    $req = new \Drupal\zmt\Zimbra\Request\SetPasswordRequest(
      $id, $newPassword
    );
    return $this->_client->doRequest($req);
  }
}
