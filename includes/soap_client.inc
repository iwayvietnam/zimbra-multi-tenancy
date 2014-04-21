<?php
/**
 * Copyright © 2013 by iWay Vietnam. (http://www.iwayvietnam.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * soap_client is a class which provides a client for SOAP servers
 * @package   Soap
 * @category  Client
 * @author  Nguyen Van Nguyen - nguyennv@iwayvietnam.com
 * @copyright Copyright © 2013 by iWay Vietnam. (http://www.iwayvietnam.com)
 */
class soap_client {
  /**
   * Authentication token
   * @var string
   */
  protected $_auth_token;

  /**
   * Authentication identify
   * @var string
   */
  protected $_session_id;

  /**
   * Zimbra soap namespace
   * @var string
   */
  protected $_namespace = 'urn:zimbra';

  /**
  * Zimbra soap message
   * @var soap_message
   */
  protected $_soap_message;

  /**
   * Server location
   * @var string
   */
  protected $_location;

  /**
   * Last response message
   * @var string
   */
  protected $_response;

  /**
   * Response header string
   * @var string
   */
  protected $_response_header;

  /**
   * Request headers
   * @var array
   */
  protected $_headers = array();

  /**
   * Enable debugging
   * @var string
   */
  private $_debug;

  /**
   * soap_client constructor
   *
   * @param string $location  The URL to request.
   * @param string $namespace The SOAP namespace.
   * @param bool   $debug   Enable debugging.
   */
  public function __construct($location, $namespace = 'urn:zimbra', $debug = false) {
    $this->_location = $location;
    $this->_namespace = !empty($namespace) ? $namespace : 'urn:zimbra';
    $this->_debug = (bool) $debug;
    $this->_headers = array(
      'Content-Type' => 'text/xml; charset=utf-8',
      'Method'     => 'POST',
      'User-Agent'   => $_SERVER['HTTP_USER_AGENT'],
    );
    $this->_soap_message = new soap_message($this->_namespace);
  }

  /**
   * Method overloading.
   *
   * @param  string $name Method name
   * @param  array  $args Method arguments
   * @return mix
   */
  public function __call($name, array $args) {
    $params = $attrs = array();
    if (isset($args[0])) {
      $params = is_array($args[0]) ? $args[0] : array($args[0]);
    }
    if (isset($args[1])) {
      $attrs = is_array($args[1]) ? $args[1] : array($args[1]);
    }

    $result = $this->soap_request(ucfirst($name).'Request', $params, $attrs);
    $response = ucfirst($name).'Response';
    return $result->$response;
  }

  /**
   * Performs a SOAP request
   *
   * @param  string $name   The soap function.
   * @param  string $params The soap parameters.
   * @param  string $attrs  The soap attributes.
   * @return object Soap response
   */
  public function soap_request($name, array $params = array(), array $attrs = array()) {
    $this->_headers['SoapAction'] = $this->_soap_message->get_namespace().'#'.$name;

    $this->_soap_message->set_body($name, $attrs, $params);

    if ($this->_debug) {
      watchdog('zimbra '.$name, htmlentities($this->last_request()), array(), WATCHDOG_DEBUG);
    }
    $response = drupal_http_request($this->_location, array(
      'headers' => $this->_headers,
      'method' => 'POST',
      'data' => (string) $this->_soap_message,
    ));
    $this->_response_header = $response->headers;
    $this->_response = $response->data;
    if ($this->_debug) {
      watchdog('zimbra '.str_replace('Request', 'Response', $name), htmlentities($this->last_response()), array(), WATCHDOG_DEBUG);
    }
    return $this->_soap_message->process_response($this->_response);
  }

  /**
   * Set or get authentication token.
   *
   * @param  string $auth_token Authentication token
   * @return mix
   */
  public function auth_token($auth_token = null) {
    if ($auth_token === null) {
      return $this->_auth_token;
    }
    $this->_auth_token = (string) $auth_token;
    $this->_soap_message->add_header('authToken', $this->_auth_token);
    return $this;
  }

  /**
   * Set or get authentication session identify.
   *
   * @param  string $session_id Authentication session identify
   * @return mix
   */
  public function session_id($session_id = null) {
    if ($session_id === null) {
      return $this->_session_id;
    }
    $this->_session_id = (string) $session_id;
    $this->_soap_message->add_header('sessionId', $this->_session_id);
    return $this;
  }

  /**
   * Returns last SOAP request.
   *
   * @return mix The last SOAP request, as an XML string.
   */
  public function last_request() {
    return (string) $this->_soap_message;
  }

  /**
   * Returns the SOAP headers from the last request.
   *
   * @return array The last SOAP request headers.
   */
  function last_request_headers() {
    return $this->_headers;
  }

  /**
   * Returns last SOAP response.
   *
   * @return mix The last SOAP response, as an XML string.
   */
  public function last_response() {
    return $this->_response;
  }

  /**
   * Returns the SOAP headers from the last response.
   *
   * @return array The last SOAP response headers.
   */
  public function last_response_headers() {
    return $this->_response_header;
  }
}
