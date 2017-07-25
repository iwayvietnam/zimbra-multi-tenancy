<?php

namespace Drupal\zmt\Zimbra\Request;

use Drupal\zmt\Zimbra\SoapRequest;

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
  public function __construct($id, array $dlm = []) {
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
