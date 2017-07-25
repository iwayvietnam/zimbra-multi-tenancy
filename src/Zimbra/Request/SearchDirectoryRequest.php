<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * SearchDirectoryRequest request class
 * Search directory
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
