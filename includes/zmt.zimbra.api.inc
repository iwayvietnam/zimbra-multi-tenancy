<?php

/**
 * @file
 * This API A simple Object Oriented wrapper for the zimbra soap api (web service).
 * It allows you to manage easily zimbra accounts, zimbra domains and zimbra servers.
 */

module_load_include('inc', 'zmt', 'includes/zmt.zimbra.soap');

/**
 * AdminInterface is a interface which allows to connect Zimbra API administration functions via SOAP
 */
interface ZimbraAdminApiInterface {

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
  function addAccountAlias($id, $alias);

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
  function addDistributionListMember($id, array $dlm);

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
   *   The account selector
   * @param string  $virtualHost
   *   Virtual host
   * @param bool    $persistAuthTokenCookie
   *   Controls whether the auth token cookie in the response should be persisted when the browser exits.
   * @return authentication token
   */
  function auth(
      $name = NULL,
      $password = NULL,
      $authToken = NULL,
      AccountSelector $account = NULL,
      $virtualHost = NULL,
      $persistAuthTokenCookie = NULL
  );

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
  function authByName($name, $password, $vhost = NULL);

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
  function authByAccount(AccountSelector $account, $password, $vhost = NULL);

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
  function authByToken($name, $token, $vhost = NULL);

  /**
   * Count number of accounts by cos in a domain.
   * Note: It doesn't include any account with zimbraIsSystemResource=TRUE,
   *       nor does it include any calendar resources.
   *
   * @param  DomainSelector $domain 
   *   Specify the domain.
   * @return SoapResponse
   */
  function countAccount(DomainSelector $domain = NULL);

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
  function createAccount($name, $password, array $attrs = array());

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
  function createCos($name, array $attrs = array());

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
  function createDistributionList($name, $dynamic = NULL, array $attrs = array());

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
  function createDomain($name, array $attrs = array());

  /**
   * Create an LDAP entry.
   *
   * @param  string $dn
   *   A valid LDAP DN String (RFC 2253) that describes the new DN to create.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  function createLDAPEntry($dn, array $attrs = array());

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
  function deleteAccount($id);

  /**
   * Delete a Class of Service (COS).
   *
   * @param  string $id Zimbra identify.
   * @return SoapResponse
   */
  function deleteCos($id);

  /**
   * Delete a distribution list.
   * Access: domain admin sufficient.
   *
   * @param  string $id Zimbra ID for distribution list.
   * @return SoapResponse
   */
  function deleteDistributionList($id);

  /**
   * Delete a domain.
   *
   * @param  string $id Zimbra ID for domain.
   * @return SoapResponse
   */
  function deleteDomain($id);

  /**
   * Delete an LDAP entry.
   *
   * @param  string $dn
   *   A valid LDAP DN String (RFC 2253) that describes the DN to delete.
   * @return SoapResponse
   */
  function deleteLDAPEntry($dn);

  /**
   * Get attributes related to an account.
   * {request-attrs} - comma-seperated list of attrs to return 
   * Note: this request is by default proxied to the account's home server 
   * Access: domain admin sufficient
   *
   * @param  AccountSelector $account
   *   Specify the account.
   * @param  bool    $applyCos
   *   Flag whether or not to apply class of service (COS) rules.
   * @param  string  $attrs
   *   Comma separated list of attributes.
   * @return SoapResponse
   */
  function getAccount(AccountSelector $account = NULL, $applyCos = NULL, $attrs = NULL);

  /**
   * Get information about an account.
   * Currently only 2 attrs are returned:
   *   zimbraId    the unique UUID of the zimbra account
   *   zimbraMailHost  the server on which this user's mail resides 
   * Access: domain admin sufficient
   *
   * @param  Account $account
   *   Specify the account.
   * @return SoapResponse
   */
  function getAccountInfo(AccountSelector $account);

  /**
   * Get All accounts matching the selectin criteria.
   * Access: domain admin sufficient
   *
   * @param  ServerSelector $server
   *   Specify server selector.
   * @param  DomainSelector $domain
   *   Specify domain selector.
   * @return SoapResponse
   */
  function getAllAccounts(ServerSelector $server = NULL, DomainSelector $domain = NULL);

  /**
   * Get all classes of service (COS).
   *
   * @return SoapResponse
   */
  function getAllCos();

  /**
   * Get all calendar resources that match the selection criteria.
   * Access: domain admin sufficient.
   *
   * @param  DomainSelector $domain
   *   Specify domain selector.
   * @return SoapResponse
   */
  function getAllDistributionLists(DomainSelector $domain = NULL);

  /**
   * Get all domains.
   *
   * @param  bool $applyConfig
   *   Apply config flag.
   * @return SoapResponse
   */
  function getAllDomains($applyConfig = NULL);

  /**
   * Get Class Of Service (COS).
   *
   * @param  CosSelector $cos
   *   Specify Class Of Service (COS)
   * @param  string $attrs
   *   Comma separated list of attributes.
   * @return SoapResponse
   */
  function getCos(CosSelector $cos = NULL, $attrs = NULL);

  /**
   * Get a Distribution List.
   *
   * @param  DistributionListSelector $dl
   *   Specify the distribution list.
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
  );

  /**
   * Get information about a domain.
   * 
   * @param  DomainSelector $domain
   *   Specify the domain.
   * @param  bool   $applyConfig
   *   Apply config flag.
   *   True, then certain unset attrs on a domain will get their values from the global config.
   *   False, then only attributes directly set on the domain will be returned.
   * @param  string $attrs
   *   Attributes.
   * @return SoapResponse
   */
  function getDomain(DomainSelector $domain = NULL, $applyConfig = NULL, $attrs = NULL);

  /**
   * Get Domain information.
   * This call does not require an auth token.
   * It returns attributes that are pertinent to domain settings
   * for cases when the user is not authenticated.
   * For example, URL to direct the user to upon logging out or when auth token is expired.
   * 
   * @param  DomainSelector $domain
   *   Specify the domain.
   * @param  bool   $applyConfig
   *   Apply config flag.
   *   True, then certain unset attrs on a domain will get their values from the global config.
   *   False, then only attributes directly set on the domain will be returned.
   * @return SoapResponse
   */
  function getDomainInfo(DomainSelector $domain = NULL, $applyConfig = NULL);

  /**
   * Get LDAP entries.
   * getLDAPEntries fetches ldap entry (or entries) by a search-base ({ldap-search-base}) and a search query ({query}). 
   * 
   * @param  string  $query
   *   Query string. Should be an LDAP-style filter string (RFC 2254).
   * @param  string  $ldapSearchBase
   *   LDAP search base. An LDAP-style filter string that defines an LDAP search base (RFC 2254).
   * @param  string  $sortBy
   *   Name of attribute to sort on. default is NULL.
   * @param  bool    $sortAscending
   *   Flag whether to sort in ascending order 1 (true) is default.
   * @param  integer $limit
   *   Limit - the maximum number of LDAP objects (records) to return (0 is default and means all).
   * @param  integer $offset
   *   The starting offset (0, 25, etc).
   * @return SoapResponse
   */
  function getLDAPEntries(
      $query,
      $ldapSearchBase,
      $sortBy = NULL,
      $sortAscending = NULL,
      $limit = NULL,
      $offset = NULL
  );

  /**
   * Get Version information.
   * 
   * @return SoapResponse
   */
  function getVersionInfo();

  /**
   * Get all installed skins on the server.
   * 
   * @return SoapResponse
   */
  function getAllSkins();

  /**
   * Modify an account.
   * 
   * @param  string $id
   *   Zimbra ID of account.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  function modifyAccount($id, array $attrs = array());

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
  function modifyCos($id, array $attrs = array());

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
  function modifyDistributionList($id, array $attrs = array());

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
  function modifyDomain($id, array $attrs = array());

  /**
   * Modify an LDAP Entry.
   * 
   * @param  string $dn
   *   A valid LDAP DN String (RFC 2253) that identifies the LDAP object.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  function modifyLDAPEntry($dn, array $attrs = array());

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
  function removeAccountAlias($alias, $id = NULL);

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
  function removeDistributionListMember($id, array $dlm);

  /**
   * Rename Account.
   * Access: domain admin sufficient.
   * Note: this request is by default proxied to the account's home server. 
   * 
   * @param  string $id
   *   Zimbra ID
   * @param  array  $newName
   *   New account name.
   * @return SoapResponse
   */
  function renameAccount($id, $newName);

  /**
   * Rename Class of Service (COS).
   * 
   * @param  string $id
   *   Zimbra ID
   * @param  array  $newName
   *   New COS name.
   * @return SoapResponse
   */
  function renameCos($id, $newName);

  /**
   * Rename Distribution List.
   * Access: domain admin sufficient.
   * 
   * @param  string $id
   *   Zimbra ID
   * @param  array  $newName
   *   New Distribution List name.
   * @return SoapResponse
   */
  function renameDistributionList($id, $newName);

  /**
   * Rename LDAP Entry.
   * 
   * @param  string $dn
   *   A valid LDAP DN String (RFC 2253) that identifies the LDAP object
   * @param  array  $new_dn
   *   New DN - a valid LDAP DN String (RFC 2253) that describes the new DN to be given to the LDAP object.
   * @return SoapResponse
   */
  function renameLDAPEntry($dn, $new_dn);

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
   * @param  bool    $countOnly
   *   Whether response should be count only. Default is 0 (false).
   * @param  string   $types
   *   Comma-separated list of types to return.
   *   Legal values are: accounts|distributionlists|aliases|resources|domains|coses. (default is accounts)
   * @param  string  $sortBy
   *   Name of attribute to sort on. Default is the account name.
   * @param  bool    $sortAscending
   *   Whether to sort in ascending order. Default is 1 (true).
   * @param  array   $attrs
   *   Comma separated list of attributes
   * @return SoapResponse
   */
  function searchDirectory(
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
  );

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
  function setPassword($id, $newPassword);
}

/**
 * ZimbraAdminApi is a class which allows to connect Zimbra API administration functions via SOAP using http protocol
 */
class ZimbraAdminApi implements ZimbraAdminApiInterface {

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
  function __construct($location, $debug = FALSE) {
    $this->_location = $location;
    $this->_client = new HttpSoapClient($this->_location, $debug);
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
    $request = new AddAccountAliasRequest(
      $id, $alias
    );
    return $this->_client->doRequest($request);
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
    $request = new AddDistributionListMemberRequest(
      $id, $dlm
    );
    return $this->_client->doRequest($request);
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
    $request = new AuthRequest(
      $name, $password, $authToken, $account,
      $virtualHost, $persistAuthTokenCookie
    );
    $result = $this->_client->doRequest($request);
    $authToken = NULL;
    if(isset($result->authToken) && !empty($result->authToken)){
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
   * Count number of accounts by cos in a domain.
   * Note: It doesn't include any account with zimbraIsSystemResource=TRUE,
   *       nor does it include any calendar resources.
   *
   * @param  DomainSelector $domain
   *   Specify the domain.
   * @return SoapResponse
   */
  public function countAccount(DomainSelector $domain = NULL) {
    $request = new CountAccountRequest(
      $domain
    );
    return $this->_client->doRequest($request);
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
    $request = new CreateAccountRequest(
      $name, $password, $attrs
    );
    return $this->_client->doRequest($request);
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
    $request = new CreateCosRequest(
      $name, $attrs
    );
    return $this->_client->doRequest($request);
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
    $request = new CreateDistributionListRequest(
      $name, $dynamic, $attrs
    );
    return $this->_client->doRequest($request);
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
    $request = new CreateDomainRequest(
      $name, $attrs
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Create an LDAP entry.
   *
   * @param  string $dn
   *   A valid LDAP DN String (RFC 2253) that describes the new DN to create.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  public function createLDAPEntry($dn, array $attrs = array()) {
    $request = new CreateLDAPEntryRequest(
      $dn, $attrs
    );
    return $this->_client->doRequest($request);
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
    $request = new DeleteAccountRequest(
      $id
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Delete a Class of Service (COS).
   *
   * @param  string $id
   *   Zimbra identify.
   * @return SoapResponse
   */
  public function deleteCos($id) {
    $request = new DeleteCosRequest(
      $id
    );
    return $this->_client->doRequest($request);
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
    $request = new DeleteDistributionListRequest(
      $id
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Delete a domain.
   *
   * @param  string $id
   *   Zimbra ID for domain.
   * @return SoapResponse
   */
  public function deleteDomain($id) {
    $request = new DeleteDomainRequest(
      $id
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Delete an LDAP entry.
   *
   * @param  string $dn A valid LDAP DN String (RFC 2253) that describes the DN to delete.
   * @return SoapResponse
   */
  public function deleteLDAPEntry($dn) {
    $request = new DeleteLDAPEntryRequest(
      $dn
    );
    return $this->_client->doRequest($request);
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
    $request = new GetAccountRequest(
      $account, $applyCos, $attrs
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Get information about an account.
   * Currently only 2 attrs are returned:
   *   zimbraId    the unique UUID of the zimbra account
   *   zimbraMailHost  the server on which this user's mail resides 
   * Access: domain admin sufficient
   *
   * @param  AccountSelector $account
   *   The name used to identify the account.
   * @return SoapResponse
   */
  public function getAccountInfo(AccountSelector $account) {
    $request = new GetAccountInfoRequest(
      $account
    );
    return $this->_client->doRequest($request);
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
    $request = new GetAllAccountsRequest(
      $server, $domain
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Get all classes of service (COS).
   *
   * @return SoapResponse
   */
  public function getAllCos() {
    $request = new GetAllCosRequest();
    return $this->_client->doRequest($request);
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
    $request = new GetAllDistributionListsRequest(
      $domain
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Get all domains.
   *
   * @param  bool $applyConfig
   *   Apply config flag.
   * @return SoapResponse
   */
  public function getAllDomains($applyConfig = NULL) {
    $request = new GetAllDomainsRequest(
      $applyConfig
    );
    return $this->_client->doRequest($request);
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
    $request = new GetCosRequest($cos, $attrs);
    return $this->_client->doRequest($request);
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
    $request = new GetDistributionListRequest(
      $dl, $limit, $offset, $sortAscending, $attrs
    );
    return $this->_client->doRequest($request);
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
    $request = new GetDomainRequest(
      $domain, $applyConfig, $attrs
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Get Domain information.
   * This call does not require an auth token.
   * It returns attributes that are pertinent to domain settings
   * for cases when the user is not authenticated.
   * For example, URL to direct the user to upon logging out or when auth token is expired.
   * 
   * @param  DomainSelector $domain
   *   The name used to identify the domain.
   * @param  bool   $applyConfig
   *   Apply config flag.
   *   True, then certain unset attrs on a domain will get their values from the global config.
   *   False, then only attributes directly set on the domain will be returned.
   * @return SoapResponse
   */
  public function getDomainInfo(DomainSelector $domain = NULL, $applyConfig = NULL) {
    $request = new GetDomainInfoRequest(
      $domain, $applyConfig
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Get index statistics.
   * 
   * @param  string  $query
   *   Query string. Should be an LDAP-style filter string (RFC 2254).
   * @param  string  $ldapSearchBase
   *   LDAP search base. An LDAP-style filter string that defines an LDAP search base (RFC 2254).
   * @param  string  $sortBy
   *   Name of attribute to sort on. default is NULL.
   * @param  bool    $sortAscending
   *   Flag whether to sort in ascending order 1 (true) is default.
   * @param  integer $limit
   *   Limit - the maximum number of LDAP objects (records) to return (0 is default and means all).
   * @param  integer $offset
   *   The starting offset (0, 25, etc).
   * @return SoapResponse
   */
  public function getLDAPEntries(
      $query,
      $ldapSearchBase,
      $sortBy = NULL,
      $sortAscending = NULL,
      $limit = NULL,
      $offset = NULL
  ) {
    $request = new GetLDAPEntriesRequest(
      $query, $ldapSearchBase, $sortBy, $sortAscending, $limit, $offset
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Get Version information.
   * 
   * @return SoapResponse
   */
  public function getVersionInfo() {
    $request = new GetVersionInfoRequest();
    return $this->_client->doRequest($request);
  }

  /**
   * Get all installed skins on the server.
   * 
   * @return SoapResponse
   */
  public function getAllSkins() {
    $request = new GetAllSkinsRequest();
    return $this->_client->doRequest($request);
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
    $request = new ModifyAccountRequest($id, $attrs);
    return $this->_client->doRequest($request);
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
    $request = new ModifyCosRequest($id, $attrs);
    return $this->_client->doRequest($request);
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
    $request = new ModifyDistributionListRequest(
      $id, $attrs
    );
    return $this->_client->doRequest($request);
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
    $request = new ModifyDomainRequest(
      $id, $attrs
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Modify an LDAP Entry.
   * 
   * @param  string $dn
   *   A valid LDAP DN String (RFC 2253) that identifies the LDAP object.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  public function modifyLDAPEntry($dn, array $attrs = array()) {
    $request = new ModifyLDAPEntryRequest(
      $dn, $attrs
    );
    return $this->_client->doRequest($request);
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
    $request = new RemoveAccountAliasRequest(
      $alias, $id
    );
    return $this->_client->doRequest($request);
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
    $request = new RemoveDistributionListMemberRequest(
      $id, $dlm
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Rename Account.
   * Access: domain admin sufficient.
   * Note: this request is by default proxied to the account's home server. 
   * 
   * @param  string $id
   *   Zimbra ID
   * @param  array  $newName
   *   New account name.
   * @return SoapResponse
   */
  public function renameAccount($id, $newName) {
    $request = new RenameAccountRequest(
      $id, $newName
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Rename Class of Service (COS).
   * 
   * @param  string $id
   *   Zimbra ID
   * @param  array  $newName
   *   New COS name.
   * @return SoapResponse
   */
  public function renameCos($id, $newName) {
    $request = new RenameCosRequest(
      $id, $newName
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Rename Distribution List.
   * Access: domain admin sufficient.
   * 
   * @param  string $id
   *   Zimbra ID
   * @param  array  $newName
   *   New Distribution List name.
   * @return SoapResponse
   */
  public function renameDistributionList($id, $newName) {
    $request = new RenameDistributionListRequest(
      $id, $newName
    );
    return $this->_client->doRequest($request);
  }

  /**
   * Rename LDAP Entry.
   * 
   * @param  string $dn
   *   A valid LDAP DN String (RFC 2253) that identifies the LDAP object
   * @param  array  $new_dn
   *   New DN - a valid LDAP DN String (RFC 2253) that describes the new DN to be given to the LDAP object.
   * @return SoapResponse
   */
  public function renameLDAPEntry($dn, $new_dn) {
    $request = new RenameLDAPEntryRequest(
      $dn, $new_dn
    );
    return $this->_client->doRequest($request);
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
    $request = new SearchDirectoryRequest(
      $query, $maxResults, $limit, $offset, $domain, $applyCos,
      $applyConfig, $types, $sortBy, $sortAscending, $countOnly, $attrs
    );
    return $this->_client->doRequest($request);
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
    $request = new SetPasswordRequest(
      $id, $newPassword
    );
    return $this->_client->doRequest($request);
  }
}
