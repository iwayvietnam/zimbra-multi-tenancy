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
 * The SoapClient class provides a client for SOAP 1.1, SOAP 1.2 servers.
 * It can be used in WSDL.
 * @package   WSDL
 * @category  Client
 * @author    Nguyen Van Nguyen - nguyennv@iwayvietnam.com
 * @copyright Copyright © 2013 by iWay Vietnam. (http://www.iwayvietnam.com)
 */
class wsdl_client
{
	/**
	 * Soap namespace
	 * @var string
	 */
	protected $_namespace = 'urn:zimbra';

	/**
	 * Authentication headers
	 * @var array
	 */
	private $_headers = array();

	/**
	 * Filter callbacks
	 * @var array
	 */
	private $_filters = array();

	/**
	 * wsdl_client constructor
	 *
	 * @param string $location  The URL to request.
	 * @param string $namespace The SOAP namespace.
	 */
	public function __construct($location, $namespace = 'urn:zimbra')
	{
		$this->_namespace = $namespace;
		$options = array(
			'trace' => 1,
			'exceptions' => 1,
			'soap_version' => SOAP_1_2,
			'user_agent' => $_SERVER['HTTP_USER_AGENT'],
			'cache_wsdl' => WSDL_CACHE_DISK,
		);
		parent::__construct($location, $options);
	}

	/**
	 * Performs SOAP request over HTTP.
	 * This method can be overridden in subclasses to implement different transport layers, perform additional XML processing or other purpose.
	 *
	 * @param  string $request  The XML SOAP request.
	 * @param  string $location The URL to request.
	 * @param  string $action   The SOAP action.
	 * @param  int    $version  The SOAP version.
	 * @param  int    $one_way  If one_way is set to 1, this method returns nothing. Use this where a response is not expected.
	 * @return mixed
	 */
	public function __doRequest($request, $location, $action, $version, $one_way = 0)
	{
		$request = $this->_filter_request($request);
		if ($this->_filters)
		{
			foreach ($this->_filters as $callback)
			{
				$request = call_user_func($callback, $request, $location, $action, $version, $one_way);
			}
		}

		$this->__last_request = $request;
		return parent::__doRequest($request, $location, $action, $version, $one_way);
	}

	/**
	 * Method overloading.
	 *
	 * @param  string $name Method name
	 * @param  array  $args Method arguments
	 * @return mix
	 */
	public function __call($name, $args)
	{
		$request = ucfirst($name).'Request';
		$params = $attrs = array();
		if(isset($args[0]))
		{
			$params = is_array($args[0]) ? $args[0] : array($args[0]);
		}
		if(isset($args[1]))
		{
			$attrs = is_array($args[1]) ? $args[1] : array($args[1]);
		}
		return $this->soap_request($request, $params, $attrs);
	}

	/**
	 * Performs a SOAP request
	 *
	 * @param  string $name   The soap function.
	 * @param  string $params The soap parameters.
	 * @param  string $attrs  The soap attributes.
	 * @throws SoapFault
	 * @return object Soap response
	 */
	public function soap_request($name, array $params = array(), array $attrs = array())
	{
		$soap_header = $this->soap_header();
		if(isset($params['_']))
		{
			$parameters = array('parameters' => array('_' => (string) $params['_']) + $attrs);
		}
		else
		{
			$parameters = array('parameters' => $params + $attrs);
		}
		if($soap_header instanceof SoapHeader)
		{
			return $this->__soapCall($name, $parameters, NULL, $soap_header);
		}
		else
		{
			return $this->__soapCall($name, $parameters);
		}
	}

	public function soap_header()
	{
		$soap_header = NULL;
		if(count($this->_headers))
		{
			$soap_var = new SoapVar((object) $this->_headers, SOAP_ENC_OBJECT);
			$soap_header = new SoapHeader('urn:zimbra', 'context', $soap_var);
		}
		return $soap_header;
	}

	/**
	 * Filters to be run before request are sent.
	 *
	 * @param  array $callback Callback string, array, or closure
	 * @throws Exception
	 * @return self
	 */
	public function add_filter($callback)
	{
		if(!is_callable($callback))
		{
			throw new Exception('Invalid filter specified');
		}

		$this->_filters[] = $callback;

		return $this;
	}

	/**
	 * Set or get authentication token.
	 *
	 * @param  string $auth_token Authentication token
	 * @return self
	 */
	public function auth_token($auth_token = NULL)
	{
		if($auth_token === NULL)
		{
			return isset($this->_headers['authToken']) ? $this->_headers['authToken'] : NULL;
		}
		$this->_headers['authToken'] = (string) $auth_token;
		return $this;
	}

	/**
	 * Set or get authentication session identify.
	 *
	 * @param  string $session_id Authentication session identify
	 * @return self
	 */
	public function session_id($session_id = NULL)
	{
		if($session_id === NULL)
		{
			return isset($this->_headers['sessionId']) ? $this->_headers['sessionId'] : NULL;
		}
		$this->_headers['sessionId'] = (string) $session_id;
		return $this;
	}

	/**
	 * Returns last SOAP request.
	 *
	 * @return string The last SOAP request, as an XML string.
	 */
	public function last_request()
	{
		return $this->__getLastRequest();
	}

	/**
	 * Returns the SOAP headers from the last request.
	 *
	 * @return array The last SOAP request headers.
	 */
	public function last_request_headers()
	{
		return self::extract_headers($this->__getLastRequestHeaders());
	}

	/**
	 * Returns last SOAP response.
	 *
	 * @return string The last SOAP response, as an XML string.
	 */
	public function last_response()
	{
		return $this->__getLastResponse();
	}

	/**
	 * Returns the SOAP headers from the last response.
	 *
	 * @return array The last SOAP response headers.
	 */
	public function last_response_headers()
	{
		return self::extract_headers($this->__getLastResponseHeaders());
	}

	/**
	 * Filter soap request.
	 *
	 * @param  string $request The XML SOAP request.
	 * @return string The XML SOAP request.
	 */
	protected function _filter_request($request)
	{
		return $request;
	}

	/**
	 * Extract header string to array.
	 *
	 * @param  string $header_string Header string.
	 * @return array
	 */
	public static function extract_headers($header_string = '')
	{
		$parts = explode("\r\n", $header_string);
		$headers = array();
		foreach ($parts as $part)
		{
			$pos = strpos($part, ':');
			if($pos)
			{
				$name = trim(substr($part, 0, $pos));
				$value = trim(substr($part, ($pos + 1)));
				$headers[$name] = $value;
			}
		}
		return $headers;		
	}
}