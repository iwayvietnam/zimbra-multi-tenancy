<?php

namespace Drupal\zmt\Zimbra;

use GuzzleHttp\Psr7\Response as HttpResponse;
/**
 * Response class in Zimbra Soap API.
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
      throw new \RuntimeException('Property ' . $name . ' is not defined.');
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
  private function _processResponse(HttpResponse $response) {
    $object = json_decode($response->getBody());
    if ((int) $response->getStatusCode() == 200 && $object) {
      $body = $object->Body;
      $ref = new ReflectionObject($body);
      $props = $ref->getProperties(ReflectionProperty::IS_PUBLIC);
      $prop = reset($props);
      $name = ($prop instanceof ReflectionProperty) ? $prop->getName() : 'Response';
      $this->_response = isset($body->$name) ? $body->$name : NULL;
    }
    elseif ($object) {
      if (isset($object->Body->Fault)) {
        throw new \RuntimeException($object->Body->Fault->Reason->Text, (int) $response->getStatusCode());
      }
      else{
        throw new \RuntimeException($response->getReasonPhrase(), (int) $response->getStatusCode());
      }
    }
    else {
      if (isset($response->getBody())) {
        throw new \UnexpectedValueException(
          'Error to parse reponse data: ' . $response->getBody(), (int) $response->getStatusCode()
        );
      }
      else {
        throw new \UnexpectedValueException($response->getReasonPhrase(), (int) $response->getStatusCode());
      }
    }
  }
}
