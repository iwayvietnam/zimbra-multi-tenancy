<?php

namespace Drupal\zmt\Zimbra;

use Drupal\zmt\Zimbra\Struct\AccountSelector;
use Drupal\zmt\Zimbra\Struct\DistributionListSelector;
use Drupal\zmt\Zimbra\Struct\DomainSelector;
use Drupal\zmt\Zimbra\Struct\CosSelector;
use Drupal\zmt\Zimbra\Struct\ServerSelector;

/**
 * AdminInterface is a interface which allows to connect Zimbra API administration functions via SOAP.
 */
interface AdminApiInterface {

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
   * Check password strength.
   *
   * @param string $id Zimbra ID
   * @param string $password Passowrd to check
   * @return mix.
   */
  function checkPasswordStrength($id, $password);

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
   * Count number of objects.
   * Returns number of objects of requested type. 
   * Note: For account/alias/dl, if a domain is specified, only entries on the specified domain are counted.
   *       If no domain is specified, entries on all domains are counted.
   *
   * @param  string $type 
   *   Object type.
   * @param  DomainSelector $domain 
   *   Specify the domain.
   * @return SoapResponse
   */
  function countObjects($type, DomainSelector $domain = NULL);

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
  function createAccount($name, $password, array $attrs = []);

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
  function createCos($name, array $attrs = []);

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
  function createDistributionList($name, $dynamic = NULL, array $attrs = []);

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
  function createDomain($name, array $attrs = []);

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
    array $attrs = []
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
  function getQuotaUsage(
    $domain = null,
    $allServers = null,
    $limit = null,
    $offset = null,
    $sortBy = null,
    $sortAscending = null,
    $refresh = null
  );

  /**
   * Modify an account.
   * 
   * @param  string $id
   *   Zimbra ID of account.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  function modifyAccount($id, array $attrs = []);

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
  function modifyCos($id, array $attrs = []);

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
  function modifyDistributionList($id, array $attrs = []);

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
  function modifyDomain($id, array $attrs = []);

  /**
   * Modify an LDAP Entry.
   * 
   * @param  string $dn
   *   A valid LDAP DN String (RFC 2253) that identifies the LDAP object.
   * @param  array  $attrs
   *   Attributes.
   * @return SoapResponse
   */
  function modifyLDAPEntry($dn, array $attrs = []);

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
