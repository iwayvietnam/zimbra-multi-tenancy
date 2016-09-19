<?php

/**
 * @file
 * This API A simple Object Oriented wrapper for the zimbra soap api (web service).
 * It allows you to manage easily zimbra accounts, zimbra domains and zimbra servers.
 */

/**
 * Interface for SoapStruct.
 */
interface SoapStructInterface extends ArrayAccess {

  /**
   * Sets value
   *
   * @param  string $value
   * @return self
   */
  function setValue($value);

  /**
   * Gets value
   *
   * @return string
   */
  function getValue();

  /**
   * Sets namespace
   *
   * @param  string $value
   * @return self
   */
  function setNamespace($namespace);

  /**
   * Gets namespace
   *
   * @return string
   */
  function getNamespace() ;

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  function toArray($name = NULL);
}

/**
 * A class representation soap struct.
 */
abstract class SoapStruct implements SoapStructInterface {

  /**
   * Struct value
   * @var string
   */
  private $_value;

  /**
   * Properties
   *
   * @var array
   */
  private $_properties = array();

  /**
   * soap struct namespace
   * @var string
   */
  private $_namespace = NULL;

  /**
   * Constructor method for SoapStruct
   *
   * @param  string $value
   * @return self
   */
  public function __construct($value = NULL){
    if (NULL !== $value) {
      $this->_value = trim($value);
    }
  }

  /**
   * Sets value
   *
   * @param  string $value
   * @return self
   */
  public function setValue($value) {
    $this->_value = trim($value);
    return $this;
  }

  /**
   * Gets value
   *
   * @return string
   */
  public function getValue() {
    return $this->_value;
  }

  /**
   * Sets namespace
   *
   * @param  string $value
   * @return self
   */
  public function setNamespace($namespace) {
    $this->_namespace = trim($namespace);
    return $this;
  }

  /**
   * Gets namespace
   *
   * @return string
   */
  public function getNamespace() {
    return $this->_namespace;
  }

  /**
   * Returns name representation of this class 
   *
   * @return string
   */
  public function className(){
    $ref = new ReflectionObject($this);
    return $ref->getName();
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = NULL) {
    $name = !empty($name) ? $name : $this->className();
    $arr = array();
    if (NULL !== $this->_value) {
      $arr['_content'] = $this->_value;
    }
    if (!empty($this->_namespace)) {
      $arr['_jsns'] = $this->_namespace;
    }
    if (!empty($this->_properties)) {
      foreach ($this->_properties as $key => $value) {
        if ($value instanceof SoapStructInterface) {
          $arr += $value->toArray($key);
        }
        elseif (is_array($value) && !empty($value)) {
          $arr[$key] = array();
          foreach ($value as $child) {
            if ($child instanceof SoapStructInterface) {
              $childArr = $child->toArray($key);
              $arr[$key][] = $childArr[$key];
            }
            else {
              $arr[$key][] = $child;
            }
          }
        }
        else {
          $arr[$key] = $value;
        }
      }
    }
    return array($name => $arr);
  }

  /**
   * Get a data by key
   *
   * @param string $key
   *   The key data to retrieve
   */
  public function &__get($key) {
    return $this->_properties[$key];
  }

  /**
   * Assigns a value to the specified data
   *
   * @param string $key
   *   The data key to assign the value to
   * @param mixed  $value
   *   The value to set
   */
  public function __set($key, $value) {
    $this->_properties[$key] = $value;
  }

  /**
   * Whether or not an data exists by key
   *
   * @param string $key
   *   An data key to check for
   * @return boolean
   */
  public function __isset($key) {
    return isset($this->_properties[$key]);
  }

  /**
   * Unsets an data by key
   *
   * @param string $key
   *   The key to unset
   */
  public function __unset($key) {
    unset($this->_properties[$key]);
  }

  /**
   * Assigns a value to the specified offset.
   *
   * @param string $offset
   *   The offset to assign the value to
   * @param mixed  $value
   *   The value to set
   */
  public function offsetSet($offset, $value) {
    if (is_null($offset)) {
      $this->_properties[] = $value;
    }
    else {
      $this->_properties[$offset] = $value;
    }
  }

  /**
   * Whether or not an offset exists
   *
   * @param string $offset
   *   An offset to check for
   * @return boolean
   */
  public function offsetExists($offset) {
    return isset($this->_properties[$offset]);
  }

  /**
   * Unsets an offset
   *
   * @param string $offset
   *   The offset to unset
   */
  public function offsetUnset($offset) {
    if ($this->offsetExists($offset)) {
      unset($this->_properties[$offset]);
    }
  }

  /**
   * Returns the value at specified offset
   *
   * @param string $offset
   *   The offset to retrieve
   * @return mixed
   */
  public function offsetGet($offset) {
    return $this->offsetExists($offset) ? $this->_properties[$offset] : NULL;
  }
}

/**
 * KeyValuePair struct class
 */
class KeyValuePair extends SoapStruct {

  /**
   * Constructor method for KeyValuePair
   * @param string $key
   * @param string $value
   * @return self
   */
  public function __construct($key, $value = NULL){
    parent::__construct($value);
    $this->n = $key;
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = 'a') {
    return parent::toArray($name);
  }
}

/**
 * AccountSelector struct class
 */
class AccountSelector extends SoapStruct {

  /**
   * Account by enums
   *
   * @var array
   */
  private static $_by_enums = array(
    'adminName',
    'appAdminName',
    'foreignPrincipal',
    'id',
    'krb5Principal',
    'name',
  );

  /**
   * Constructor method for AccountSelector
   * @param  string $by
   * @param  string $value
   * @return self
   */
  public function __construct($by = 'name', $value = NULL) {
    parent::__construct(trim($value));
    $this->by = in_array($by, self::$_by_enums) ? $by : 'name';
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = 'account') {
    if (!in_array($this->by, self::$_by_enums)) {
      $this->by = 'name';
    }
    return parent::toArray($name);
  }
}

/**
 * CosSelector struct class
 */
class CosSelector extends SoapStruct {

  /**
   * Cos by enums
   *
   * @var array
   */
  private static $_by_enums = array(
    'id',
    'name',
  );

  /**
   * Constructor method for CosSelector
   * @param  string $by
   * @param  string $value
   * @return self
   */
  public function __construct($by = 'name', $value = NULL) {
    parent::__construct(trim($value));
    $this->by = in_array($by, self::$_by_enums) ? $by : 'name';
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = 'cos') {
    if (!in_array($this->by, self::$_by_enums)) {
      $this->by = 'name';
    }
    return parent::toArray($name);
  }
}

/**
 * DistributionListSelector struct class
 */
class DistributionListSelector extends SoapStruct {

  /**
   * Distribution list by enums
   *
   * @var array
   */
  private static $_by_enums = array(
    'id',
    'name',
  );

  /**
   * Constructor method for CosSelector
   * @param  string $by
   * @param  string $value
   * @return self
   */
  public function __construct($by = 'name', $value = NULL) {
    parent::__construct(trim($value));
    $this->by = in_array($by, self::$_by_enums) ? $by : 'name';
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = 'dl') {
    if (!in_array($this->by, self::$_by_enums)) {
      $this->by = 'name';
    }
    return parent::toArray($name);
  }
}

/**
 * DomainSelector struct class
 */
class DomainSelector extends SoapStruct {

  /**
   * Domain by enums
   *
   * @var array
   */
  private static $_by_enums = array(
    'foreignName',
    'id',
    'krb5Realm',
    'name',
    'virtualHostname',
  );

  /**
   * Constructor method for CosSelector
   * @param  string $by
   * @param  string $value
   * @return self
   */
  public function __construct($by = 'name', $value = NULL) {
    parent::__construct(trim($value));
    $this->by = in_array($by, self::$_by_enums) ? $by : 'name';
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = 'domain') {
    if (!in_array($this->by, self::$_by_enums)) {
      $this->by = 'name';
    }
    return parent::toArray($name);
  }
}

/**
 * ServerSelector struct class
 */
class ServerSelector extends SoapStruct {

  /**
   * Server by enums
   *
   * @var array
   */
  private static $_by_enums = array(
    'id',
    'name',
    'serviceHostname',
  );

  /**
   * Constructor method for ServerSelector
   * @param  string $by
   *   Selects the meaning of {server-key}
   * @param  string $value
   *   Key for choosing server
   * @return self
   */
  public function __construct($by, $value = NULL) {
    parent::__construct(trim($value));
    $this->by = in_array($by, self::$_by_enums) ? $by : 'name';
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = 'server') {
    if (!in_array($this->by, self::$_by_enums)) {
      $this->by = 'name';
    }
    return parent::toArray($name);
  }
}

/**
 * Soap request struct class
 */
abstract class SoapRequest extends SoapStruct {

  /**
   * SoapRequest constructor
   *
   * @param  string $value
   * @return self
   */
  public function __construct($value = NULL) {
    parent::__construct($value);
    $this->setNamespace('urn:zimbraAdmin');
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = NULL) {
    $name = empty($name) ? $this->className() : $name;
    return parent::toArray($name);
  }
}

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

/**
 * AddDistributionListMember request class
 * Adding members to a distribution list
 */
class AddDistributionListMemberRequest extends SoapRequest {

  /**
   * Constructor method for AddDistributionListMember
   * @param  string $id Zimbra ID
   * @param  array  $dlm Members
   * @return self
   */
  public function __construct($id, array $dlm) {
    parent::__construct();
    $this->id = trim($id);
    if (!empty($dlm)) {
      $this->dlm = array();
      foreach ($dlm as $member) {
        $this->dlm[] = array('_content' => $member);
      }
    }
  }
}

/**
 * Auth request class
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
      $this->authToken = array(array('_content' => trim($authToken)));
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

/**
 * CheckPasswordStrength request class
 * Check password strength 
 */
class CheckPasswordStrengthRequest extends SoapRequest {
  
  /**
   * Constructor method for CountAccountRequest
   * @param string $id Zimbra ID
   * @param string $password Passowrd to check
   * @return self
   */
  public function __construct($id, $password) {
    parent::__construct();
    $this->id = trim($id);
    $this->password = trim($password);
  }
}

/**
 * CountAccount request class
 * Count number of accounts by cos in a domain.
 */
class CountAccountRequest extends SoapRequest {

  /**
   * Constructor method for CountAccountRequest
   * @param DomainSelector $domain Domain
   * @return self
   */
  public function __construct(DomainSelector $domain) {
    parent::__construct();
    $this->domain = $domain;
  }
}

/**
 * CountObjects request class
 * Count number of objects.
 */
class CountObjectsRequest extends SoapRequest {

  /**
   * Object types
   *
   * @var array
   */
  private static $_types = array(
    'userAccount',
    'account',
    'alias',
    'dl',
    'domain',
    'cos',
    'server',
    'calresource',
    'accountOnUCService',
    'cosOnUCService',
    'domainOnUCService',
    'internalUserAccount',
    'internalArchivingAccount'
  );

  /**
   * Constructor method for CountObjectsRequest
   * @param string $type Object type
   * @param DomainSelector $domain Domain
   * @return self
   */
  public function __construct($type, DomainSelector $domain = NULL) {
    parent::__construct();
    $this->type = in_array($type, self::$_types) ? $type : 'account';
    if ($domain instanceof DomainSelector) {
      $this->domain = $domain;
    }
  }

  /**
   * Returns the array representation of this class 
   *
   * @param  string $name
   * @return array
   */
  public function toArray($name = NULL) {
    $this->type = in_array($this->type, self::$_types) ? $this->type : 'account';
    return parent::toArray($name);
  }
}

/**
 * CreateAccount request class
 * Create account.
 */
class CreateAccountRequest extends SoapRequest {

  /**
   * Constructor method for CreateAccountRequest
   * @param string $name
   *   New account's name. Must include domain (uid@name), and domain specified in name must exist
   * @param string $password
   *   New account's password
   * @param array  $attrs
   * @return self
   */
  public function __construct($name, $password, array $attrs = array()) {
    parent::__construct();
    $this->name = trim($name);
    $this->password = trim($password);
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}

/**
 * CreateCos request class
 * Create class of service.
 */
class CreateCosRequest extends SoapRequest {

  /**
   * Constructor method for CreateCosRequest
   * @param string $name The name
   * @param array  $attrs
   * @return self
   */
  public function __construct($name, array $attrs = array()) {
    parent::__construct();
    $this->name = array(array('_content' => trim($name)));
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}

/**
 * CreateDistributionList request class
 * Create a distribution list.
 */
class CreateDistributionListRequest extends SoapRequest {

  /**
   * Constructor method for CreateDistributionListRequest
   * @param string $name
   *   Name for distribution list
   * @param bool   $dynamic
   *   If 1 (true) then create a dynamic distribution list
   * @param array  $attrs
   * @return self
   */
  public function __construct($name, $dynamic = NULL, array $attrs = array()) {
    parent::__construct();
    $this->name = trim($name);
    if (NULL !== $dynamic) {
      $this->dynamic = (bool) $dynamic;
    }
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}

/**
 * CreateDomain request class
 * Create domain.
 */
class CreateDomainRequest extends SoapRequest {

  /**
   * Constructor method for CreateDomainRequest
   * @param string $name The name
   * @param array  $attrs
   * @return self
   */
  public function __construct($name, array $attrs = array()) {
    parent::__construct();
    $this->name = trim($name);
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}

/**
 * CreateLDAPEntry request class
 * Create an LDAP entry.
 */
class CreateLDAPEntryRequest extends SoapRequest {

  /**
   * Constructor method for CreateLDAPEntry
   * @param string $dn
   *   A valid LDAP DN String (RFC 2253) that describes the new DN to create
   * @param array  $attrs
   * @return self
   */
  public function __construct($dn, array $attrs = array()) {
    parent::__construct();
    $this->dn = trim($dn);
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}

/**
 * DelegateAuth request class
 * Used to request a new auth token that is valid for the specified account.
 * The id of the auth token will be the id of the target account,
 * and the requesting admin's id will be stored in the auth token for auditing purposes.
 */
class DelegateAuthRequest extends SoapRequest {

  /**
   * Constructor method for DelegateAuthRequest
   * @param AccountSelector $account
   *   Details of target account
   * @param int $duration
   *    Lifetime in seconds of the newly-created authtoken. Defaults to 1 hour.
   *    Can't be longer then zimbraAuthTokenLifetime.
   * @return self
   */
  public function __construct(AccountSelector $account, $duration = NULL) {
    parent::__construct();
    $this->account = $account;
    if (NULL !== $duration) {
      $this->duration = (int) $duration;
    }
  }
}

/**
 * DeleteAccount request class
 * Deletes the account with the given id.
 */
class DeleteAccountRequest extends SoapRequest {

  /**
   * Constructor method for DeleteAccountRequest
   * @param string $id Zimbra ID
   * @return self
   */
  public function __construct($id) {
    parent::__construct();
    $this->id = trim($id);
  }
}

/**
 * DeleteCos request class
 * Delete a Class of Service (COS).
 */
class DeleteCosRequest extends SoapRequest {

  /**
   * Constructor method for DeleteCosRequest
   * @param string $id Zimbra ID
   * @return self
   */
  public function __construct($id) {
    parent::__construct();
    $this->id = trim($id);
  }
}

/**
 * DeleteDistributionList request class
 * Delete a distribution list.
 */
class DeleteDistributionListRequest extends SoapRequest {

  /**
   * Constructor method for DeleteDistributionListRequest
   * @param string $id Zimbra ID
   * @return self
   */
  public function __construct($id) {
    parent::__construct();
    $this->id = trim($id);
  }
}

/**
 * DeleteDomain request class
 * Delete a domain.
 */
class DeleteDomainRequest extends SoapRequest {

  /**
   * Constructor method for DeleteDomainRequest
   * @param string $id Zimbra ID
   * @return self
   */
  public function __construct($id) {
    parent::__construct();
    $this->id = trim($id);
  }
}

/**
 * DeleteLDAPEntry request class
 * Delete an LDAP entry.
 */
class DeleteLDAPEntryRequest extends SoapRequest {

  /**
   * Constructor method for DeleteLDAPEntryRequest
   * @param  string $dn A valdn LDAP DN String (RFC 2253) that describes the DN to delete
   * @return self
   */
  public function __construct($dn) {
    parent::__construct();
    $this->dn = trim($dn);
  }
}

/**
 * GetAccount request class
 * Get attributes related to an account.
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

/**
 * GetAccountInfo request class
 * Get information about an account.
 */
class GetAccountInfoRequest extends SoapRequest {

  /**
   * Constructor method for GetAccountInfoRequest
   * @param  AccountSelector $account Account selector
   * @return self
   */
  public function __construct(AccountSelector $account) {
    parent::__construct();
    $this->account = $account;
  }
}

/**
 * GetAllAccounts request class
 * Get All servers matching the selectin criteria.
 */
class GetAllAccountsRequest extends SoapRequest {

  /**
   * Constructor method for GetAllAccountsRequest
   * @param  ServerSelector $server
   * @param  DomainSelector $domain
   * @return self
   */
  public function __construct(ServerSelector $server = NULL, DomainSelector $domain = NULL) {
    parent::__construct();
    if ($server instanceof ServerSelector) {
      $this->server = $server;
    }
    if ($domain instanceof DomainSelector) {
      $this->domain = $domain;
    }
  }
}

/**
 * GetAllCos request class
 * Get all classes of service (COS).
 */
class GetAllCosRequest extends SoapRequest {
}

/**
 * GetAllDistributionLists request class
 * Get all distribution lists that match the selection criteria.
 */
class GetAllDistributionListsRequest extends SoapRequest {

  /**
   * Constructor method for GetAllDistributionListsRequest
   * @param  DomainSelector $domain
   * @return self
   */
  public function __construct(DomainSelector $domain = NULL) {
    parent::__construct();
    if ($domain instanceof DomainSelector) {
        $this->domain = $domain;
    }
  }
}

/**
 * GetAllDomains request class
 * Get all domains.
 */
class GetAllDomainsRequest extends SoapRequest {

  /**
   * Constructor method for GetAllDomainsRequest
   * @param  bool $applyConfig
   *   Apply config flag
   * @return self
   */
  public function __construct($applyConfig = NULL) {
    parent::__construct();
    if (NULL !== $applyConfig) {
      $this->applyConfig = (bool) $applyConfig;
    }
  }
}

/**
 * GetCos request class
 * Get Class Of Service (COS).
 */
class GetCosRequest extends SoapRequest {

  /**
   * Constructor method for GetCosRequest
   * @param  Cos $cos
   *   Specify Class Of Service (COS)
   * @param  string $attrs
   *   Comma separated list of attributes
   * @return self
   */
  public function __construct(CosSelector $cos = NULL, $attrs = NULL) {
    parent::__construct();
    if ($cos instanceof CosSelector) {
        $this->cos = $cos;
    }
    if (NULL !== $attrs) {
        $this->attrs = trim($attrs);
    }
  }
}

/**
 * GetDistributionList request class
 * Get a Distribution List.
 */
class GetDistributionListRequest extends SoapRequest {

  /**
   * Constructor method for GetDistributionListRequest
   * @param  DistributionListSelector $dl
   *   Distribution List
   * @param  int $limit
   *   The maximum number of dls to return (0 is default and means all)
   * @param  int $offset
   *   The starting offset (0, 25 etc)
   * @param  bool $sortAscending
   *   Flag whether to sort in ascending order 1 (true) is the default
   * @param  array $attrs
   * @return self
   */
  public function __construct(
    DistributionListSelector $dl = NULL,
    $limit = NULL,
    $offset = NULL,
    $sortAscending = NULL,
    array $attrs = array()
  ) {
    parent::__construct();
    if ($dl instanceof DistributionListSelector) {
      $this->dl = $dl;
    }
    if (NULL !== $limit) {
      $this->limit = (int) $limit;
    }
    if (NULL !== $offset) {
      $this->offset = (int) $offset;
    }
    if (NULL !== $sortAscending) {
      $this->sortAscending = (bool) $sortAscending;
    }
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}

/**
 * GetDomain request class
 * Get information about a domain.
 */
class GetDomainRequest extends SoapRequest {

  /**
   * Constructor method for GetDomainRequest
   * @param  DomainSelector $domain
   *   Specify the domain.
   * @param  bool $applyConfig
   *   Apply config flag
   * @param  string $attrs
   *   Comma separated list of attributes
   * @return self
   */
  public function __construct(DomainSelector $domain = NULL, $applyConfig = NULL, $attrs = NULL) {
    parent::__construct();
    if ($domain instanceof DomainSelector) {
      $this->domain = $domain;
    }
    if (NULL !== $applyConfig) {
      $this->applyConfig = (bool) $applyConfig;
    }
    if (NULL !== $attrs) {
      $this->attrs = trim($attrs);
    }
  }
}

/**
 * GetDomainInfo request class
 * Get domain information.
 */
class GetDomainInfoRequest extends SoapRequest {

  /**
   * Constructor method for GetDomainInfoRequest
   * @param  DomainSelector $account
   *   Specify the domain.
   * @param  bool $applyConfig
   *   Apply config flag
   * @return self
   */
  public function __construct(DomainSelector $domain = NULL, $applyConfig = NULL) {
    parent::__construct();
    if ($domain instanceof DomainSelector) {
      $this->domain = $domain;
    }
    if (NULL !== $applyConfig) {
      $this->applyConfig = (bool) $applyConfig;
    }
  }
}

/**
 * GetLDAPEntries request class
 * Get LDAP entries
 */
class GetLDAPEntriesRequest extends SoapRequest {

  /**
   * Constructor method for GetLDAPEntriesRequest
   * @param string $query
   *   Query string. Should be an LDAP-style filter string (RFC 2254)
   * @param string $ldapSearchBase
   *   LDAP search base. An LDAP-style filter string that defines an LDAP search base (RFC 2254)
   * @param string $sortBy
   *   Name of attribute to sort on. default is NULL
   * @param bool $sortAscending
   *   Flag whether to sort in ascending order 1 (true) is default
   * @param int $limit
   *   Limit - the maximum number of LDAP objects (records) to return (0 is default and means all)
   * @param int $offset
   *   The starting offset (0, 25, etc)
   * @return self
   */
  public function __construct(
    $query,
    $ldapSearchBase,
    $sortBy = NULL,
    $sortAscending = NULL,
    $limit = NULL,
    $offset = NULL
  ) {
    parent::__construct();
    $this->query = trim($query);
    $this->ldapSearchBase = array(array('_content' => trim($ldapSearchBase)));
    if (NULL !== $sortBy) {
      $this->sortBy = trim($sortBy);
    }
    if (NULL !== $sortAscending) {
      $this->sortAscending = (bool) $sortAscending;
    }
    if (NULL !== $limit) {
        $this->limit = (int) $limit;
    }
    if (NULL !== $offset) {
      $this->offset = (int) $offset;
    }
  }
}

/**
 * GetVersionInfo request class
 * Get version information.
 */
class GetVersionInfoRequest extends SoapRequest {
}

/**
 * GetAllSkins request class
 * Get all installed skins on the server.
 */
class GetAllSkinsRequest extends SoapRequest {
}

/**
 * ModifyAccount request class
 * Modify an account.
 */
class ModifyAccountRequest extends SoapRequest {

  /**
   * Constructor method for ModifyAccountRequest
   * @param string $id
   *   Zimbra ID
   * @param array  $attrs
   * @return self
   */
  public function __construct($id, array $attrs = array()) {
    parent::__construct();
    $this->id = trim($id);
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}

/**
 * ModifyCos request class
 * Modify Class of Service (COS) attributes.
 */
class ModifyCosRequest extends SoapRequest {

  /**
   * Constructor method for ModifyCosRequest
   * @param string $id Zimbra ID
   * @param array  $attrs
   * @return self
   */
  public function __construct($id = NULL, array $attrs = array()) {
    parent::__construct();
    $this->id = array(array('_content' => trim($id)));
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}

/**
 * ModifyDistributionList request class
 * Modify attributes for a Distribution List.
 */
class ModifyDistributionListRequest extends SoapRequest {

  /**
   * Constructor method for ModifyDistributionListRequet
   * @param string $id Zimbra ID
   * @param array  $attrs
   * @return self
   */
  public function __construct($id, array $attrs = array()) {
    parent::__construct();
    $this->id = trim($id);
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}

/**
 * ModifyDomain request class
 * Modify attributes for a domain.
 */
class ModifyDomainRequest extends SoapRequest {

  /**
   * Constructor method for ModifyDomainRequest
   * @param string $id Zimbra ID
   * @param array  $attrs
   * @return self
   */
  public function __construct($id, array $attrs = array()) {
    parent::__construct();
    $this->id = trim($id);
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}

/**
 * ModifyLDAPEntry request class
 * Modify an LDAP Entry.
 */
class ModifyLDAPEntryRequest extends SoapRequest {

  /**
   * Constructor method for ModifyLDAPEntryRequest
   * @param string $dn
   *   A valid LDAP DN String (RFC 2253) that identifies the LDAP object
   * @param array  $attrs
   * @return self
   */
  public function __construct($dn, array $attrs = array()) {
    parent::__construct();
    $this->dn = trim($dn);
    if (!empty($attrs)) {
      $this->a = $attrs;
    }
  }
}

/**
 * RemoveAccountAlias request class
 * Remove Account Alias.
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

/**
 * RemoveDistributionListMember request class
 * Remove Distribution List Member
 * Unlike add, remove of a non-existent member causes an exception and no modification to the list.
 */
class RemoveDistributionListMemberRequest extends SoapRequest {

  /**
   * Constructor method for RemoveDistributionListMemberRequest
   * @param  string $id Zimbra ID
   * @param  array  $dlm Members
   * @return self
   */
  public function __construct($id, array $dlm) {
    parent::__construct();
    $this->id = trim($id);
    if (!empty($dlm)) {
      $this->dlm = array();
      foreach ($dlm as $member) {
        $this->dlm[] = array('_content' => $member);
      }
    }
  }
}

/**
 * RenameAccount request class
 * Rename Account.
 */
class RenameAccountRequest extends SoapRequest {

  /**
   * Constructor method for RenameAccountRequest
   * @param string $id Zimbra ID
   * @param string $newName New account name
   * @return self
   */
  public function __construct($id, $newName) {
    parent::__construct();
    $this->id = trim($id);
    $this->newName = trim($newName);
  }
}

/**
 * RenameCos request class
 * Rename Class of Service (COS).
 */
class RenameCosRequest extends SoapRequest {

  /**
   * Constructor method for RenameCosRequest
   * @param string $id Zimbra ID
   * @param string $newName New COS name
   * @return self
   */
  public function __construct($id, $newName) {
    parent::__construct();
    $this->id = trim($id);
    $this->newName = trim($newName);
  }
}

/**
 * RenameCos request class
 * Rename Class of Service (COS).
 */
class RenameDistributionListRequest extends SoapRequest {

  /**
   * Constructor method for RenameDistributionListRequest
   * @param string $id Zimbra ID
   * @param string $newName New Distribution List name
   * @return self
   */
  public function __construct($id, $newName) {
    parent::__construct();
    $this->id = trim($id);
    $this->newName = trim($newName);
  }
}

/**
 * RenameLDAPEntry request class
 * Rename LDAP Entry.
 */
class RenameLDAPEntryRequest extends SoapRequest {

  /**
   * Constructor method for RenameLDAPEntryRequest
   * @param string $dn
   *   A valid LDAP DN String (RFC 2253) that identifies the LDAP object
   * @param string $new_dn
   *   New DN - a valid LDAP DN String (RFC 2253) that describes the new DN to be given to the LDAP object
   * @return self
   */
  public function __construct($dn, $new_dn) {
    parent::__construct();
    $this->dn = trim($dn);
    $this->new_dn = trim($new_dn);
  }
}

/**
 * SearchDirectory request class
 * Search directory.
 */
class SearchDirectoryRequest extends SoapRequest {

  /**
   * Constructor method for SearchDirectoryRequest
   * @param string $query
   *   Query string - should be an LDAP-style filter string (RFC 2254)
   * @param int $maxResults
   *   Maximum results that the backend will attempt to fetch from the directory before returning an account
   * @param int $limit
   *   The maximum number of accounts to return (0 is default and means all)
   * @param int $offset
   *   The starting offset (0, 25, etc)
   * @param string $domain
   *   The domain name to limit the search to.
   * @param bool $applyCos
   *   Flag whether or not to apply the COS policy to account.
   * @param bool $applyConfig
   *   Whether or not to apply the global config attrs to account.
   * @param array $types
   *   Comma-separated list of types to return.
   * @param string $sortBy
   *   Name of attribute to sort on. Default is the account name.
   * @param bool $sortAscending
   *   Whether to sort in ascending order. Default is 1 (true).
   * @param bool $countOnly
   *   Whether response should be count only. Default is 0 (false)
   * @param string $attrs
   *   Comma separated list of attributes
   * @return self
   */
  public function __construct(
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
    parent::__construct();
    $this->query = trim($query);
    if (NULL !== $maxResults) {
      $this->maxResults = (int) $maxResults;
    }
    if (NULL !== $limit) {
      $this->limit = (int) $limit;
    }
    if (NULL !== $offset) {
      $this->offset = (int) $offset;
    }
    if (NULL !== $domain) {
      $this->domain = trim($domain);
    }
    if (NULL !== $applyCos) {
      $this->applyCos = (bool) $applyCos;
    }
    if (NULL !== $applyConfig) {
      $this->applyConfig = (bool) $applyConfig;
    }
    if (NULL !== $types) {
      $this->types = trim($types);
    }
    if (NULL !== $sortBy) {
      $this->sortBy = trim($sortBy);
    }
    if (NULL !== $sortAscending) {
      $this->sortAscending = (bool) $sortAscending;
    }
    if (NULL !== $countOnly) {
      $this->countOnly = (bool) $countOnly;
    }
    if (NULL !== $attrs) {
      $this->attrs = trim($attrs);
    }
  }
}

/**
 * SetPassword request class
 * Set Password.
 */
class SetPasswordRequest extends SoapRequest {

  /**
   * Constructor method for SetPasswordRequest
   * @param string $id Zimbra ID
   * @param string $newPassword New Password
   * @return self
   */
  public function __construct($id, $newPassword) {
    parent::__construct();
    $this->id = trim($id);
    $this->newPassword = trim($newPassword);
  }
}

/**
 * Response class in Zimbra API PHP.
 */
class SoapResponse {

  /**
   * Soap response object
   * @var object
   */
  private $_response;

  /**
   * SoapResponse constructor
   *
   * @param  object $response
   *   Http response object
   * @return self
   */
  public function __construct($response) {
    $this->_processResponse($response);
  }

  /**
   * Returns a property value.
   * @param string $name the property name
   * @return mixed the property value
   * @throws Exception if the property not defined
   */
  public function __get($name) {
    if (isset($this->_response->$name)) {
      return $this->_response->$name;
    }
    else {
      throw new RuntimeException('Property ' . $name . ' is not defined.');
    }
  }

  /**
   * Checks if a property value is NULL.
   * @param string $name the property name
   * @return boolean
   */
  public function __isset($name) {
    return isset($this->_response->$name);
  }

  /**
   * Process soap response json.
   *
   * @param  object $response
   *   Http response object
   * @throws RuntimeException|UnexpectedValueException
   */
  private function _processResponse($response) {
    $object = isset($response->data) ? json_decode($response->data) : NULL;
    if ((int) $response->code == 200 && $object) {
      $body = $object->Body;
      $ref = new ReflectionObject($body);
      $props = $ref->getProperties(ReflectionProperty::IS_PUBLIC);
      $prop = reset($props);
      $name = ($prop instanceof ReflectionProperty) ? $prop->getName() : 'Response';
      $this->_response = isset($body->$name) ? $body->$name : NULL;
    }
    elseif ($object) {
      if (isset($object->Body->Fault)) {
        throw new RuntimeException($object->Body->Fault->Reason->Text, (int) $response->code);
      }
      else{
        throw new RuntimeException($response->error, (int) $response->code);
      }
    }
    else {
      if (isset($response->data)) {
        throw new UnexpectedValueException(
          'Error to parse reponse data: ' . $response->data, (int) $response->code
        );
      }
      else {
        throw new UnexpectedValueException($response->error, (int) $response->code);
      }
    }
  }
}

/**
 * A class representation soap message.
 */
class SoapMessage {

  /**
   * Soap headers
   * @var array
   */
  private $_headers = array();

  /**
   * Soap request
   * @var SoapRequest
   */
  private $_request;

  /**
   * The xml namespaces
   * @var array
   */
  private $_namespaces = array('urn:zimbra');

  /**
   * Sets request
   *
   * @param  SoapRequest $request
   * @return self
   */
  public function setRequest(SoapRequest $request) {
    $this->_request = $request;
    return $this;
  }

  /**
   * Get request
   *
   * @return SoapRequest
   */
  public function getRequest() {
    return $this->_request;
  }

  /**
   * Add header.
   *
   * @param  string|array $name
   * @param  string $value
   * @return self
   */
  public function addHeader($name, $value = NULL) {
    if (is_array($name)) {
      foreach ($name as $n => $v) {
        $this->addHeader($n, $v);
      }
    }
    else {
      $this->_headers[$name] = $value;
    }
    return $this;
  }

  /**
   * Get soap header.
   *
   * @param  string $name
   * @return string|array
   */
  public function header($name = NULL) {
    if (NULL === $name) {
      return $this->_headers;
    }
    else {
      return isset($this->_headers[$name]) ? $this->_headers[$name] : NULL;
    }
  }

  /**
   * Returns the json encoded string representation of this class 
   *
   * @return string
   */
  public function toJson(){
    $array = array();
    if (count($this->_headers)) {
      $array['Header'] = array(
        'context' => array(
          '_jsns' => 'urn:zimbra',
        ),
      );
      foreach ($this->_headers as $name => $value) {
        $array['Header']['context'][$name] = $value;
      }
    }
    if ($this->_request instanceof SoapRequest) {
      $reqArray = $this->_request->toArray();
      $reqName = $this->_request->className();
      $array['Body'][$reqName] = $reqArray[$reqName];
    }
    return json_encode((object) $array);
  }

  /**
   * Return a json string.
   *
   * @return string json string
   */
  public function __toString() {
    return trim($this->toJson());
  }
}

/**
 * This is a class which provides a http client for SOAP servers
 */
class HttpSoapClient {

  /**
   * Authentication token
   * @var string
   */
  private $_authToken;

  /**
   * Authentication session identify
   * @var string
   */
  private $_sessionId;

  /**
   * @var SoapMessage
   */
  private $_soapMessage;

  /**
   * Request headers
   * @var array
   */
  private $_headers = array();

  /**
   * Server location
   * @var string
   */
  private $_location;

  /**
   * Last request message
   * @var string
   */
  private $_request;

  /**
   * Last response message
   * @var string
   */
  private $_response;

  /**
   * Last response headers
   * @var string
   */
  private $_responseHeaders = array();

  /**
   * Enable debugging
   * @var string
   */
  private $_debug;

  /**
   * SoapClient constructor
   *
   * @param string $location  The URL to request.
   */
  public function __construct($location, $debug = FALSE) {
    $this->_location = $location;
    $this->_debug = (bool) $debug;
  }

  /**
   * Sets authentication token.
   *
   * @param  string $authToken Authentication token
   * @return self
   */
  public function setAuthToken($authToken) {
    $this->_authToken = trim($authToken);
    return $this;
  }

  /**
   * Gets authentication token.
   *
   * @return string
   */
  public function getAuthToken() {
    return $this->_authToken;
  }

  /**
   * Sets authentication session identify.
   *
   * @param  string $sessionId Authentication session identify
   * @return self
   */
  public function setSessionId($sessionId = NULL){
    $this->_sessionId = trim($sessionId);
    return $this;
  }

  /**
   * Gets authentication session identify.
   *
   * @return string
   */
  public function getSessionId() {
    return $this->_sessionId;
  }

  /**
   * Performs a SOAP request
   *
   * @param  SoapRequest $request
   * @return object Soap response
   */
  public function doRequest(SoapRequest $request) {
    $this->_soapMessage = new SoapMessage;
    if (!empty($this->_authToken)) {
      $this->_soapMessage->addHeader('authToken', $this->_authToken);
    }
    if (!empty($this->_sessionId)) {
      $this->_soapMessage->addHeader('sessionId', $this->_sessionId);
    }
    $this->_soapMessage->addHeader('format', 'js');
    $this->_soapMessage->setRequest($request);
    $this->_request = $this->_soapMessage->toJson();

    if ($this->_debug) {
      $name = 'zimbra - ' . $request->className();
      watchdog($name, htmlentities($this->_request), array(), WATCHDOG_DEBUG);
    }

    $response = $this->_doRequest($this->_request, array(
      'Content-Type' => 'application/soap+xml; charset=utf-8',
      'Method'       => 'POST',
      'User-Agent'   => isset($_SERVER['HTTP_USER_AGENT']) ? $_SERVER['HTTP_USER_AGENT'] : 'PHP-Zimbra-Soap-API',
      'SoapAction' => $request->getNamespace() . '#' . $request->className()
    ));
    if ($this->_debug) {
      $name = 'zimbra - ' . str_replace('Request', 'Response', $request->className());
      $data = isset($response->data) ? $response->data : '';
      watchdog($name, htmlentities($data), array(), WATCHDOG_DEBUG);
    }
    return new SoapResponse($response);
  }

  /**
   * Returns last SOAP request.
   *
   * @return mix The last SOAP request string.
   */
  public function lastRequest(){
    return $this->_request;
  }

  /**
   * Returns the SOAP headers from the last request.
   *
   * @return mix The last SOAP request headers.
   */
  function lastRequestHeaders(){
    return $this->_headers;
  }

  /**
   * Returns last SOAP response.
   *
   * @return mix The last SOAP response string.
   */
  public function lastResponse(){
    return $this->_response;
  }

  /**
   * Returns the SOAP headers from the last response.
   *
   * @return mix The last SOAP response headers.
   */
  public function lastResponseHeaders() {
    return $this->_responseHeaders;
  }

  /**
   * Performs SOAP request over HTTP.
   *
   * @param  string $request The SOAP request.
   * @param  string $headers The HTTP request header.
   * @return mixed
   */
  private function _doRequest($request, array $headers = array()) {
    $this->_headers = $headers;
    if (module_exists('chr')) {
      $response = chr_curl_http_request($this->_location, array(
        'headers' => $headers,
        'method' => 'POST',
        'data' => (string) $request,
      ));
    }
    else {
      $context = stream_context_create(array(
        'http' => array(
          'method'           => 'POST',
          'header'           => implode("\r\n", $headers),
          'protocol_version' => 1.1,
          'timeout'          => 30.0,
        ),
        'ssl' => array(
          'allow_self_signed' => TRUE,
          'verify_peer' => FALSE,
          'verify_peer_name' => FALSE,
        ),
      ));
      $response = backdrop_http_request($this->_location, array(
        'headers' => $headers,
        'method' => 'POST',
        'data' => (string) $request,
        'context' => $context,
      ));
    }
    $this->_responseHeaders = isset($response->headers) ? $response->headers : array();
    $this->_response = isset($response->data) ? $response->data : NULL;
    return $response;
  }
}
