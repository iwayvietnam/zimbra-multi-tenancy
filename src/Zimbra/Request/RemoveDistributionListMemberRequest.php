<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

/**
 * RemoveDistributionListMemberRequest request class
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
      $this->dlm = [];
      foreach ($dlm as $member) {
        $this->dlm[] = ['_content' => $member];
      }
    }
  }
}
