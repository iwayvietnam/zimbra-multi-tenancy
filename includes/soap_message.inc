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
 * soap_message class
 * @package   Soap
 * @category  Soap
 * @author  Nguyen Van Nguyen - nguyennv@iwayvietnam.com
 * @copyright Copyright © 2013 by iWay Vietnam. (http://www.iwayvietnam.com)
 */
class soap_message {
  /**
   * @var SimpleXMLElement
   */
  private $_xml;

  /**
   * @var SimpleXMLElement
   */
  private $_header;

  /**
   * @var string The xml namespace
   */
  private $_namespace;

  /**
   * soap_message constructor
   *
   * @param string $namespace The xml namespace.
   */
  public function __construct($namespace = 'urn:zimbra') {
    $this->_namespace = empty($namespace) ? 'urn:zimbra' : $namespace;
    if ($this->_namespace === 'urn:zimbra') {
      $message = '<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" xmlns:urn="urn:zimbra" />';
    } else {
      $message = '<env:Envelope xmlns:env="http://www.w3.org/2003/05/soap-envelope" '
        . 'xmlns:urn="urn:zimbra" '
        . 'xmlns:urn1="'.$this->_namespace.'" />';
    }
    $this->_xml = new SimpleXMLElement($message);
    $this->_header = $this->_xml->addChild('Header')
         ->addChild('context', null, 'urn:zimbra');
    $this->_xml->addChild('Body');
  }

  /**
   * Add header.
   *
   * @param  string $name  Header name.
   * @param  string $value Header value
   * @return self
   */
  public function add_header($name, $value) {
    if (isset($this->_header->$name)) {
      $this->_header->$name = $value;
    } else {
      $this->_header->addChild($name, self::bool_to_string($value));
    }
    return $this;
  }

  /**
   * Set soap body.
   *
   * @param  string $name   Soap function name.
   * @param  array  $attrs  Soap function attributes
   * @param  array  $params Soap function params
   * @return self
   */
  public function set_body($name, $attrs = array(), $params = array()) {
    unset($this->_xml->children('env', true)->Body);
    $body = $child = $this->_xml->addChild('Body');
    if(isset($params['_'])) {
      $body->addChild($name, self::bool_to_string((string) $params['_']), $this->_namespace);
    } else {
      $child = $body->addChild($name, null, $this->_namespace);
      $this->_process_params($child, $params);
    }

    foreach ($attrs as $key => $value) {
      if(self::is_valid_tag($key)) {
        $child->addAttribute($key, self::bool_to_string($value));
      }
    }
    return $this;
  }

  /**
   * Process soap response body.
   *
   * @param  string $response Soap response message.
   * @throws Exception
   * @return mix
   */
  public function process_response($response) {
    $fault = empty($response);
    if ($fault) {
      throw new Exception('Soap response is empty!');
    } else {
      $xml = @simplexml_load_string($response);
      if($xml instanceof SimpleXMLElement) {
        $fault = $xml->children('soap', true)->Body->Fault;
        if ($fault) {
          throw new Exception($fault->children('soap', true)->Reason->Text);
        }
      } else {
        throw new Exception('Error to load xml string!');
      }
      return self::xml_to_object($xml->children('soap', true)->Body);
    }
  }

  /**
   * Get namespace.
   *
   * @return string Namespace string
   */
  public function get_namespace() {
    return $this->_namespace;
  }

  /**
   * Return a well-formed XML string.
   *
   * @return string Xml string
   */
  public function __toString() {
    return trim($this->_xml->asXml());
  }

  /**
   * Process soap parameters.
   *
   * @param  SimpleXMLElement $xml  SimpleXMLElement object.
   * @param  array      $params Parametters.
   * @return void
   */
  private function _process_params(SimpleXMLElement $xml, array $params = array()) {
    foreach ($params as $name => $param) {
      if (is_array($param) && self::is_valid_tag($name)) {
        $text_value = null;
        if (isset($param['_'])) {
          $text_value = $param['_'];
          unset($param['_']);
        }

        if (is_numeric(key($param))) {
          foreach($param as $value) {
            if(is_array($value)) {
              $this->_process_params($xml, array($name => $value));
            } else {
              $xml->addChild($name, self::bool_to_string($value), $this->_namespace);
            }
          }
        } else {
          $child = $xml->addChild($name, self::bool_to_string($text_value), $this->_namespace);
          foreach ($param as $key => $value) {
            if (self::is_valid_tag($key)) {
              if (is_array($value)) {
                if (is_numeric(key($value))) {
                  foreach ($value as $k => $v) {
                    if (is_array($v)) {
                      $this->_process_params($child, array($key => $v));
                    } else {
                      $child->addChild($key, self::bool_to_string($v), $this->_namespace);
                    }
                  }
                } else {
                  $this->_process_params($child, array($key => $value));
                }
              } else {
                $child->addAttribute($key, self::bool_to_string($value));
              }
            }
          }
        }
      } else {
        if(self::is_valid_tag($name))
        {
          $xml->addChild($name, self::bool_to_string($param), $this->_namespace);
        }
      }
    }
  }

  /**
   * Convert SimpleXMLElement object to stdClass object.
   *
   * @param  SimpleXMLElement $xml The xml object.
   * @return object
   */
  public static function xml_to_object(SimpleXMLElement $xml) {
    $attributes = $xml->attributes();
    $children = $xml->children();
    $text_value = trim((string)$xml);
    if(count($attributes) === 0 && count($children) === 0) {
      return $text_value;
    } else {
      $object = new stdClass();
      foreach ($attributes as $key => $value) {
        $object->$key = (string)$value;
      }
      if (!empty($text_value)) {
        $object->_ = $text_value;
      }
      foreach($children as $value) {
        $name = $value->getName();
        if(isset($object->$name)) {
          if(is_array($object->$name)) {
            array_push($object->$name, self::xml_to_object($value));
          } else {
            $object->$name = array($object->$name, self::xml_to_object($value));
          }
        } else {
          $object->$name = self::xml_to_object($value);
        }
      }
      return $object;
    }
  }

  /**
   * Check the tag is valid.
   *
   * @param  string $tag The tag name.
   * @return bool
   */
  public static function is_valid_tag($tag) {
    $pattern = '/^[a-z_]+[a-z0-9\:\-\.\_]*[^:]*$/i';
    return preg_match($pattern, $tag, $matches) && $matches[0] == $tag;
  }

  /**
   * Convert bool value to string.
   *
   * @param  string $tag The tag name.
   * @return string
   */
  public static function bool_to_string($value) {
    $value = $value === true ? 'true' : $value;
    $value = $value === false ? 'false' : $value;
    return $value;
  }
}
