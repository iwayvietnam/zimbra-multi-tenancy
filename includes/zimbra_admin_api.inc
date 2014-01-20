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
 * zimbra_admin_api is a class which allows to connect Zimbra API administration functions via SOAP
 * @package   Zimbra/Admin
 * @category  API
 * @author    Nguyen Van Nguyen - nguyennv@iwayvietnam.com
 * @copyright Copyright © 2013 by iWay Vietnam. (http://www.iwayvietnam.com)
 */
class zimbra_admin_api
{
    /**
     * The Zimbra api soap location
     * @var string
     */
    protected $_location = 'https://localhost/service/soap';

    /**
     * The user account
     * @var string 
     */
    protected $_account;

    /**
     * Zimbra api soap client
     * @var soap_client 
     */
    protected $_client;

    /**
     * The soap namespace
     * @var string
     */
    protected $_namespace = 'urn:zimbra';

    /**
     * zimbra_admin_api constructor
     *
     * @param string $location The Zimbra api soap location.
     */
    public function __construct($location, $client = 'soap')
    {
        $this->_location = $location;
        $this->_namespace = 'urn:zimbraAdmin';
        switch ($client)
        {
            case 'wsdl':
                $this->_client = new wsdl_client($location, $this->_namespace);
                break;
            default:
                $this->_client = new soap_client($location, $this->_namespace);
                break;
        }
    }

    /**
     * Add an alias for the account.
     * Access: domain admin sufficient.
     * Note: this request is by default proxied to the account's home server.
     *
     * @param  string $id    Value of zimbra identify.
     * @param  string $alias Account alias.
     * @return mix
     */
    public function addAccountAlias($id, $alias)
    {
        $options = array(
            'id' => $id,
            'alias' =>$alias,
        );
        return $this->_client->addAccountAlias(array(), $options);
    }

    /**
     * Changes logging settings on a per-account basis.
     * Adds a custom logger for the given account and log category.
     * The logger stays in effect only during the lifetime of the current server instance.
     * If the request is sent to a server other than the one that the account resides on,
     * it is proxied to the correct server.
     * If the category is "all", adds a custom logger for every category or the given user.
     *
     * @param  string $account  The user account.
     * @param  string $category Logger category.
     * @param  string $level    Level of the logging (error|warn|info|debug|trace).
     * @return mix
     */
    public function addAccountLogger($account, $category, $level = 'error')
    {
        $levels = array('error', 'warn', 'info', 'debug', 'trace');
        $level = in_array($level, $levels) ? $level : 'error';
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
            'logger' => array(
                'category' => $category,
                'level' => $level,
            ),
        );
        return $this->_client->addAccountLogger($params);
    }

    /**
     * Add an alias for a distribution list.
     * Access: domain admin sufficient.
     *
     * @param  string $id    Value of zimbra identify.
     * @param  string $alias Distribution list alias.
     * @return mix
     */
    public function addDistributionListAlias($id, $alias)
    {
        $options = array(
            'id' => $id,
            'alias' =>$alias,
        );
        return $this->_client->addDistributionListAlias(array(), $options);
    }

    /**
     * Adding members to a distribution list.
     * Access: domain admin sufficient.
     *
     * @param  string $id      Value of zimbra identify.
     * @param  array  $members Distribution list members.
     * @return mix
     */
    public function addDistributionListMember($id, array $members)
    {
        $params['dlm'] = array();
        foreach ($members as $member)
        {
            $params['dlm'][] = $member;
        }
        return $this->_client->addDistributionListMember($params, array('id' => $id,));
    }

    /**
     * Add a GalSync data source.
     * Access: domain admin sufficient.
     *
     * @param  string $name    Name of the data source.
     * @param  string $domain  Name of pre-existing domain.
     * @param  string $type    GalMode type (both|ldap|zimbra).
     * @param  string $account Account.
     * @param  string $folder  Contact folder name.
     * @param  array  $attrs   Attributes.
     * @return mix
     */
    public function addGalSyncDataSource($name, $domain, $type, $account, $folder = '', array $attrs = array())
    {
        $types = array('both', 'ldap', 'zimbra');
        $type = in_array($type, $types) ? $type : 'zimbra';
        $options = array(
            'name' => $name,
            'domain' => $domain,
            'type' => $type,
        );
        if(!empty($folder)) $options['folder'] = $folder;

        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );

        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;         
        }
        return $this->_client->addGalSyncDataSource($params, $options);
    }

    /**
     * Create a waitset to listen for changes on one or more accounts.
     * Called once to initialize a WaitSet and to set its "default interest types".
     * WaitSet: scalable mechanism for listening for changes to one or more accounts.
     * Interest types:
     *   f. folders
     *   m. messages
     *   c. contacts
     *   a. appointments
     *   t. tasks
     *   d. documents
     *   all. all types (equiv to "f,m,c,a,t,d")
     *
     * @param  array  $add   Waitset.
     * @param  string $types Default interest types.
     * @param  bool   $all   If all is set, then all mailboxes on the system will be listened to, including any mailboxes which are created on the system while the WaitSet is in existence.
     * @return mix
     */
    public function adminCreateWaitSet(array $add, array $types = array(), $all = FALSE)
    {
        $defTypes = $this->_defTypes($types);
        $options = array(
            'defTypes' => empty($defTypes) ? 'all' : $defTypes,
            'allAccounts' => ($all === FALSE OR $all === 0) ? 0 : 1,
        );

        $params['add'] = array();
        $waitSets = $this->_waitSets($add);
        if(count($waitSets))
        {
            $params['add']['a'] = $waitSets;            
        }
        return $this->_client->adminCreateWaitSet($params, $options);
    }


    /**
     * Use this to close out the waitset.
     * Note that the server will automatically time out a wait set
     * if there is no reference to it for (default of) 20 minutes.
     * WaitSet: scalable mechanism for listening for changes to one or more accounts.
     *
     * @param  string $waitSet Waitset identify.
     * @return mix
     */
    public function adminDestroyWaitSet($waitSet)
    {
        return $this->_client->adminDestroyWaitSet(array(), array('waitSet' => $waitSet));
    }

    /**
     * AdminWaitSetRequest optionally modifies the wait set and checks for any notifications.
     * If block=1 and there are no notifications, then this API will BLOCK until there is data.
     * Interest types:
     *   f. folders
     *   m. messages
     *   c. contacts
     *   a. appointments
     *   t. tasks
     *   d. documents
     *   all. all types (equiv to "f,m,c,a,t,d")
     *
     * @param  string  $waitSet Waitset identify.
     * @param  string  $seq     Last known sequence number.
     * @param  array   $add     Waitset to add.
     * @param  array   $update  Waitset to update.
     * @param  array   $remove  Waitset to remove.
     * @param  array   $types   Default interest types.
     * @param  bool    $block   Flag whether or not to block until some account has new data.
     * @param  integer $timeout Timeout length.
     * @return mix
     */
    public function adminWaitSet($waitSet, $seq, array $add = array(), array $update = array(), array $remove = array(), array $types = array('all'), $block = 0, $timeout = 0)
    {
        $options = array(
            'waitSet' => $waitSet,
            'seq' => $seq,
        );
        $defTypes = $this->_defTypes($types);
        if(!empty($defTypes)) $options['defTypes'] = $defTypes;
        if((int) $block > 0) $options['block'] = 1;
        if((int) $timeout > time()) $options['timeout'] = (int) $timeout;

        $params['add'] = array();
        $addWaitSets = $this->_waitSets($add);
        if(count($addWaitSets))
        {
            $params['add']['a'] = $addWaitSets;
        }

        $params['update'] = array();
        $updateWaitSets = $this->_waitSets($update);
        if(count($updateWaitSets))
        {
            $params['update']['a'] = $updateWaitSets;
        }

        $params['remove'] = array();
        $removeWaitSets = $this->_waitSets($remove);
        if(count($removeWaitSets))
        {
            $params['remove']['a'] = array();
            foreach ($remove as $key => $value)
            {
                $params['remove']['a'][] = array('id' => $value);
            }
        }
        return $this->_client->adminWaitSet($params, $options);
    }


    /**
     * Authenticate for an adminstration account
     *
     * @param  string $account     The user account.
     * @param  string $password    The user password.
     * @param  string $virtualHost Virtual-host is used to determine the domain of the account name
     * @return authentication token
     */
    public function auth($account, $password, $virtualHost = '')
    {
        $this->_account = $account;
        $params = array();
        if(!empty($virtualHost))
        {
            $params['virtualHost'] = $virtualHost;
        }

        $options = array(
            'name' => $account,
            'password' => $password,
        );
        $result = $this->_client->auth($params, $options);
        $authToken = $result->authToken;
        if($authToken) $this->_client->auth_token($authToken);
        return $result;
    }

    /**
     * Authenticate for an adminstration account
     *
     * @param  string $account     The adminstration account.
     * @param  string $token       The authentication token.
     * @param  string $virtualHost Virtual-host is used to determine the domain of the account name
     * @return authentication token
     */
    public function authByToken($account, $token, $virtualHost = '')
    {
        $this->_account = $account;
        $params = array(
            'authToken' => $token,
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        if(!empty($virtualHost))
        {
            $params['virtualHost'] = $virtualHost;
        }
        $result = $this->_client->auth($params);
        $authToken = $result->authToken;
        if($authToken) $this->_client->auth_token($authToken);
        return $result;
    }

    /**
     * Perform an autocomplete for a name against the Global Address List
     * Notes: admin verison of mail equiv. Used for testing via zmprov.
     * Type of addresses to auto-complete on:
     *   1. "account" for regular user accounts, aliases and distribution lists
     *   2. "resource" for calendar resources
     *   3. "group" for groups
     *   4. "all" for combination of types
     *
     * @param  string $domain The domain name.
     * @param  string $name   The name to test for autocompletion.
     * @param  string $type   Type of addresses to auto-complete on.
     * @param  string $acctID GAL Account ID.
     * @return mix
     */
    public function autoCompleteGal($domain, $name, $type = 'accounts', $acctID = '')
    {
        $options = array(
            'domain' => $domain,
            'name' => $name,
        );
        $types = array('all', 'account', 'resource', 'group');
        if(in_array($type, $types)) $options['type'] = $type;
        if(!empty($acctID)) $options['galAcctId'] = $acctID;

        return $this->_client->autoCompleteGal(array(), $options);
    }

    /**
     * Auto-provision an account
     *
     * @param  string $domain    The domain name.
     * @param  string $principal The name used to identify the principal.
     * @param  string $password  Password.
     * @return mix
     */
    public function autoProvAccount($domain, $principal, $password = '')
    {
        $params = array(
            'domain' => array(
                'by' => 'name',
                '_' => $domain,
            ),
            'principal' => array(
                'by' => 'name',
                '_' => $principal,
            ),
        );
        if(!empty($password)) $params['password'] = $password;

        return $this->_client->autoProvAccount($params);
    }

    /**
     * Auto-provision task control.
     * Under normal situations, the EAGER auto provisioning task(thread)
     * should be started/stopped automatically by the server when appropriate.
     * The task should be running when zimbraAutoProvPollingInterval is not 0
     * and zimbraAutoProvScheduledDomains is not empty.
     * The task should be stopped otherwise.
     * This API is to manually force start/stop or query status of the EAGER auto provisioning task.
     * It is only for diagnosis purpose and should not be used under normal situations.
     *
     * @param  string $action Action to perform - one of start|status|stop
     * @return mix
     */
    public function autoProvTaskControl($action)
    {
        $actions = array('start', 'status', 'stop');
        $options = array(
            'action' => in_array($action, $actions) ? $action : 'status',
        );
        return $this->_client->autoProvTaskControl(array(), $options);
    }

    /**
     * Check Auth Config.
     *
     * @param  string $name     Name.
     * @param  string $password Password.
     * @param  array  $attrs    Attributes.
     * @return mix
     */
    public function checkAuthConfig($name, $password, array $attrs = array())
    {
        $options = array(
            'name' => $name,
            'password' => $password,
        );

        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->checkAuthConfig($params, $options);
    }

    /**
     * Checks for items that have no blob, blobs that have no item,
     * and items that have an incorrect blob size stored in their metadata.
     * If no volumes are specified, all volumes are checked.
     * If no mailboxes are specified, all mailboxes are checked.
     * Blob sizes are checked by default.
     * Set checkSize to 0 (false) to * avoid the CPU overhead
     * of uncompressing compressed blobs in order to calculate size.
     *
     * @param  string $checkSize Check size.
     * @param  string $report    If set a complete list of all blobs used by the mailbox(es) is returned.
     * @param  array  $volumes   Volumes.
     * @param  array  $mboxes    Mailboxes.
     * @return mix
     */
    public function checkBlobConsistency($checkSize = 0, $report = 1, array $volumes = array(), array $mboxes = array())
    {
        $options = array(
            'checkSize' => ((int) $checkSize > 0) ? 1 : 0,
            'reportUsedBlobs' => ((int) $report > 0) ? 1 : 0,
        );
        $params = array();
        if(count($volumes))
        {
            $params['volume'] = array();
            foreach ($volumes as $volume)
            {
                $params['volume'][] = array('id' => $volume);
            }
        }
        if(count($mboxes))
        {
            $params['mbox'] = array();
            foreach ($mboxes as $mbox)
            {
                $params['mbox'][] = array('id' => $mbox);
            }
        }
        return $this->_client->checkBlobConsistency($params, $options);
    }

    /**
     * Check existence of one or more directories and optionally create them.
     *
     * @param  array $directories Directories.
     * @return mix
     */
    public function checkDirectory(array $directories = array())
    {
        $params = array();
        if(count($directories))
        {
            $params['directory'] = array();
            foreach ($directories as $dir)
            {
                $arr = array();
                $arr['path'] = isset($dir['path']) ? $dir['path'] : '';
                $arr['create'] = (isset($dir['create']) && ((int) $dir['create'] > 0)) ? 1 : 0;
                $params['directory'][] = $dir;
            }
        }

        return $this->_client->checkDirectory($params);
    }

    /**
     * Check Domain MX record.
     *
     * @param  string $domain The name used to identify the domain.
     * @return mix
     */
    public function checkDomainMXRecord($domain)
    {
        $params = array(
            'domain' => array(
                'by' => 'name',
                '_' => $domain,
            ),
        );
        return $this->_client->checkDomainMXRecord($params);
    }

    /**
     * Check Exchange Authorisation.
     *
     * @param  string $url URL to Exchange server.
     * @param  string $user Exchange user.
     * @param  string $pass Exchange password.
     * @param  string $scheme Auth scheme (basic|form).
     * @return mix
     */
    public function checkExchangeAuth($url, $user, $pass, $scheme = 'basic')
    {
        $scheme = in_array($scheme, array('basic', 'form')) ? $scheme : 'basic';
        $params = array(
            'auth' => array(
                'url' => $url,
                'user' => $user,
                'pass' => $pass,
                'scheme' => $scheme,
            ),
        );
        return $this->_client->checkExchangeAuth($params);
    }

    /**
     * Check Global Addressbook Configuration .
     * Notes:
     *   1. zimbraGalMode must be set to ldap, even if you eventually want to set it to "both".
     *   2. <action> is optional. GAL-action can be autocomplete|search|sync. Default is search.
     *   3. <query> is ignored if <action> is "sync".
     *   4. AuthMech can be none|simple|kerberos5.
     *      - Default is simple if both BindDn/BindPassword are provided.
     *      - Default is none if either BindDn or BindPassword are NOT provided.
     *   5. BindDn/BindPassword are required if AuthMech is "simple".
     *   6. Kerberos5Principal/Kerberos5Keytab are required only if AuthMech is "kerberos5".
     *   7. zimbraGalSyncLdapXXX attributes are for GAL sync. They are ignored if <action> is not sync. 
     *      For GAL sync, if a zimbraGalSyncLdapXXX attribute is not set,
     *      server will fallback to the corresponding zimbraGalLdapXXX attribute.
     *
     * @param  string  $query  Description for element text content.
     * @param  string  $action Action (autocomplete|search|sync).
     * @param  integer $limit  Limit. Default value 10
     * @param  array   $attrs  Attributes.
     * @return mix
     */
    public function checkGalConfig($query, $action = 'search', $limit = 10, array $attrs = array())
    {
        $acctions = array('autocomplete', 'search', 'sync');
        $params = array(
            'action' => in_array($action, $acctions) ? (string) $action : 'search',
            'query' => array(
                'limit' => ((int) $limit > 0) ? $limit : 10,
                '_' => $query,
            ),
        );
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }
        return $this->_client->checkGalConfig($params);
    }

    /**
     * Check Health.
     *
     * @return mix
     */
    public function checkHealth()
    {
        return $this->_client->checkHealth();
    }

    /**
     * Check whether a hostname can be resolved.
     *
     * @param  string $hostname Hostname.
     * @return mix
     */
    public function checkHostnameResolve($hostname = '')
    {
        $options = array();
        if(!empty($hostname)) $options['hostname'] = $hostname;
        return $this->_client->checkHostnameResolve(array(), $options);
    }

    /**
     * Check password strength.
     * Access: domain admin sufficient.
     * Note: this request is by default proxied to the account's home server
     *
     * @param  string $id       Zimbra identify.
     * @param  string $password Passowrd to check.
     * @return mix
     */
    public function checkPasswordStrength($id, $password)
    {
        $options = array(
            'id' => $id,
            'password' => $password,
        );
        return $this->_client->checkPasswordStrength($options);
    }

    /**
     * Check if a principal has the specified right on target. 
     * A successful return means the principal specified by the <grantee>
     * is allowed for the specified right on the * target object. 
     * Note: this request is by default proxied to the account's home server
     *
     * @param  string $right   Name of right.
     * @param  string $target  The name used to identify the target.
     * @param  string $type    Target type. Valid values: (account|calresource|cos|dl|group|domain|server|ucservice|xmppcomponent|zimlet|config|global).
     * @param  array  $grantee Grantee.
     * @param  array  $attrs   Attributes.
     * @return mix
     */
    public function checkRights($right, $target, $type, array $grantee, array $attrs = array())
    {
        $params = array(
            'right' => $right,
            'target' => array(
                'by' => 'name',
                'type' => in_array($type, $this->_resourceTypes) ? $type : 'account',
                '_' => $target,
            ),
            'grantee' => array(),
        );

        if(isset($grantee['_']))
            $params['grantee']['_'] = $grantee['_'];
        $gTypes = array('usr', 'grp', 'egp', 'all', 'dom', 'gst', 'key', 'pub', 'email');
        if(isset($grantee['type']) AND in_array($grantee['type'], $gTypes))
            $params['grantee']['type'] = $grantee['type'];
        if(isset($grantee['by']) AND in_array($grantee['by'], array('id', 'name')))
            $params['grantee']['by'] = $grantee['by'];
        if(isset($grantee['all']))
            $params['grantee']['all'] = ((int) $grantee['all'] > 0) ? 1 : 0;

        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->checkRights($params);
    }

    /**
     * Clear cookie.
     *
     * @param  array $cookies Specifies cookies to clean.
     * @return mix
     */
    public function clearCookie(array $cookies = array())
    {
        $params = array();
        if(count($cookies))
        {
            $params['cookie'] = array();
            foreach ($cookies as $cookie)
            {
                $params['cookie'][] = array('name' => $cookie);
            }
        }
        return $this->_client->clearCookie($params);
    }

    /**
     * Compact index.
     * Access: domain admin sufficient.
     * Note: this request is by default proxied to the account's home server.
     *
     * @param  string $id     Account identify.
     * @param  string $action Action to perform (start|status).
     * @return mix
     */
    public function compactIndex($id, $action = 'status')
    {
        $options = array(
            'action' => in_array($action, array('start', 'status')) ? $action : 'status',
        );
        $params = array(
            'mbox' => array(
                'id' => $id,
            ),
        );
        return $this->_client->compactIndex($params, $options);
    }

    /**
     * Computes the aggregate quota usage for all domains in the system.
     * The request handler issues GetAggregateQuotaUsageOnServerRequest
     * to all mailbox servers and computes the aggregate quota used by each domain.
     * The request handler updates the zimbraAggregateQuotaLastUsage domain attribute
     * and sends out warning messages for each domain having quota usage greater than a defined percentage threshold.
     *
     * @return mix
     */
    public function computeAggregateQuotaUsage()
    {
        return $this->_client->computeAggregateQuotaUsage();
    }

    /**
     * Configure Zimlet.
     *
     * @param  string $aid Attachment identify.
     * @return mix
     */
    public function configureZimlet($aid)
    {
        $params = array(
            'content' => array(
                'aid' => $aid,
            ),
        );
        return $this->_client->configureZimlet($params);
    }

    /**
     * Copy Class of service (COS).
     *
     * @param  string $name Destination name for COS.
     * @param  string $cos  Source COS.
     * @return mix
     */
    public function copyCos($name, $cos)
    {
        $params = array(
            'name' => $name,
            'cos' => array(
                'by' => 'name',
                '_' => $cos,
            ),
        );
        return $this->_client->copyCos($params);
    }

    /**
     * Count number of accounts by cos in a domain.
     * Note: It doesn't include any account with zimbraIsSystemResource=TRUE,
     *       nor does it include any calendar resources.
     *
     * @param  string $domain The name used to identify the domain.
     * @return mix
     */
    public function countAccount($domain = '')
    {
        $params = array();
        if(!empty($domain))
        {
            $params = array(
                'domain' => array(
                    'by' => 'name',
                    '_' => $domain,
                ),
            );          
        }
        return $this->_client->countAccount($params);
    }

    /**
     * Count number of objects. 
     * Returns number of objects of requested type.
     * Note: For account/alias/dl, if a domain is specified,
     *       only entries on the specified domain are counted.
     *       If no domain is specified, entries on all domains are counted.
     *       For accountOnUCService/cosOnUCService/domainOnUCService,
     *       UCService is required, and domain cannot be specified.
     *
     * @param  string $domain    The name used to identify the domain.
     * @param  string $type      Object type. Valid values: (userAccount|account|alias|dl|domain|cos|server|calresource|accountOnUCService|cosOnUCService|domainOnUCService|internalUserAccount|internalArchivingAccount).
     * @param  string $ucservice Key for choosing ucservice.
     * @return mix
     */
    public function countObjects($domain = '', $type = 'account', $ucservice = '')
    {
        $options = array(
            'type' => in_array($type, $this->_objectTypes) ? $type : 'account',
        );

        $params = array();
        if(!empty($domain))
        {
            $params['domain'] = array(
                'by' => 'name',
                '_' =>$domain,
            );
        }
        if(!empty($ucservice))
        {
            $params['ucservice'] = array(
                'by' => 'name',
                '_' =>$domain,
            );
        }

        return $this->_client->countObjects($params, $options);
    }

    /**
     * Create account.
     * Notes:
     *   1. accounts without passwords can't be logged into.
     *   2. name must include domain (uid@name), and domain specified in name must exist.
     *   3. default value for zimbraAccountStatus is "active".
     * Access: domain admin sufficient.
     *
     * @param  string $name     New account's name. Must include domain (uid@name), and domain specified in name must exist.
     * @param  string $password New account's password.
     * @param  array  $attrs    Attributes.
     * @return mix
     */
    public function createAccount($name, $password, array $attrs = array())
    {
        $options = array(
            'name' => $name,
            'password' => $password,
        );

        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->createAccount($params, $options);
    }

    /**
     * Create a calendar resource.
     * Notes:
     *   1. A calendar resource is a special type of Account. The Create, Delete, Modify, Rename, Get, GetAll, and Search operations are very similar to those of Account.
     *   2. Must specify the displayName and zimbraCalResType attributes
     * Access: domain admin sufficient.
     *
     * @param  string $name     Name or calendar resource. Must include domain (uid@domain), and domain specified after @ must exist.
     * @param  string $password Password for calendar resource.
     * @param  array  $attrs    Attributes.
     * @return mix
     */
    public function createCalendarResource($name, $password = '', array $attrs = array())
    {
        $options = array(
            'name' => $name,
        );
        if(!empty($password)) $options['password'] = $password;

        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->createCalendarResource($params, $options);
    }

    /**
     * Create a Class of Service (COS).
     * Notes:
     *   1. Extra attrs: description, zimbraNotes.
     *
     * @param  string $name  COS name.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function createCos($name, array $attrs = array())
    {
        $params = array(
            'name' => $name,
        );
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }
        return $this->_client->createCos($params);
    }

    /**
     * Creates a data source that imports mail items into the specified folder.
     * Notes:
     *   1. Currently the only type supported is pop3.
     *   2. every attribute value is returned except password.
     *   3. this request is by default proxied to the account's home server.
     *
     * @param  string $id    ID for an existing Account.
     * @param  string $type  Data source type. Valid values: (pop3|imap|caldav|contacts|yab|rss|cal|gal|xsync|tagmap).
     * @param  string $name  Data source name.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function createDataSource($id, $type, $name, array $attrs = array())
    {
        $types = array('pop3', 'imap', 'caldav', 'contacts', 'yab', 'rss', 'cal', 'gal', 'xsync', 'tagmap');
        $params = array(
            'dataSource' => array(
                'type' => in_array($type, $types) ? $type : 'contacts',
                'name' => $name,
            ),
        );
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['dataSource']['a'] = $attributes;
        }
        return $this->_client->createDataSource($params, array('id' => $id));
    }

    /**
     * Create a distribution list.
     * Notes:
     *   1. dynamic - create a dynamic distribution list.
     *   2. Extra attrs: description, zimbraNotes.
     *
     * @param  string $name    Name for distribution list.
     * @param  array  $attrs   Attributes.
     * @param  bool   $dynamic If 1 (true) then create a dynamic distribution list.
     * @return mix
     */
    public function createDistributionList($name, array $attrs = array(), $dynamic = FALSE)
    {
        $options = array(
            'name' => $name,
            'dynamic' => ((bool) $dynamic) ? 1 : 0,
        );
        $params = array();
        $validAttrs = array('description', 'zimbraNotes');
        $attributes = $this->_attributes($attrs, 'n', $validAttrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }
        return $this->_client->createDistributionList($params, $options);
    }

    /**
     * Create a domain.
     * Note:
     *   1. Extra attrs: description, zimbraNotes.
     *
     * @param  string $name  Name of new domain.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function createDomain($name, array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }
        return $this->_client->createDomain($params, array('name' => $name));
    }

    /**
     * Create a domain.
     * Notes:
     *   1. if the referenced account is not found it will be created.
     *   2. the identifier used in name attr is used for SyncGal and SearchGal.
     *   3. name attribute is for the name of the data source.
     *   4. if folder attr is not present it'll default to Contacts folder.
     *   5. passed in attrs in <a/> are used to initialize the gal data source.
     *   6. server is a required parameter and specifies the mailhost on which this account resides.
     *
     * @param  string $name     Name of the data source.
     * @param  string $domain   Domain name.
     * @param  string $server   The mailhost on which this account resides.
     * @param  string $account  The name used to identify the account.
     * @param  string $type     GalMode type. Valid values: (both|ldap|zimbra).
     * @param  string $password Password.
     * @param  string $folder   Contact folder name.
     * @param  array  $attrs    Attributes.
     * @return mix
     */
    public function createGalSyncAccount($name, $domain, $server, $account, $type = 'both', $password = '', $folder = '', array $attrs = array())
    {
        $types = array('both', 'ldap', 'zimbra');
        $options = array(
            'name' => $name,
            'domain' => $domain,
            'server' => $server,
            'type' => in_array($type, $types) ? $type : 'both',
        );

        if(!empty($password)) $options['password'] = $password;
        if(!empty($folder)) $options['folder'] = $folder;

        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }
        return $this->_client->createGalSyncAccount($params, $options);
    }

    /**
     * Create an LDAP entry.
     *
     * @param  string $dn    A valid LDAP DN String (RFC 2253) that describes the new DN to create.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function createLDAPEntry($dn, array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->createLDAPEntry($params, array('dn' => $dn));
    }

    /**
     * Create a Server.
     * Extra attrs: description, zimbraNotes.
     *
     * @param  string $name  New server name.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function createServer($name, array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->createServer($params, array('name' => $name));
    }

    /**
     * Create a system retention policy.
     * The system retention policy SOAP APIs allow the administrator
     * to edit named system retention policies that users can apply to folders and tags.
     *
     * @param  string $cos   The name used to identify the COS.
     * @param  array  $keep  Keep policy details.
     * @param  array  $purge Purge policy details.
     * @return mix
     */
    public function createSystemRetentionPolicy($cos = '', array $keep = array(), array $purge = array())
    {
        $params = array();
        if(!empty($cos))
        {
            $params['cos'] = array(
                'by' => 'name',
                '_' => $cos,
            );
        }
        $keepPolicy = $this->_retentionPolicy($keep);
        if(count($keepPolicy))
        {
            $params['keep']['policy'] = $keepPolicy;
        }
        $purgePolicy = $this->_retentionPolicy($purge);
        if(count($purgePolicy))
        {
            $params['purge']['policy'] = $purgePolicy;
        }

        return $this->_client->createSystemRetentionPolicy($params);
    }


    /**
     * Create a UC service.
     *
     * @param  string $name  New ucservice name.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function createUCService($name, array $attrs = array())
    {
        $params = array(
            'name' => $name,
        );

        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->createUCService($params);
    }

    /**
     * Create a volume.
     *
     * @param  array $volume Volume information.
     * @return mix
     */
    public function createVolume(array $volume)
    {
        $params['volume'] = array();
        if(isset($volume['id']))
            $params['volume']['id'] = $volume['id'];
        if(isset($volume['name']))
            $params['volume']['name'] = $volume['name'];
        if(isset($volume['rootpath']))
            $params['volume']['rootpath'] = $volume['rootpath'];
        if(isset($volume['type']) AND in_array((int)$volume['type'], array(1, 2, 10)))
            $params['volume']['id'] = (int) $volume['id'];
        if(isset($volume['compressBlobs']))
            $params['volume']['compressBlobs'] = ((int) $volume['compressBlobs'] > 0) ? 1 : 0;
        if(isset($volume['compressionThreshold']))
            $params['volume']['compressionThreshold'] = (int) $volume['compressionThreshold'];
        if(isset($volume['mgbits']))
            $params['volume']['mgbits'] = (int) $volume['mgbits'];
        if(isset($volume['mbits']))
            $params['volume']['mbits'] = (int) $volume['mbits'];
        if(isset($volume['fgbits']))
            $params['volume']['fgbits'] = (int) $volume['fgbits'];
        if(isset($volume['fbits']))
            $params['volume']['fbits'] = (int) $volume['fbits'];
        if(isset($volume['isCurrent']))
            $params['volume']['isCurrent'] = ((int) $volume['isCurrent'] > 0) ? 1 : 0;

        return $this->_client->createVolume($params);
    }


    /**
     * Create an XMPP component.
     *
     * @param  string $name   XMPP name.
     * @param  string $domain Domain name selector.
     * @param  string $server Server name selector.
     * @param  array  $attrs  Attributes.
     * @return mix
     */
    public function createXMPPComponent($name, $domain, $server, array $attrs = array())
    {
        $params['xmppcomponent'] = array(
            'name' => $name,
            'domain' => array(
                'by' => 'name',
                '_' => $domain,
            ),
            'server' => array(
                'by' => 'name',
                '_' => $server,
            ),
        );
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['xmppcomponent']['a'] = $attributes;
        }

        return $this->_client->createXMPPComponent($params);
    }

    /**
     * Create a Zimlet.
     *
     * @param  string $name  Zimlet name.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function createZimlet($name, array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->createZimlet($params, array('name' => $name));
    }

    /**
     * Dedupe the blobs having the same digest.
     *
     * @param  string $action  Action to perform - one of start|status|stop.
     * @param  array  $volumes Volumes.
     * @return mix
     */
    public function dedupeBlobs($action, array $volumes = array())
    {
        $options = array(
            'action' => in_array($action, array('start', 'status', 'stop', 'reset')) ? $action : 'status',
        );
        $params = array();
        if(count($volumes))
        {
            $params['volume'] = array();
            foreach ($volumes as $key => $value)
            {
                $params['volume'][] = array('id' => (int) $value);
            }
        }
        return $this->_client->dedupeBlobs($params, $options);
    }

    /**
     * Used to request a new auth token that is valid for the specified account.
     * The id of the auth token will be the id of the target account,
     * and the requesting admin's id will be stored in the auth token for auditing purposes.
     *
     * @param  string $account  The name used to identify the account.
     * @param  long   $duration Lifetime in seconds of the newly-created authtoken. defaults to 1 hour. Can't be longer then zimbraAuthTokenLifetime.
     * @return mix
     */
    public function delegateAuth($account, $duration = 3600)
    {
        $options = array(
            'duration' => ((int) $duration > 0) ? $duration : 3600,
        );
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );

        return $this->_client->delegateAuth($params, $options);
    }


    /**
     * Deletes the account with the given id.
     * Notes:
     *   1. If the request is sent to the server on which the mailbox resides,
     *      the mailbox is deleted as well.
     *   1. this request is by default proxied to the account's home server.
     *
     * @param  string $id  Zimbra identify.
     * @return mix
     */
    public function deleteAccount($id)
    {
        return $this->_client->deleteAccount(array(), array('id' => $id));
    }

    /**
     * Deletes the calendar resource with the given id.
     * Note: this request is by default proxied to the account's home server .
     * Access: domain admin sufficient.
     *
     * @param  string $id  Zimbra identify.
     * @return mix
     */
    public function deleteCalendarResource($id)
    {
        return $this->_client->deleteCalendarResource(array(), array('id' => $id));
    }

    /**
     * Delete a Class of Service (COS).
     *
     * @param  string $id  Zimbra identify.
     * @return mix
     */
    public function deleteCos($id)
    {
        return $this->_client->deleteCos(array('id' => $id));
    }

    /**
     * Deletes the given data source.
     * Note: this request is by default proxied to the account's home server.
     *
     * @param  string $id         ID for an existing Account.
     * @param  string $dataSource Data source ID.
     * @param  array  $attrs      Attributes.
     * @return mix
     */
    public function deleteDataSource($id, $dataSource, array $attrs = array())
    {
        $params = array(
            'dataSource' => array(
                'id' => $dataSource,
            ),
        );
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->deleteDataSource($params, array('id' => $id));
    }

    /**
     * Delete a distribution list.
     * Access: domain admin sufficient.
     *
     * @param  string $id Zimbra ID for distribution list.
     * @return mix
     */
    public function deleteDistributionList($id)
    {
        return $this->_client->deleteDistributionList(array(), array('id' => $id));
    }

    /**
     * Delete a domain.
     *
     * @param  string $id Zimbra ID for domain.
     * @return mix
     */
    public function deleteDomain($id)
    {
        return $this->_client->deleteDomain(array(), array('id' => $id));
    }

    /**
     * Delete a Global Address List (GAL) Synchronisation account.
     * Remove its zimbraGalAccountID from the domain, then deletes the account.
     *
     * @param  string $account The name used to identify the account.
     * @return mix
     */
    public function deleteGalSyncAccount($account)
    {
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );

        return $this->_client->deleteGalSyncAccount($params);
    }

    /**
     * Delete an LDAP entry.
     *
     * @param  string $dn A valid LDAP DN String (RFC 2253) that describes the DN to delete.
     * @return mix
     */
    public function deleteLDAPEntry($dn)
    {
        return $this->_client->deleteLDAPEntry(array(), array('dn' => $dn));
    }

    /**
     * Delete a mailbox.
     * The request includes the account ID (uuid) of the target mailbox on success,
     * the response includes the mailbox ID (numeric) of the deleted mailbox
     * the <mbox> element is left out of the response if no mailbox existed for that account.
     * Note: this request is by default proxied to the account's home server 
     * Access: domain admin sufficient
     *
     * @param  string $id Account ID.
     * @return mix
     */
    public function deleteMailbox($id)
    {
        $params = array(
            'mbox' => array(
                'id' => $id,
            ),
        );

        return $this->_client->deleteMailbox($params);
    }

    /**
     * Delete a server.
     * Note: this request is by default proxied to the referenced server.
     *
     * @param  string $id Zimbra ID.
     * @return mix
     */
    public function deleteServer($id)
    {
        return $this->_client->deleteServer(array(), array('id' => $id));
    }

    /**
     * Delete a system retention policy.
     *
     * @param  array  $policy Retention policy.
     * @param  string $cos    The name used to identify the COS.
     * @return mix
     */
    public function deleteSystemRetentionPolicy(array $policy, $cos = '')
    {
        $param['policy'] = $this->_retentionPolicy($policy);
        if(!empty($cos))
        {
            $params['cos'] = array(
                'by' => 'name',
                '_' => $cos,
            );
        }

        return $this->_client->deleteSystemRetentionPolicy($param);
    }

    /**
     * Delete a UC service.
     *
     * @param  string $id Zimbra ID.
     * @return mix
     */
    public function deleteUCService($id)
    {
        return $this->_client->deleteUCService(array('id' => $id));
    }

    /**
     * Delete a UC service.
     *
     * @param  string $id Volume ID.
     * @return mix
     */
    public function deleteVolume($id)
    {
        return $this->_client->deleteVolume(array(), array('id' => $id));
    }

    /**
     * Delete an XMPP Component.
     *
     * @param  string $xmpp The name used to identify the XMPP component.
     * @return mix
     */
    public function deleteXMPPComponent($xmpp)
    {
        $params = array(
            'xmppcomponent' => array(
                'by' => 'name',
                '_' => $xmpp,
            ),
        );
        return $this->_client->deleteXMPPComponent($params);
    }

    /**
     * Delete a Zimlet.
     *
     * @param  string $name Zimlet name.
     * @return mix
     */
    public function deleteZimlet($name)
    {
        $params = array(
            'zimlet' => array(
                'name' => $name,
            ),
        );
        return $this->_client->deleteZimlet($params);
    }

    /**
     * Delete a Zimlet.
     *
     * @param  string $action Action - valid values : deployAll|deployLocal|status.
     * @param  string $aid    Attachment ID.
     * @param  bool   $flush  Flag whether to flush the cache.
     * @param  bool   $sync   Synchronous flag.
     * @return mix
     */
    public function deployZimlet($action, $aid, $flush = FALSE, $sync = FALSE)
    {
        $options = array(
            'action' => $action,
            'flush' => ((bool) $flush) ? 1 : 0,
            'synchronous' => ((bool) $sync) ? 1 : 0,
        );
        $params = array(
            'content' => array(
                'aid' => $aid,
            ),
        );
        return $this->_client->deployZimlet($params, $options);
    }

    /**
     * Dump sessions.
     *
     * @param  bool $list List Sessions flag.
     * @param  bool $groupBy Group by account flag.
     * @return mix
     */
    public function dumpSessions($list = TRUE, $groupBy = FALSE)
    {
        $options = array(
            'listSessions' => ((bool) $list) ? 1 : 0,
            'groupByAccount' => ((bool) $groupBy) ? 1 : 0,
        );
        return $this->_client->dumpSessions(array(), $options);
    }

    /**
     * Exports the database data for the given items with SELECT INTO OUTFILE
     * and deletes the items from the mailbox.
     * Exported filenames follow the pattern {prefix}{table_name}.txt.
     * The files are written to sqlExportDir.
     * When sqlExportDir is not specified, data is not exported.
     * Export is only supported for MySQL.
     *
     * @param  string $id     Mailbox ID.
     * @param  string $dir    Path for export dir.
     * @param  string $prefix Export filename prefix.
     * @param  array $items   Mailbox items.
     * @return mix
     */
    public function exportAndDeleteItems($id, $dir, $prefix = '', array $items = array())
    {
        $options = array(
            'exportDir' => $dir,
            'exportFilenamePrefix' => $prefix,
        );
        $params = array(
            'mbox' => array(
                'id' => $id,
            ),
        );

        $mboxItems = array();
        foreach ($items as $key => $value)
        {
            $arr = array();
            $arr['id'] = (int) $value['id'];
            $arr['version'] = (int) $value['version'];
            $mboxItems[] = array();
        }
        if(count($mboxItems))
        {
            $params['mbox']['item'] = $mboxItems;
        }

        return $this->_client->exportAndDeleteItems($params, $options);
    }

    /**
     * Fix Calendar End Times.
     *
     * @param  string $account Accounts name.
     * @param  bool   $sync    Sync flag.
     * @return mix
     */
    public function fixCalendarEndTime($account, $sync = FALSE)
    {
        $options = array(
            'sync' => ((bool) $sync) ? 1 : 0,
        );
        $params = array(
            'account' => array(
                'name' => $account,
            ),
        );

        return $this->_client->fixCalendarEndTime($params, $options);
    }

    /**
     * Fix Calendar priority.
     *
     * @param  string $account Accounts name.
     * @param  bool   $sync    Sync flag.
     * @return mix
     */
    public function fixCalendarPriority($account, $sync = FALSE)
    {
        $options = array(
            'sync' => ((bool) $sync) ? 1 : 0,
        );
        $params = array(
            'account' => array(
                'name' => $account,
            ),
        );

        return $this->_client->fixCalendarPriority($params, $options);
    }

    /**
     * Fix timezone definitions in appointments and tasks to reflect changes
     * in daylight savings time rules in various timezones.
     *
     * @param  string $account Accounts name.
     * @param  string $tzfixup Timezone fixup.
     * @param  bool   $sync    Sync flag.
     * @param  string $after   Fix appts/tasks that have instances after this time, default = January 1, 2008 00:00:00 in GMT+13:00 timezone.
     * @return mix
     */
    public function fixCalendarTZ($account, array $tzfixup, $sync = FALSE, $after = 0)
    {
        $options = array(
            'sync' => ((bool) $sync) ? 1 : 0,
        );
        $params = array(
            'account' => array('name' => $account),
            'tzfixup' => $tzfixup,
        );

        return $this->_client->fixCalendarTZ($params, $options);
    }

    /**
     * FixFlush memory cache for specified LDAP or directory scan type/entries.
     * Directory scan caches(source of data is on local disk of the server): skin|locale
     * LDAP caches(source of data is LDAP): account|cos|domain|server|zimlet.
     * 
     * For LDAP caches, one or more optional <entry> can be specified. 
     * If <entry>(s) are specified, only the specified entries will be flushed.
     * If no <entry> is given, all enties of the type will be flushed from cache.
     * Type can contain a combination of skin, locale and zimlet.
     * E.g. type='skin,locale,zimlet' or type='zimletskin'.
     *
     * @param  array $types     Array of cache types. e.g. from skin|locale|account|cos|domain|server|zimlet.
     * @param  array $entries Cache entry selectors.
     * @param  bool  $all     All flag. 0 - flush cache only on the local server. 1 - flush cache only on all servers (can take a long time on systems with lots of servers)
     * @return mix
     */
    public function flushCache(array $types = array('account'), array $entries = array(), $all = FALSE)
    {
        $validTypes = array('skin', 'locale', 'account', 'cos', 'domain', 'server', 'zimlet');
        $cacheType = '';
        foreach ($types as $type)
        {
            if(in_array($type, $validTypes))
            {
                if(empty($cacheType))
                    $cacheType = $type;
                else
                    $cacheType .= ','.$type;
            }
        }
        $params = array(
            'cache' => array(
                'type' => $cacheType,
                'allServers' => ((bool) $all) ? 1 : 0,
            ),
        );
        if(count($entries))
        {
            $params['cache']['entry'] = array();
            foreach ($entries as $entry)
            {
                $params['cache']['entry'][] = array(
                    'by' => 'name',
                    '_' => $entry,
                );
            }
        }

        return $this->_client->flushCache($params);
    }

    /**
     * Request a certificate signing request (CSR).
     *
     * @param  string  $server   Accounts name.
     * @param  bool    $new      Sync flag.
     * @param  integer $keysize  Key size.
     * @param  string  $type     Type of CSR (self|comm).
     * @param  array   $attrs    Subject attributes.
     * @param  array   $altNames Used to add the Subject Alt Name extension in the certificate, so multiple hosts can be supported.
     * @return mix
     */
    public function genCSR($server, $new, $keysize = 1024, $type = 'self', array $attrs = array(), array $altNames  = array())
    {
        $options = array(
            'server' => $server,
            'new' => $new,
            'type' => in_array($type, array('self', 'comm')) ? $type : 'self',
            'keysize' => in_array((int) $keysize, array(1024, 2048)) ? $type : 1024,
        );
        $params = array();
        $subjects = array('C', 'ST', 'L', 'O','OU', 'CN');
        foreach ($attrs as $key => $value)
        {
            if(in_array($key, $subjects)) $params[$key] = $value;
        }
        if(count($altNames))
        {
            $params['SubjectAltName'] = array();
            foreach ($altNames as $name)
            {
                $params['SubjectAltName'][] = $name;
            }
        }
        return $this->_client->genCSR($params, $options);
    }

    /**
     * Get attributes related to an account.
     * {request-attrs} - comma-seperated list of attrs to return 
     * Note: this request is by default proxied to the account's home server 
     * Access: domain admin sufficient
     *
     * @param  string $account  The name used to identify the account.
     * @param  array  $attrs    List of attributes.
     * @param  bool   $applyCos Flag whether or not to apply class of service (COS) rules.
     * @return mix
     */
    public function getAccount($account, array $attrs = array(), $applyCos = TRUE)
    {
        $options = array(
            'applyCos' => ((bool)$applyCos) ? 1 : 0,
        );
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr))
        {
            $options['attrs'] = $attrsStr;
        }
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        return $this->_client->getAccount($params, $options);
    }

    /**
     * Get information about an account.
     * Currently only 2 attrs are returned:
     *   zimbraId    the unique UUID of the zimbra account
     *   zimbraMailHost  the server on which this user's mail resides 
     * Access: domain admin sufficient
     *
     * @param  string $account The name used to identify the account.
     * @return mix
     */
    public function getAccountInfo($account)
    {
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        return $this->_client->getAccountInfo($params);
    }

    /**
     * Returns custom loggers created for the given account since the last server start.
     * If the request is sent to a server other than the one that the account resides on,
     * it is proxied to the correct server.
     *
     * @param  string $account  The name used to identify the account.
     * @return mix
     */
    public function getAccountLoggers($account)
    {
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        return $this->_client->getAccountLoggers($params);
    }

    /**
     * Get distribution lists an account is a member of.
     *
     * @param  string $account The name used to identify the account.
     * @return mix
     */
    public function getAccountMembership($account)
    {
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        return $this->_client->getAccountMembership($params);
    }


    /**
     * Get distribution lists an account is a member of.
     *
     * @param  string $account The name used to identify the account.
     * @param  string $dl      The name used to identify the distribution list.
     * @return mix
     */
    public function getAdminConsoleUIComp($account, $dl = '')
    {
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        if(!empty($dl))
        {
            $params['dl'] = array(
                'by' => 'name',
                '_' => $dl,
            );
        }
        return $this->_client->getAdminConsoleUIComp($params);
    }

    /**
     * Returns the admin extension addon Zimlets.
     *
     * @return mix
     */
    public function getAdminExtensionZimlets()
    {
        return $this->_client->getAdminExtensionZimlets();
    }

    /**
     * Returns admin saved searches.
     * If no <search> is present server will return all saved searches.
     *
     * @param  string $name The search name.
     * @return mix
     */
    public function getAdminSavedSearches($name)
    {
        $params = array(
            'search' => array(
                'name' => $name,
            ),
        );
        return $this->_client->getAdminSavedSearches($params);
    }

    /**
     * Gets the aggregate quota usage for all domains on the server.
     *
     * @return mix
     */
    public function getAggregateQuotaUsageOnServer()
    {
        return $this->_client->getAggregateQuotaUsageOnServer($params);
    }

    /**
     * Returns all account loggers that have been created on the given server
     * since the last server start.
     *
     * @return mix
     */
    public function getAllAccountLoggers()
    {
        return $this->_client->getAllAccountLoggers($params);
    }

    /**
     * Get All accounts matching the selectin criteria.
     * Access: domain admin sufficient
     *
     * @param  string $server The server name.
     * @param  string $domain The domain name.
     * @return mix
     */
    public function getAllAccounts($server = '', $domain = '')
    {
        $params = array();
        if(!empty($server))
        {
            $params['server'] = array(
                'by' => 'name',
                '_' => $server,
            );
        }
        if(!empty($domain))
        {
            $params['domain'] = array(
                'by' => 'name',
                '_' => $domain,
            );
        }
        return $this->_client->getAllAccounts($params);
    }

    /**
     * Get all Admin accounts.
     *
     * @param  string $applyCos Apply COS.
     * @return mix
     */
    public function getAllAdminAccounts($applyCos = TRUE)
    {
        $options = array(
            'applyCos' => (bool) $applyCos ? 1 : 0,
        );
        return $this->_client->getAllAdminAccounts(array(), $options);
    }

    /**
     * Get all calendar resources that match the selection criteria.
     * Access: domain admin sufficient.
     *
     * @param  string $server The server name.
     * @param  string $domain The domain name.
     * @return mix
     */
    public function getAllCalendarResources($server = '', $domain = '')
    {
        $params = array();
        if(!empty($server))
        {
            $params['server'] = array(
                'by' => 'name',
                '_' => $server,
            );
        }
        if(!empty($domain))
        {
            $params['domain'] = array(
                'by' => 'name',
                '_' => $domain,
            );
        }
        return $this->_client->getAllCalendarResources($params);
    }

    /**
     * Get all config.
     *
     * @return mix
     */
    public function getAllConfig()
    {
        return $this->_client->getAllConfig();
    }

    /**
     * Get all classes of service (COS).
     *
     * @return mix
     */
    public function getAllCos()
    {
        return $this->_client->getAllCos();
    }

    /**
     * Get all calendar resources that match the selection criteria.
     * Access: domain admin sufficient.
     *
     * @param  string $domain The domain name.
     * @return mix
     */
    public function getAllDistributionLists($domain = '')
    {
        $params = array();
        if(!empty($domain))
        {
            $params['domain'] = array(
                'by' => 'name',
                '_' => $domain,
            );
        }
        return $this->_client->getAllDistributionLists($params);
    }

    /**
     * Get all domains.
     *
     * @param  bool $applyConfig Apply config flag.
     * @return mix
     */
    public function getAllDomains($applyConfig = TRUE)
    {
        $options['applyCos'] = (bool) $applyCos ? 1 : 0;
        return $this->_client->getAllDomains(array(), $options);
    }

    /**
     * Get all effective Admin rights.
     *
     * @param  string $grantee The name used to identify the grantee.
     * @param  string $type   Type:usr|grp|egp|all|dom|gst|key|pub|email.
     * @param  string $secret Password for guest grantee or the access key for key grantee For user right only.
     * @param  string $all    For GetGrantsRequest, selects whether to include grants granted to groups the specified grantee belongs to. Default is 1 (true).
     * @param  string $expand Flags whether to include all attribute names if the right is meant for all attributes.
     * @return mix
     */
    public function getAllEffectiveRights($grantee = '', $type = 'all', $secret = '', $all = TRUE, $expand = '')
    {
        $options = array();
        if(!empty($expand)) $options['expandAllAttrs'] = $expand;
        $params = array();
        if(!empty($grantee))
        {
            $types = array('usr', 'grp', 'egp', 'all', 'dom', 'gst', 'key', 'pub', 'email');
            $params['grantee'] = array(
                'type' => in_array($type, $types) ? $type : 'all',
                'all' => (bool) $all ? 1: 0,
                'by' => 'name',
                '_' => $grantee,
            );
            if(!empty($secret)) $params['grantee']['secret'] = $secret;
        }
        return $this->_client->getAllEffectiveRights($params);
    }

    /**
     * Get all free/busy providers.
     *
     * @return mix
     */
    public function getAllFreeBusyProviders()
    {
        return $this->_client->getAllFreeBusyProviders();
    }

    /**
     * Get all free/busy providers.
     *
     * @return mix
     */
    public function getAllLocales()
    {
        return $this->_client->getAllLocales();
    }

    /**
     * Return all mailboxes.
     * Returns all data from the mailbox table (in db.sql), except for the "comment" column.
     *
     * @param  integer $limit  The number of mailboxes to return (0 is default and means all).
     * @param  integer $offset The starting offset (0, 25, etc).
     * @return mix
     */
    public function getAllMailboxes($limit = 0, $offset = 0)
    {
        $options = array(
            'limit' => (int) $limit,
            'offset' => (int) $offset,
        );
        return $this->_client->getAllMailboxes(array(), $options);
    }

    /**
     * Get all effective Admin rights.
     *
     * @param  string $type   Target type on which a right is grantable.
     * @param  string $right  Right class to return (ADMIN|USER|ALL).
     * @param  bool   $expand Flags whether to include all attribute names in the <attrs> elements in GetRightResponse if the right is meant for all attributes.
     * @return mix
     */
    public function getAllRights($type = '', $right = 'ALL', $expand = TRUE)
    {
        $options = array();
        if(!empty($type)) $options['targetType'] = (string) $type;
        if(!empty($right)) $options['rightClass'] = in_array($right, array('ADMIN', 'USER', 'ALL')) ? $right : 'ALL';
        if((bool) $expand) $options['expandAllAttrs'] = 1;
        return $this->_client->getAllRights(array(), $options);
    }

    /**
     * Get all servers defined in the system or all servers that
     * have a particular service enabled (eg, mta, antispam, spell).
     * If {apply} is 1 (true), then certain unset attrs on a server
     * will get their value from the global config. 
     * If {apply} is 0 (false), then only attributes directly set on the server will be returned
     *
     * @param  string $service Service name. e.g. mta, antispam, spell.
     * @param  bool   $apply   Apply config flag.
     * @return mix
     */
    public function getAllServers($service = 'mailbox', $apply = FALSE)
    {
        $options = array(
            'service' => (string) $service,
            'applyConfig' => ((bool) $apply) ? 1 : 0,
        );
        return $this->_client->getAllServers(array(), $options);
    }

    /**
     * Get all installed skins on the server.
     *
     * @return mix
     */
    public function getAllSkins()
    {
        return $this->_client->getAllSkins();
    }

    /**
     * Returns all installed UC providers and applicable UC service attributes for each provider.
     *
     * @return mix
     */
    public function getAllUCProviders()
    {
        return $this->_client->getAllUCProviders();
    }

    /**
     * Get all ucservices defined in the system.
     *
     * @return mix
     */
    public function getAllUCServices()
    {
        return $this->_client->getAllUCServices();
    }

    /**
     * Get all volumes.
     *
     * @return mix
     */
    public function getAllVolumes()
    {
        return $this->_client->getAllVolumes();
    }

    /**
     * Get all XMPP components.
     *
     * @return mix
     */
    public function getAllXMPPComponents()
    {
        return $this->_client->getAllXMPPComponents();
    }

    /**
     * Get all Zimlets.
     *
     * @param  string $exclude Can be "none|extension|mail". extension: return only mail Zimlets. mail: return only admin extensions. none [default]: return both mail and admin zimlets.
     * @return mix
     */
    public function getAllZimlets($exclude = 'none')
    {
        $exclude = in_array($exclude, array('none', 'mail', 'extension')) ? $exclude : 'none';
        return $this->_client->getAllZimlets(array(), array('exclude' => $exclude));
    }

    /**
     * Get attribute information.
     * Valid entry types:
     *   account,alias,distributionList,cos,globalConfig,domain,server,mimeEntry,zimletEntry,
     *   calendarResource,identity,dataSource,pop3DataSource,imapDataSource,rssDataSource,
     *   liveDataSource,galDataSource,signature,xmppComponent,aclTarget
     *
     * @param  array $attrs      Attributes to return.
     * @param  array $entryTypes Attributes on the specified entry types will be returned.
     * @return mix
     */
    public function getAttributeInfo(array $attrs = array(), array $entryTypes = array())
    {
        $options = array();
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr)) $options['attrs'] = $attrsStr;
        $types = explode(',', 'account,alias,distributionList,cos,globalConfig,domain,server,mimeEntry,zimletEntry,calendarResource,identity,dataSource,pop3DataSource,imapDataSource,rssDataSource,liveDataSource,galDataSource,signature,xmppComponent,aclTarget');
        $entryTypesStr = $this->_commaAttributes($entryTypes, $types);
        if(!empty($entryTypesStr)) $options['entryTypes'] = $entryTypesStr;
        return $this->_client->getAttributeInfo(array(), $options);
    }

    /**
     * Get a certificate signing request (CSR).
     *
     * @param  string $server      Server ID. Can be "--- All Servers ---" or the ID of a server.
     * @param  string $type Type of CSR (required). Value: self mean self-signed certificate; comm mean commercial certificate
     * @return mix
     */
    public function getCSR($server = '--- All Servers ---', $type = 'self')
    {
        $options = array(
            'server' => empty($server) ? '--- All Servers ---' : $server,
            'type' => in_array($type, array('self', 'comm')) ? $type : 'self',
        );
        return $this->_client->getCSR(array(), $options);
    }

    /**
     * Get a calendar resource.
     * Access: domain admin sufficient.
     *
     * @param  string $account  The name used to identify the account.
     * @param  bool   $applyCos Flag whether to apply Class of Service (COS).
     * @param  array  $attrs    Array of attributes.
     * @return mix
     */
    public function getCalendarResource($account = '', $applyCos = TRUE, array $attrs = array())
    {
        $options = array(
            'applyCos' => ((bool) $applyCos) ? 1 : 0,
        );
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr)) $options['attrs'] = $attrsStr;
        $params = array();
        if(!empty($account))
        {
            $params['calresource'] = array(
                'by' => 'name',
                '_' => $account,
            );
        }
        return $this->_client->getCalendarResource($params, $options);
    }

    /**
     * Get Certificate.
     * Currently, GetCertRequest/Response only handle 2 types "staged" and "all".
     * May need to support other options in the future.
     *
     * @param  string $server The server's ID whose cert is to be got.
     * @param  bool   $type   Certificate type. Value: staged - view the staged crt. Other options (all, mta, ldap, mailboxd, proxy) are used to view the deployed crt
     * @param  array  $option Required only when type is "staged". Could be "self" (self-signed cert) or "comm" (commerical cert).
     * @return mix
     */
    public function getCert($server, $type = 'all', $option = 'self')
    {
        $options = array(
            'server' => $server,
            'type' => in_array($type, array('all', 'mta', 'ldap', 'mailboxd', 'proxy', 'staged')) ? $type : 'all',
            'option' => in_array($option, array('self', 'comm')) ? $option : 'self',
        );
        return $this->_client->getCert(array(), $options);
    }

    /**
     * Get Config request.
     *
     * @param  array $attrs Array of attributes.
     * @return mix
     */
    public function getConfig(array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;

        return $this->_client->getConfig($params);
    }

    /**
     * Get Class Of Service (COS).
     *
     * @param  string $cos   The name used to identify the COS.
     * @param  array  $attrs Array of attributes.
     * @return mix
     */
    public function getCos($cos = '', array $attrs = array())
    {
        $options = array();
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr)) $options['attrs'] = $attrsStr;

        $params = array();
        if(!empty($cos))
        {
            $params['cos'] = array(
                'by' => 'name',
                '_' => $cos,
            );
        }

        return $this->_client->getCos($params, $options);
    }

    /**
     * Returns attributes, with defaults and constraints if any,
     * that can be set by the authed admin when an object is created.
     * Domain name.required if target type is account/calresource/dl/domain, ignored otherwise.
     *   1. if {target-type} is account/calresource/dl:
     *      This is the domain in which the object will be in.
     *      The domain can be speciffied by id or by name.
     *   2. if {target-type} is domain, it is the domain name to be created.
     *      E.g. to create a subdomain named foo.bar.test.com,
     *      should pass in <domain by="name">foo.bar.test.com</domain>.
     *
     * @param  string $type   Target type. Valid values: (userAccount|account|alias|dl|domain|cos|server|calresource|accountOnUCService|cosOnUCService|domainOnUCService|internalUserAccount|internalArchivingAccount)
     * @param  string $target Target.
     * @param  string $domain The name used to identify the domain.
     * @param  string $cos    The name used to identify the COS..
     * @return mix
     */
    public function getCreateObjectAttrs($type, $target = '', $domain = '', $cos = '')
    {
        $params = array(
            'target' => array(
                'type' => in_array($type, $this->_objectTypes) ? $type : 'account',
                '_' => !empty($target) ? $target : NULL,
            ),
        );
        if(!empty($domain))
        {
            $params['domain'] = array(
                'by' => 'name',
                '_' => $domain,
            );
        }
        if(!empty($cos))
        {
            $params['cos'] = array(
                'by' => 'name',
                '_' => $cos,
            );
        }

        return $this->_client->getCreateObjectAttrs($params);
    }

    /**
     * Get current volumes.
     *
     * @return mix
     */
    public function getCurrentVolumes()
    {
        return $this->_client->getCurrentVolumes();
    }
    /**
     * Returns all data sources defined for the given mailbox.
     * For each data source, every attribute value is returned except password.
     * Note: this request is by default proxied to the account's home server.
     *
     * @param  string $id    Account ID for an existing account.
     * @param  array  $attrs Array of attributes.
     * @return mix
     */
    public function getDataSources($id, array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->getDataSources($params, array('id' => $id));
    }

    /**
     * Get constraints (zimbraConstraint) for delegated admin on global config or a COS
     * none or several attributes can be specified for which constraints are to be returned.
     * If no attribute is specified, all constraints on the global config/cos will be returned.
     * If there is no constraint for a requested attribute,
     * <a> element for the attribute will not appear in the response.
     *
     * @param  string $type  Target type. Valid values: (account|calresource|cos|dl|group|domain|server|ucservice|xmppcomponent|zimlet|config|global).
     * @param  string $name  Name of target.
     * @param  string $id    ID of target.
     * @param  array  $attrs Array of name.
     * @return mix
     */
    public function getDelegatedAdminConstraints($type, $name = '', $id = '', array $attrs = array())
    {
        $options = array(
            'type' => in_array($type, $this->_resourceTypes) ? $type : 'account',
        );
        if(!empty($name))
        {
            $options['name'] = $name;
        }
        if(!empty($name))
        {
            $options['id'] = $id;
        }
        $params = array();
        $attributes = $this->_attributes($attrs, 'name');
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->getDelegatedAdminConstraints($params, $options);
    }

    /**
     * Get devices.
     *
     * @param  string $account The name used to identify the account.
     * @return mix
     */
    public function getDevices($account)
    {
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        return $this->_client->getDevices($params);
    }

    /**
     * Get a Distribution List.
     *
     * @param  string  $dl     The name used to identify the distribution list.
     * @param  array   $attrs  Attributes.
     * @param  integer $limit  The maximum number of accounts to return (0 is default and means all).
     * @param  integer $offset The starting offset (0, 25 etc).
     * @param  bool    $asc    Flag whether to sort in ascending order 1 (true) is the default.
     * @return mix
     */
    public function getDistributionList($dl = '', array $attrs = array(), $limit = 0, $offset = 0, $asc = TRUE)
    {
        $options = array(
            'limit' => (int) $limit,
            'offset' => (int) $offset,
            'sortAscending' => (bool) $asc ? 1 : 0,
        );
        $params = array();
        if(!empty($dl))
        {
            $params['dl'] = array(
                'by' => 'name',
                '_' => $dl,
            );
        }
        $attributes = $this->_attributes($attrs);
        if(count($attributes))
        {
            $params['a'] = $attributes;
        }

        return $this->_client->getDistributionList($params, $options);
    }

    /**
     * Request a list of DLs that a particular DL is a member of.
     *
     * @param  string  $dl     The name used to identify the distribution list.
     * @param  integer $limit  The maximum number of DLs to return (0 is default and means all).
     * @param  integer $offset The starting offset (0, 25 etc).
     * @return mix
     */
    public function getDistributionListMembership($dl = '', $limit = 0, $offset = 0)
    {
        $options = array(
            'limit' => (int) $limit,
            'offset' => (int) $offset,
        );
        $params = array();
        if(!empty($dl))
        {
            $params['dl'] = array(
                'by' => 'name',
                '_' => $dl,
            );
        }

        return $this->_client->getDistributionListMembership($params, $options);
    }

    /**
     * Get information about a domain.
     * 
     * @param  string $domain The name used to identify the domain.
     * @param  bool   $apply  Apply config flag. True, then certain unset attrs on a domain will get their values from the global config. False, then only attributes directly set on the domain will be returned.
     * @param  array  $attrs  Attributes.
     * @return mix
     */
    public function getDomain($domain = '', $apply = TRUE, array $attrs = array())
    {
        $options = array(
            'applyConfig' => ((bool) $apply) ? 1 : 0,
        );
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr))
        {
            $options['attrs'] = $attrsStr;
        }

        $params = array();
        if(!empty($domain))
        {
            $params['domain'] = array(
                'by' => 'name',
                '_' => $domain,
            );
        }

        return $this->_client->getDomain($params, $options);
    }

    /**
     * Get Domain information.
     * This call does not require an auth token.
     * It returns attributes that are pertinent to domain settings
     * for cases when the user is not authenticated.
     * For example, URL to direct the user to upon logging out or when auth token is expired.
     * 
     * @param  string $domain The name used to identify the domain.
     * @param  bool   $apply  Apply config flag. True, then certain unset attrs on a domain will get their values from the global config. False, then only attributes directly set on the domain will be returned.
     * @return mix
     */
    public function getDomainInfo($domain = '', $apply = TRUE)
    {
        $options = array(
            'applyConfig' => ((bool) $apply) ? 1 : 0,
        );

        $params = array();
        if(!empty($domain))
        {
            $params['domain'] = array(
                'by' => 'name',
                '_' => $domain,
            );
        }

        return $this->_client->getDomainInfo($params, $options);
    }

    /**
     * Returns effective ADMIN rights the authenticated admin has on the specified target entry.
     * Effective rights are the rights the admin is actually allowed.
     * It is the net result of applying ACL checking rules given the target and grantee.
     * Specifically denied rights will not be returned.
     * 
     * @param  string $target  The name used to identify the target.
     * @param  string $type    Target type. Valid values: (account|calresource|cos|dl|group|domain|server|ucservice|xmppcomponent|zimlet|config|global).
     * @param  array  $grantee Grantee.
     * @param  string $expand  Whether to include all attribute names in the <getAttrs>/<setAttrs> elements in the response if all attributes of the target are gettable/settable.
     *                         Valid values are:
     *                         1. getAttrs: expand attrs in getAttrs in the response
     *                         2. setAttrs: expand attrs in setAttrs in the response
     *                         3. getAttrs,setAttrs: expand attrs in both getAttrs and setAttrs in the response
     * @return mix
     */
    public function getEffectiveRights($target, $type, array $grantee = array(), $expand = 'getAttrs,setAttrs')
    {
        $options = array(
            'expandAllAttrs' => in_array($expand, array('getAttrs', 'setAttrs', 'getAttrs,setAttrs')) ? $expand : 'getAttrs,setAttrs',
        );
        $params = array(
            'target' => array(
                'by' => 'name',
                'type' => in_array($type, $this->_resourceTypes) ? $type : 'account',
                '_' => $target,
            ),
        );

        if(count($grantee))
        {
            $params['grantee'] = array();
            if(isset($grantee['_']))
                $params['grantee']['_'] = $grantee['_'];
            $gTypes = array('usr', 'grp', 'egp', 'all', 'dom', 'gst', 'key', 'pub', 'email');
            if(isset($grantee['type']) AND in_array($grantee['type'], $gTypes))
                $params['grantee']['type'] = $grantee['type'];
            if(isset($grantee['by']) AND in_array($grantee['by'], array('id', 'name')))
                $params['grantee']['by'] = $grantee['by'];
            if(isset($grantee['all']))
                $params['grantee']['all'] = ((int) $grantee['all'] > 0) ? 1 : 0;
        }

        return $this->_client->getEffectiveRights($params, $options);
    }

    /**
     * Get Free/Busy provider information.
     * If the optional element <provider/> is present in the request, the response contains the requested provider only.
     * If no provider is supplied in the request, the response contains all the providers.
     * 
     * @param  string $name Provider name.
     * @return mix
     */
    public function getFreeBusyQueueInfo($name)
    {
        $params = array(
            'provider' => array(
                'name' => $name,
            ),
        );
        return $this->_client->getFreeBusyQueueInfo($params);
    }

    /**
     * Returns all grants on the specified target entry,
     * or all grants granted to the specified grantee entry. 
     * The authenticated admin must have an effective "viewGrants"
     * (TBD) system right on the specified target/grantee. 
     * At least one of <target> or <grantee> must be specified.
     * If both <target> and <grantee> are specified, only grants that are granted
     * on the target to the grantee are returned.
     * 
     * @param  string $target  The name used to identify the target.
     * @param  string $type    Target type. Valid values: (account|calresource|cos|dl|group|domain|server|ucservice|xmppcomponent|zimlet|config|global).
     * @param  array  $grantee Grantee.
     * @return mix
     */
    public function getGrants($target, $type, array $grantee = array())
    {
        $params = array(
            'target' => array(
                'by' => 'name',
                'type' => in_array($type, $this->_resourceTypes) ? $type : 'account',
                '_' => $target,
            ),
        );

        if(count($grantee))
        {
            $params['grantee'] = array();
            if(isset($grantee['_']))
                $params['grantee']['_'] = $grantee['_'];
            $gTypes = array('usr', 'grp', 'egp', 'all', 'dom', 'gst', 'key', 'pub', 'email');
            if(isset($grantee['type']) AND in_array($grantee['type'], $gTypes))
                $params['grantee']['type'] = $grantee['type'];
            if(isset($grantee['by']) AND in_array($grantee['by'], array('id', 'name')))
                $params['grantee']['by'] = $grantee['by'];
            if(isset($grantee['all']))
                $params['grantee']['all'] = ((int) $grantee['all'] > 0) ? 1 : 0;
        }

        return $this->_client->getGrants($params);
    }

    /**
     * Get index statistics.
     * 
     * @param  string $id  Account ID.
     * @return mix
     */
    public function getIndexStats($id)
    {
        $params = array(
            'mbox' => array(
                'id' => $id,
            ),
        );
        return $this->_client->getIndexStats($params);
    }

    /**
     * Get index statistics.
     * 
     * @param  string  $query  Query string. Should be an LDAP-style filter string (RFC 2254).
     * @param  string  $base   LDAP search base. An LDAP-style filter string that defines an LDAP search base (RFC 2254).
     * @param  integer $limit  Limit - the maximum number of LDAP objects (records) to return (0 is default and means all).
     * @param  integer $offset The starting offset (0, 25, etc).
     * @param  string  $sort   Name of attribute to sort on. default is null.
     * @param  bool    $asc    Flag whether to sort in ascending order 1 (true) is default.
     * @return mix
     */
    public function getLDAPEntries($query, $base, $limit = 0, $offset = 0, $sort = NULL, $asc = TRUE)
    {
        $options = array(
            'query' => $query,
            'limit' => (int) $limit,
            'offset' => (int) $offset,
            'sort' => $sort,
            'sortAscending' => (bool) $asc ? 1 : 0,
        );
        $params = array(
            'ldapSearchBase' => $base,
        );

        return $this->_client->getLDAPEntries($params, $options);
    }

    /**
     * Get License information.
     * 
     * @return mix
     */
    public function getLicenseInfo()
    {
        return $this->_client->getLicenseInfo();
    }

    /**
     * Query to retrieve Logger statistics in ZCS.
     * Use cases:
     *   1. No elements specified. Result: a listing of reporting host names.
     *   2. Hostname specified. Result: a listing of stat groups for the specified host.
     *   3. Hostname and stats specified, text content of stats non-empty.
     *      Result: a listing of columns for the given host and group
     *   4. Hostname and stats specified, text content empty, startTime/endTime optional.
     *      Result: all of the statistics for the given host/group are returned,
     *      if start and end are specified, limit/expand the timerange to the given setting.
     *      If limit=true is specified, attempt to reduce result set to under 500 records
     * 
     * @param  string $hostname  Hostname.
     * @param  string $startTime Start time.
     * @param  string $endTime   End time .
     * @param  array  $stats     Stats specification.
     * @return mix
     */
    public function getLoggerStats($hostname, $startTime, $endTime, array $stats = array())
    {
        $params = array(
            'hostname' => array(
                'hn' => $hostname,
            ),
            'startTime' => array(
                'time' => $startTime,
            ),
            'endTime' => array(
                'time' => $endTime,
            ),
        );
        if(count($stats))
        {
            $params['stats'] = array();
            if(isset($stats['limit'])) $params['stats']['limit'] = $stats['limit'];
            if(isset($stats['name'])) $params['stats']['name'] = $stats['name'];
            $params['stats']['values'] = array();
            if(isset($stats['values']) && is_array($stats['values']))
            {
                foreach ($stats['values'] as $value)
                {
                    $params['stats']['values'][] = array(
                        'stat' => array('name' => $value),
                    );
                }
            }
        }

        return $this->_client->getLoggerStats($params);
    }

    /**
     * Summarize and/or search a particular mail queue on a particular server.
     * The admin SOAP server initiates a MTA queue scan (via ssh)
     * and then caches the result of the queue scan.
     * To force a queue scan, specify scan=1 in the request.
     * The response has two parts.
     *   1. <qs> elements summarize queue by various types of data (sender addresses,
     *      recipient domain, etc). Only the deferred queue has error summary type.
     *   2. <qi> elements list the various queue items that match the requested query.
     * The stale-flag in the response means that since the scan,
     * some queue action was done and the data being presented is now stale.
     * This allows us to let the user dictate when to do a queue scan.
     * The scan-flag in the response indicates that the server has not completed scanning
     * the MTA queue, and that this scan is in progress,
     * and the client should ask again in a little while. 
     * The more-flag in the response indicates that more qi's are available past the limit specified in the request.
     * 
     * @param  string  $server Server Mail Queue Query.
     * @param  string  $queue  Queue name.
     * @param  array   $query  Query.
     * @param  array   $field  Queue query field.
     * @param  bool    $scan   To fora a queue scan, set this to 1 (true).
     * @param  integer $wait   Maximum time to wait for the scan to complete in seconds (default 3).
     * @param  integer $limit  Limit the number of queue items to return in the response.
     * @param  integer $offset Offset - the starting offset (0, 25, etc).
     * @return mix
     */
    public function getMailQueue($server, $queue, array $field = array(), $scan = TRUE, $wait = 3, $limit = 0, $offset = 0)
    {
        $params = array(
            'server' => array(
                'name' => $server,
                'queue' => array(
                    'name' => $queue,
                    'scan' => ((bool) $scan) ? 1 : 0,
                    'wait' => ((int) $wait < 3) ? 3 : (int) $wait,
                ),
            ),
        );

        $params['server']['queue']['query'] = array(
            'limit' => $limit,
            'offset' => $offset,
        );
        $params['server']['queue']['query']['field'] = $field;

        return $this->_client->getMailQueue($params);
    }

    /**
     * Get a count of all the mail queues by counting the number of files in the queue directories.
     * Note that the admin server waits for queue counting to complete before responding
     * - client should invoke requests for different servers in parallel.
     * 
     * @param  string $server MTA server name.
     * @return mix
     */
    public function getMailQueueInfo($server)
    {
        $params = array(
            'server' => array(
                'name' => $server,
            ),
        );
        return $this->_client->getMailQueueInfo($params);
    }

    /**
     * Get a Mailbox.
     * Note: this request is by default proxied to the account's home server.
     * 
     * @param  string $id Account ID.
     * @return mix
     */
    public function getMailbox($id)
    {
        $params = array(
            'mbox' => array(
                'id' => $id,
            ),
        );
        return $this->_client->getMailbox($params);
    }

    /**
     * Get MailBox Statistics.
     * 
     * @return mix
     */
    public function getMailboxStats()
    {
        return $this->_client->getMailboxStats();
    }

    /**
     * Returns the memcached client configuration on a mailbox server.
     * 
     * @return mix
     */
    public function getMemcachedClientConfig()
    {
        return $this->_client->getMemcachedClientConfig();
    }

    /**
     * Returns the memcached client configuration on a mailbox server.
     * 
     * @param  string  $domain  Domain - the domain name to limit the search to.
     * @param  bool    $all     Whether to fetch quota usage for all domain accounts from across all mailbox servers, default is false, applicable when domain attribute is specified.
     * @param  integer $limit   Limit - the number of accounts to return (0 is default and means all).
     * @param  integer $offset  Offset - the starting offset (0, 25, etc).
     * @param  string  $sort    SortBy - valid values: "percentUsed", "totalUsed", "quotaLimit".
     * @param  bool    $asc     Whether to sort in ascending order 0 (false) is default, so highest quotas are returned first.
     * @param  bool    $refresh Refresh - whether to always recalculate the data even when cached values are available. 0 (false) is the default..
     * @return mix
     */
    public function getQuotaUsage($domain = '', $all = TRUE, $limit = 0, $offset = 0, $sort = 'percentUsed', $asc = FALSE, $refresh = FALSE)
    {
        $options = array(
            'allServers' => ((bool) $all) ? 1: 0,
            'sortAscending' => ((bool) $asc) ? 1: 0,
            'refresh' => ((bool) $refresh) ? 1: 0,
            'limit' => (int) $limit,
            'offset' => (int) $offset,
        );
        if(!empty($domain)) $options['domain'] = $domain;
        if(!empty($sort) && in_array($sort, array('percentUsed', 'totalUsed', 'quotaLimit')))
        {
            $options['sortBy'] = $sort;
        }
        return $this->_client->getQuotaUsage(array(), $options);
    }

    /**
     * Get definition of a right.
     * 
     * @param  string $right  Right name.
     * @param  bool   $expand Whether to include all attribute names in the <attrs> elements in the response if the right is meant for all attributes.
     *                        0 (false) [default] default, do not include all attribute names in the <attrs> elements.
     *                        1 (true)  include all attribute names in the <attrs> elements.
     * @return mix
     */
    public function getRight($right, $expand = FALSE)
    {
        $options = array(
            'expandAllAttrs' => ((bool) $expand) ? 1 : 0,
        );
        $params = array(
            'right' => $right,
        );
        return $this->_client->getRight($params, $options);
    }

    /**
     * Get Rights Document.
     * 
     * @param  array $packages Packages.
     * @return mix
     */
    public function getRightsDoc(array $packages = array())
    {
        $params = array();
        if(count($packages))
        {
            $params['package'] = array();
            foreach ($packages as $package)
            {
                $params['package'][] = array('name' => $package);
            }
        }
        return $this->_client->getRightsDoc($params);
    }

    /**
     * Get Server.
     * 
     * @param  string $server Server name.
     * @param  array  $attrs  Attributes.
     * @param  bool   $apply  Apply config flag.
     *                        If {apply} is 1 (true), then certain unset attrs on a server will get their values from the global config. 
     *                        if {apply} is 0 (false), then only attributes directly set on the server will be returned.
     * @return mix
     */
    public function getServer($server, array $attrs = array(), $apply = TRUE)
    {
        $options = array(
            'applyConfig' => ((bool)$apply) ? 1 : 0,
        );
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr)) $options['attrs'] = $attrsStr;

        $params = array();
        if(!empty($server))
        {
            $params['server'] = array(
                'by' => 'name',
                '_' => $server,
            );
        }
        return $this->_client->getServer($params, $options);
    }

    /**
     * Get Network Interface information for a server.
     * Get server's network interfaces. Returns IP addresses and net masks.
     * This call will use zmrcd to call /opt/zimbra/libexec/zmserverips
     * 
     * @param  string $server Server name.
     * @param  string $type   Specifics the ipAddress type (ipV4/ipV6/both). default is ipv4.
     * @return mix
     */
    public function getServerNIfs($server, $type = 'ipV4')
    {
        $options = array(
            'type' => in_array($type, array('ipV4', 'ipV6', 'both')) ? $type : 'ipV4',
        );
        $params = array(
            'server' => array(
                'by' => 'name',
                '_' => $server,
            ),
        );
        return $this->_client->getServerNIfs($params, $options);
    }

    /**
     * Returns server monitoring stats.
     * These are the same stats that are logged to mailboxd.csv.
     * If no <stat> element is specified, all server stats are returned.
     * If the stat name is invalid, returns a SOAP fault.
     * 
     * @param  array $stats Stats.
     * @return mix
     */
    public function getServerStats(array $stats = array())
    {
        $params = array();
        $arrStats = array();
        foreach ($stats as $stat)
        {
            $arrStat = array();
            if(isset($stat['name'])) $arrStat['name'] = $stat['name'];
            if(isset($stat['description'])) $arrStat['description'] = $stat['description'];
            if(isset($stat['_'])) $arrStat['_'] = $stat['_'];
            $arrStats[] = $arrStat;
        }
        if(count($arrStats)) $params['stat'] = $arrStats;
        return $this->_client->getServerStats($params);
    }

    /**
     * Get Service Status.
     * 
     * @return mix
     */
    public function getServiceStatus()
    {
        return $this->_client->getServiceStatus();
    }

    /**
     * Get Sessions.
     * Access: domain admin sufficient (though a domain admin can't specify "domains" as a type).
     * 
     * @param  string  $type    Type - valid values soap|imap|admin.
     * @param  string  $sortBy  Sort by - valid values nameAsc|nameDesc|createdAsc|createdDesc|accessedAsc|accessedDesc.
     * @param  integer $limit   Limit - the number of sessions to return per page (0 is default and means all).
     * @param  integer $offset  Offset - the starting offset (0, 25, etc).
     * @param  bool    $refresh Refresh. If 1 (true), ignore any cached results and start fresh..
     * @return mix
     */
    public function getSessions($type, $sortBy = 'nameAsc', $limit = 0, $offset = 0, $refresh = TRUE)
    {
        $sorts = array('nameAsc', 'nameDesc', 'createdAsc', 'createdDesc', 'accessedAsc', 'accessedDesc');
        $options = array(
            'type' => in_array($type, array('soap', 'imap', 'admin')) ? $type : 'imap',
            'sortBy' => in_array($sortBy, $sorts) ? $sortBy : 'nameAsc',
            'limit' => (int) $limit,
            'offset' => (int) $offset,
            'refresh' => ((bool) $refresh) ? 1 : 0,
        );
        return $this->_client->getSessions(array(), $options);
    }

    /**
     * Iterate through all folders of the owner's mailbox and return shares
     * that match grantees specified by the <grantee> specifier.
     * 
     * @param  string $owner The name used to identify the account.
     * @param  string $type  If specified, filters the result by the specified grantee type.
     * @param  string $name  If specified, filters the result by the specified grantee name.
     * @param  string $id    If specified, filters the result by the specified grantee ID.
     * @return mix
     */
    public function getShareInfo($owner, $type = '', $name = '', $id = '')
    {
        $params = array(
            'owner' => array(
                'by' => 'name',
                '_' => $owner,
            ),
        );
        if(!empty($type)) $params['grantee']['type'] = $type;
        if(!empty($name)) $params['grantee']['name'] = $name;
        if(!empty($id)) $params['grantee']['id'] = $id;

        return $this->_client->getShareInfo($params);
    }

    /**
     * Get System Retention Policy.
     * The system retention policy SOAP APIs allow the administrator
     * to edit named system retention policies that users can apply to folders and tags.
     * 
     * @param  string $cos The name used to identify the COS.
     * @return mix
     */
    public function getSystemRetentionPolicy($cos = '')
    {
        $params = array();
        if(!empty($cos))
        {
            $params['cos'] = array(
                'by' => 'name',
                '_' => $cos,
            );
        }
        return $this->_client->getSystemRetentionPolicy($params);
    }

    /**
     * Get UC Service.
     * 
     * @param  string $ucservice UC Service name.
     * @param  array  $attrs     Attributes.
     * @return mix
     */
    public function getUCService($ucservice = '', array $attrs = array())
    {
        $options = array();
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr))
        {
            $options['attrs'] = $attrsStr;
        }
        $params = array();
        if(!empty($ucservice))
        {
            $params['ucservice'] = array(
                'by' => 'name',
                '_' => $ucservice,
            );
        }
        return $this->_client->getUCService($params, $options);
    }

    /**
     * Get Version information.
     * 
     * @return mix
     */
    public function getVersionInfo()
    {
        return $this->_client->getVersionInfo();
    }

    /**
     * Get Volume.
     * 
     * @param  integer $id ID of volume.
     * @return mix
     */
    public function getVolume($id)
    {
        return $this->_client->getVolume(array(), array('id' => (int) $id));
    }

    /**
     * Get Volume.
     * 
     * @param  string $xmpp  The name used to identify the XMPP component.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function getXMPPComponent($xmpp, array $attrs = array())
    {
        $options = array();
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr))
        {
            $options['attrs'] = $attrsStr;
        }
        $params = array(
            'xmppcomponent' => array(
                'by' => 'name',
                '_' => $xmpp,
            ),
        );
        return $this->_client->getXMPPComponent($params, $options);
    }

    /**
     * Retreives a list of search tasks running or cached on a server.
     * 
     * @param  string $name  Zimlet name.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function getZimlet($name, array $attrs = array())
    {
        $options = array();
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr))
        {
            $options['attrs'] = $attrsStr;
        }
        $params = array(
            'zimlet' => array(
                'name' => $name,
            ),
        );
        return $this->_client->getZimlet($params, $options);
    }

    /**
     * Get status for Zimlets.
     * Priority is listed in the global list <zimlets> ... </zimlets> only.
     * This is because the priority value is relative to other Zimlets in the list.
     * The same Zimlet may show different priority number depending
     * on what other Zimlets priorities are.
     * The same Zimlet will show priority 0 if all by itself,
     * or priority 3 if there are three other Zimlets with higher priority.
     * 
     * @return mix
     */
    public function getZimletStatus()
    {
        return $this->_client->getZimletStatus();
    }

    /**
     * Grant a right on a target to an individual or group grantee.
     * 
     * @param  string $target  Target selector. The name used to identify the target.
     * @param  string $type    Target type. Valid values: (account|calresource|cos|dl|group|domain|server|ucservice|xmppcomponent|zimlet|config|global).
     * @param  array  $grantee Grantee selector.
     * @param  array  $right   Right selector.
     * @return mix
     */
    public function grantRight($target, $type, array $grantee, array $right)
    {
        $params = array(
            'target' => array(
                'type' => in_array($type, $this->_resourceTypes) ? $type : 'account',
                'by' => 'name',
                '_' => $target,
            ),
            'grantee' => array(),
            'right' => array(),
        );
        if(isset($grantee['_']))
            $params['grantee']['_'] = $grantee['_'];
        $gTypes = array('usr', 'grp', 'egp', 'all', 'dom', 'gst', 'key', 'pub', 'email');
        if(isset($grantee['type']) AND in_array($grantee['type'], $gTypes))
            $params['grantee']['type'] = $grantee['type'];
        if(isset($grantee['by']) AND in_array($grantee['by'], array('id', 'name')))
            $params['grantee']['by'] = $grantee['by'];
        if(isset($grantee['secret']))
            $params['grantee']['secret'] = $grantee['secret'];
        if(isset($grantee['all']))
            $params['grantee']['all'] = ((int) $grantee['all'] > 0) ? 1 : 0;

        if(isset($right['_']))
            $params['right']['_'] = $right['_'];
        if(isset($grantee['deny']))
            $params['grantee']['deny'] = ((int) $grantee['deny'] > 0) ? 1 : 0;
        if(isset($grantee['canDelegate']))
            $params['grantee']['canDelegate'] = ((int) $grantee['canDelegate'] > 0) ? 1 : 0;
        if(isset($grantee['disinheritSubGroups']))
            $params['grantee']['disinheritSubGroups'] = ((int) $grantee['disinheritSubGroups'] > 0) ? 1 : 0;
        if(isset($grantee['subDomain']))
            $params['grantee']['subDomain'] = ((int) $grantee['subDomain'] > 0) ? 1 : 0;

        return $this->_client->grantRight($params);
    }

    /**
     * Command to act on invidual queue files.
     * This proxies through to postsuper.
     * list-of-ids can be ALL.
     * 
     * @param  string $server MTA server.
     * @param  string $queue  Queue name.
     * @param  string $op     Action operation. Valid values: (hold|release|delete|requeue).
     * @param  string $by     Action by selector (id|query). (id) Body contains a list of ids. (query) Body contains a query element
     * @param  array  $query  Queue query
     * @return mix
     */
    public function mailQueueAction($server, $queue, $op, $by, array $query = array(), array $field = array())
    {
        $params = array(
            'server' => array(
                'name' => $server,
                'queue' => array(
                    'name' => $queue,
                    'action' => array(
                        'op' => in_array($op, array('hold', 'release', 'delete', 'requeue')) ? $op : 'hold',
                        'by' => in_array($by, array('id', 'query')) ? $by : 'query',
                        'query' => array(),
                    ),
                ),
            ),
        );
        if(isset($query['limit'])) $params['server']['queue']['action']['query']['limit'] = (int) $query['limit'];
        if(isset($query['offset'])) $params['server']['queue']['action']['query']['offset'] = (int) $query['offset'];
        if(count($field)) $params['server']['queue']['action']['query']['field'] = $field;
        return $this->_client->mailQueueAction($params);
    }

    /**
     * Command to invoke postqueue -f.
     * All queues cached in the server are stale after invoking this because
     * this is a global operation to all the queues in a given server.
     * 
     * @param  string $server MTA server.
     * @return mix
     */
    public function mailQueueFlush($server)
    {
        $params = array(
            'server' => array('name' => $server),
        );
        return $this->_client->mailQueueFlush($params);
    }

    /**
     * Migrate an account.
     * 
     * @param  string $id     Zimbra ID of account.
     * @param  string $action Action.
     * @return mix
     */
    public function migrateAccount($id, $action)
    {
        $params = array(
            'migrate' => array(
                'id' => $id,
                'action' => $action,
            ),
        );
        return $this->_client->migrateAccount($params);
    }

    /**
     * Modify an account.
     * 
     * @param  string $id    Zimbra ID of account.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function modifyAccount($id, array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;
        return $this->_client->modifyAccount($params, array('id' => $id));
    }

    /**
     * Modifies admin saved searches.
     * Returns the admin saved searches.
     * If {search-query} is empty => delete the search if it exists.
     * If {search-name} already exists => replace with new {search-query}.
     * If {search-name} does not exist => save as a new search.
     * 
     * @param  array $searchs Array of search.
     * @return mix
     */
    public function modifyAdminSavedSearches(array $searchs = array())
    {
        $params = array();
        if(count($searchs))
        {
            $params['search'] = array();
            foreach ($searchs as $key => $value)
            {
                $params['search'][] = array(
                    'name' => $key,
                    '_' => $value,
                );
            }
        }
        return $this->_client->modifyAdminSavedSearches($params);
    }

    /**
     * Modify a calendar resource.
     * Notes:
     *   1. an empty attribute value removes the specified attr.
     *   2. this request is by default proxied to the resources's home server.
     * Access: domain admin sufficient. limited set of attributes that can be updated by a domain admin.
     * 
     * @param  string $id    Zimbra ID.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function modifyCalendarResource($id, array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;
        return $this->_client->modifyCalendarResource($params, array('id' => $id));
    }

    /**
     * Modify Configuration attributes.
     * Note: an empty attribute value removes the specified attr.
     * 
     * @param  array $attrs Attributes.
     * @return mix
     */
    public function modifyConfig(array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;
        return $this->_client->modifyConfig($params);
    }

    /**
     * Modify Class of Service (COS) attributes.
     * Note: an empty attribute value removes the specified attr.
     * 
     * @param  string $id    Zimbra ID.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function modifyCos($id, array $attrs = array())
    {
        $params = array('id' => $id);
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;
        return $this->_client->modifyCos($params);
    }

    /**
     * Changes attributes of the given data source.
     * Only the attributes specified in the request are modified.
     * To change the name, specify "zimbraDataSourceName" as an attribute.
     * Note: this request is by default proxied to the account's home server
     * 
     * @param  string $id         Existing account ID.
     * @param  string $dataSource Data source  ID.
     * @param  array  $attrs      Attributes.
     * @return mix
     */
    public function modifyDataSource($id, $dataSource, array $attrs = array())
    {
        $params = array('dataSource' => array('id' => $dataSource));
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;
        return $this->_client->modifyDataSource($params, array('id' => $id));
    }

    /**
     * Modify constraint (zimbraConstraint) for delegated admin on global config or a COS.
     * If constraints for an attribute already exists, it will be replaced by the new constraints.
     * I <constraint> is an empty element, constraints for the attribute will be removed.
     * 
     * @param  string $type  Target type. Valid values: (account|calresource|cos|dl|group|domain|server|ucservice|xmppcomponent|zimlet|config|global).
     * @param  string $id    ID.
     * @param  string $name  Name.
     * @param  array  $attrs Constaint attributes.
     * @return mix
     */
    public function modifyDelegatedAdminConstraints($type, $id = '', $name = '', array $attrs = array())
    {
        $options = array(
            'type' => in_array($type, $this->_resourceTypes) ? $type : 'account',
        );
        if(!empty($id)) $options['id'] = $id;
        if(!empty($name)) $options['name'] = $name;
        $params = array('dataSource' => array('id' => $dataSource));
        if(count($attrs)) $params['a'] = $attrs;
        return $this->_client->modifyDelegatedAdminConstraints($params, $options);
    }

    /**
     * Modify attributes for a Distribution List.
     * Notes: an empty attribute value removes the specified attr.
     * Access: domain admin sufficient.
     * 
     * @param  string $id    Zimbra ID.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function modifyDistributionList($id, array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;
        return $this->_client->modifyDistributionList($params, array('id' => $id));
    }

    /**
     * Modify attributes for a domain.
     * Note: an empty attribute value removes the specified attr.
     * 
     * @param  string $id    Zimbra ID.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function modifyDomain($id, array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;
        return $this->_client->modifyDomain($params, array('id' => $id));
    }

    /**
     * Modify an LDAP Entry.
     * 
     * @param  string $dn    A valid LDAP DN String (RFC 2253) that identifies the LDAP object.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function modifyLDAPEntry($dn, array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;
        return $this->_client->modifyLDAPEntry($params, array('dn' => $dn));
    }

    /**
     * Modify attributes for a server.
     * Notes:
     *   1. An empty attribute value removes the specified attr.
     *   2. His request is by default proxied to the referenced server.
     * 
     * @param  string $id    Zimbra ID.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function modifyServer($id, array $attrs = array())
    {
        $params = array();
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;
        return $this->_client->modifyServer($params, array('id' => $id));
    }

    /**
     * Modify system retention policy.
     * 
     * @param  array  $policy New policy.
     * @param  string $cos    The name used to identify the COS.
     * @return mix
     */
    public function modifySystemRetentionPolicy(array $policy, $cos = '' )
    {
        $params = array();
        $arrPolicy = $this->_retentionPolicy($policy);
        if(count($arrPolicy))
        {
            $params['policy'] = $arrPolicy;
        }
        if(!empty($cos))
        {
            $params['cos'] = array(
                'by' => 'name',
                '_' => $cos,
            );
        }

        return $this->_client->modifySystemRetentionPolicy($params);
    }

    /**
     * Modify attributes for a UC service.
     * Notes: An empty attribute value removes the specified attr
     * 
     * @param  string $id    Zimbra ID.
     * @param  array  $attrs Attributes.
     * @return mix
     */
    public function modifyUCService($id, array $attrs = array())
    {
        $params = array(
            'id' => $id,
        );
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;
        return $this->_client->modifyUCService($params);
    }

    /**
     * Modify volume.
     * 
     * @param  string $id     Zimbra ID.
     * @param  array  $volume Volume information.
     * @return mix
     */
    public function modifyVolume($id, array $volume = array())
    {
        $params['volume'] = array();
        if(isset($volume['id']))
            $params['volume']['id'] = $volume['id'];
        if(isset($volume['name']))
            $params['volume']['name'] = $volume['name'];
        if(isset($volume['rootpath']))
            $params['volume']['rootpath'] = $volume['rootpath'];
        if(isset($volume['type']) AND in_array((int)$volume['type'], array(1, 2, 10)))
            $params['volume']['id'] = (int) $volume['id'];
        if(isset($volume['compressBlobs']))
            $params['volume']['compressBlobs'] = ((int) $volume['compressBlobs'] > 0) ? 1 : 0;
        if(isset($volume['compressionThreshold']))
            $params['volume']['compressionThreshold'] = (int) $volume['compressionThreshold'];
        if(isset($volume['mgbits']))
            $params['volume']['mgbits'] = (int) $volume['mgbits'];
        if(isset($volume['mbits']))
            $params['volume']['mbits'] = (int) $volume['mbits'];
        if(isset($volume['fgbits']))
            $params['volume']['fgbits'] = (int) $volume['fgbits'];
        if(isset($volume['fbits']))
            $params['volume']['fbits'] = (int) $volume['fbits'];
        if(isset($volume['isCurrent']))
            $params['volume']['isCurrent'] = ((int) $volume['isCurrent'] > 0) ? 1 : 0;

        return $this->_client->modifyVolume($params, array('id' => $id));
    }

    /**
     * Modify Zimlet.
     * 
     * @param  string  $name     Zimlet name.
     * @param  string  $cos      Name of Class Of Service (COS).
     * @param  string  $acl      Acl (grant or deny).
     * @param  string  $status   Status - valid values for valueattribute - enabled|disabled.
     * @param  integer $priority Priority.
     * @return mix
     */
    public function modifyZimlet($name, $cos = '', $acl = 'grant', $status = 'enabled', $priority = 0)
    {
        $params = array(
            'zimlet' => array(
                'name' => $name,
            ),
        );
        if(!empty($cos)) $params['zimlet']['acl']['cos'] = $cos;
        if(!empty($acl) AND in_array($acl, array('grant', 'deny')))
            $params['zimlet']['acl']['acl'] = $acl;
        $status = in_array($status, array('enabled', 'disabled')) ? $status : 'enabled';
        $params['zimlet']['status'] = array('value' => $status);
        $params['zimlet']['priority'] = array('value' => (int) $priority);

        return $this->_client->modifyZimlet($params);
    }

    /**
     * A request that does nothing and always returns nothing.
     * Used to keep an admin session alive.
     * 
     * @return mix
     */
    public function noOp()
    {
        return $this->_client->noOp();
    }

    /**
     * Ping.
     * 
     * @return mix
     */
    public function ping()
    {
        return $this->_client->ping();
    }

    /**
     * Purge the calendar cache for an account.
     * Access: domain admin sufficient.
     * 
     * @param  string $id Zimbra ID.
     * @return mix
     */
    public function purgeAccountCalendarCache($id)
    {
        return $this->_client->purgeAccountCalendarCache(array(), array('id' => $id));
    }

    /**
     * Purges the queue for the given freebusy provider on the current host.
     * 
     * @param  string $name Provider name.
     * @return mix
     */
    public function purgeFreeBusyQueue($name)
    {
        $params = array(
            'provider' => array('name' => $name),
        );
        return $this->_client->purgeFreeBusyQueue($params);
    }

    /**
     * Purges aged messages out of trash, spam, and entire mailbox.
     * (if <mbox> element is omitted, purges all mailboxes on server).
     * 
     * @param  string $id Account ID.
     * @return mix
     */
    public function purgeMessages($id)
    {
        $params = array(
            'mbox' => array('id' => $id),
        );
        return $this->_client->purgeMessages($params);
    }

    /**
     * Push Free/Busy.
     * The request must include either <domain/> or <account/>.
     * When <domain/> is specified in the request, the server will push
     * the free/busy for all the accounts in the domain to the configured free/busy providers.
     * When <account/> list is specified, the server will push the free/busy for
     * the listed accounts to the providers.
     * 
     * @param  array  $domains Domain names specification.
     * @param  string $account Account ID.
     * @return mix
     */
    public function pushFreeBusy(array $domains = array(), $account = '')
    {
        $params = array();
        if(count($domains))
        {
            $params['domain'] = array();
            foreach ($domains as $domain)
            {
                $params['domain'][] = array('name' => $domain);
            }
        }
        if(!empty($account))
        {
            $params['account'] = array('id' => $account);
        }
        return $this->_client->pushFreeBusy($params);
    }

    /**
     * Query WaitSet.
     * This API dumps the internal state of all active waitsets.
     * It is intended for debugging use only and should not be used for production uses.
     * This API is not guaranteed to be stable between releases in any way
     * and might be removed without warning.
     * 
     * @param  string $waitSet WaitSet ID.
     * @return mix
     */
    public function queryWaitSet($waitSet)
    {
        return $this->_client->queryWaitSet(array(), array('waitSet' => $waitSet));
    }

    /**
     * ReIndex.
     * Access: domain admin sufficient.
     * Note: This request is by default proxied to the account's home server.
     * Note: Only one of {ids} and {types} may be specified.
     * 
     * @param  string $id     Account ID.
     * @param  string $action Action to perform. start: (start) reindexing. (status): show reindexing progress. (cancel): cancel reindexing.
     * @param  string $types  Array of types. Valid values: conversation|message|contact|appointment|task|note|wiki|document.
     * @param  string $ids    Array of IDs to re-index.
     * @return mix
     */
    public function reIndex($id, $action = 'status', array $types = array(), array $ids = array())
    {
        $validTypes = array('all', 'conversation', 'message', 'contact', 'appointment', 'task', 'note', 'wiki', 'document');
        $typesStr = $this->_commaAttributes($types, $validTypes);
        $params = array(
            'mbox' => array(
                'id' => $id,
                'types' => empty($typesStr) ? 'all' : $typesStr,
                'ids' => implode(',', $ids),
            ),
        );

        $action = in_array($action, array('start', 'status', 'cancel')) ? $action : 'status';
        return $this->_client->reIndex($params, array('action' => $action));
    }

    /**
     * Recalculate Mailbox counts.
     * Forces immediate recalculation of total mailbox quota usage and all folder unread
     * and size counts.
     * Access: domain admin sufficient.
     * Note: this request is by default proxied to the account's home server.
     * 
     * @param  string $id Account ID.
     * @return mix
     */
    public function recalculateMailboxCounts($id)
    {
        $params = array(
            'mbox' => array('id' => $id),
        );
        return $this->_client->recalculateMailboxCounts($params);
    }

    /**
     * Reload LocalConfig.
     * 
     * @return mix
     */
    public function reloadLocalConfig()
    {
        return $this->_client->reloadLocalConfig();
    }

    /**
     * Reloads the memcached client configuration on this server.
     * Memcached client layer is reinitialized accordingly.
     * Call this command after updating the memcached server list, for example.
     * 
     * @return mix
     */
    public function reloadMemcachedClientConfig()
    {
        return $this->_client->reloadMemcachedClientConfig();
    }

    /**
     * Remove Account Alias.
     * Access: domain admin sufficient.
     * Note: this request is by default proxied to the account's home server.
     * 
     * @param  string $alias Account alias.
     * @param  string $id    Zimbra ID.
     * @return mix
     */
    public function removeAccountAlias($alias, $id = '')
    {
        $options = array(
            'alias' => $alias,
        );
        if(!empty($id)) $options['id'] = $id;
        return $this->_client->removeAccountAlias(array(), $options);
    }

    /**
     * Removes one or more custom loggers.
     * If both the account and logger are specified, removes the given account logger if it exists.
     * If only the account is specified or the category is "all",
     * removes all custom loggers from that account.
     * If only the logger is specified, removes that custom logger from all accounts.
     * If neither element is specified, removes all custom loggers from all accounts
     * on the server that receives the request.
     * 
     * @param  string $account  The name used to identify the account.
     * @param  string $category Name of the logger category.
     * @param  string $level    Level of the logging (error|warn|info|debug|trace).
     * @return mix
     */
    public function removeAccountLogger($account, $category = '', $level = '')
    {
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        if(!empty($category) OR !empty($level))
        {
            $params['logger'] = array(
                'category' => $category,
                'level' => in_array($level, array('error', 'warn', 'info', 'debug', 'trace')) ? $level : 'error',
            );
        }
        return $this->_client->removeAccountLogger($params);
    }

    /**
     * Remove a device or remove all devices attached to an account.
     * This will not cause a reset of sync data, but will cause a reset of policies on the next sync.
     * 
     * @param  string $account  Account email address.
     * @param  string $deviceId Device ID. Note - if not supplied ALL devices will be removed.
     * @return mix
     */
    public function removeDevice($account, $deviceId = '')
    {
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        if(!empty($deviceId))
        {
            $params['device'] = array('id' => $deviceId);
        }
        return $this->_client->removeDevice($params);
    }

    /**
     * Remove Distribution List Alias.
     * Access: domain admin sufficient.
     * 
     * @param  string $id    Zimbra ID
     * @param  string $alias Distribution list alias.
     * @return mix
     */
    public function removeDistributionListAlias($id, $alias)
    {
        $options = array(
            'id' => $id,
            'alias' => $alias,
        );
        return $this->_client->removeDistributionListAlias(array(), $options);
    }

    /**
     * Remove Distribution List Member.
     * Unlike add, remove of a non-existent member causes an exception and no modification to the list. 
     * Access: domain admin sufficient.
     * 
     * @param  string $id  Zimbra ID
     * @param  array  $dlm Members.
     * @return mix
     */
    public function removeDistributionListMember($id, array $dlm)
    {
        $params['dlm'] = array();
        foreach ($dlm as $member)
        {
            $params['dlm'][] = (string) $member;
        }
        return $this->_client->removeDistributionListMember($params, array('id' => $id));
    }


    /**
     * Rename Account.
     * Access: domain admin sufficient.
     * Note: this request is by default proxied to the account's home server. 
     * 
     * @param  string $id      Zimbra ID
     * @param  array  $newName New account name.
     * @return mix
     */
    public function renameAccount($id, $newName)
    {
        $options = array(
            'id' => $id,
            'newName' => $newName,
        );
        return $this->_client->renameAccount(array(), $options);
    }

    /**
     * Rename Calendar Resource.
     * Access: domain admin sufficient.
     * Note: this request is by default proxied to the account's home server. 
     * 
     * @param  string $id      Zimbra ID
     * @param  array  $newName New Calendar Resource name.
     * @return mix
     */
    public function renameCalendarResource($id, $newName)
    {
        $options = array(
            'id' => $id,
            'newName' => $newName,
        );
        return $this->_client->renameCalendarResource(array(), $options);
    }

    /**
     * Rename Class of Service (COS).
     * 
     * @param  string $id      Zimbra ID
     * @param  array  $newName New COS name.
     * @return mix
     */
    public function renameCos($id, $newName)
    {
        $params = array(
            'id' => $id,
            'newName' => $newName,
        );
        return $this->_client->renameCos($params);
    }

    /**
     * Rename Distribution List.
     * Access: domain admin sufficient.
     * 
     * @param  string $id      Zimbra ID
     * @param  array  $newName New Distribution List name.
     * @return mix
     */
    public function renameDistributionList($id, $newName)
    {
        $options = array(
            'id' => $id,
            'newName' => $newName,
        );
        return $this->_client->renameDistributionList(array(), $options);
    }

    /**
     * Rename LDAP Entry.
     * 
     * @param  string $dn    A valid LDAP DN String (RFC 2253) that identifies the LDAP object
     * @param  array  $newDn New DN - a valid LDAP DN String (RFC 2253) that describes the new DN to be given to the LDAP object.
     * @return mix
     */
    public function renameLDAPEntry($dn, $newDn)
    {
        $options = array(
            'dn' => $dn,
            'newDn' => $newDn,
        );
        return $this->_client->renameLDAPEntry(array(), $options);
    }

    /**
     * Rename Unified Communication Service.
     * 
     * @param  string $id      Zimbra ID
     * @param  array  $newName New UC Service name.
     * @return mix
     */
    public function renameUCService($id, $newName)
    {
        $params = array(
            'id' => $id,
            'newName' => $newName,
        );
        return $this->_client->renameUCService($params);
    }

    /**
     * Removes all account loggers and reloads /opt/zimbra/conf/log4j.properties.
     * 
     * @return mix
     */
    public function resetAllLoggers()
    {
        return $this->_client->resetAllLoggers();
    }

    /**
     * Resume sync with a device or all devices attached to an account if currently suspended.
     * This will cause a policy reset, but will not reset sync data.
     * 
     * @param  string $account The name used to identify the account.
     * @param  string $device  Device ID.
     * @return mix
     */
    public function resumeDevice($account, $device = '')
    {
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        if(!empty($device))
        {
            $params['device'] = array(
                'id' => $device,
            );
        }
        return $this->_client->resumeDevice($params);
    }


    /**
     * Revoke a right from a target that was previously granted to an individual or group grantee.
     * 
     * @param  string $target  The name used to identify the target.
     * @param  string $type    Target type. Valid values (account|calresource|cos|dl|group|domain|server|ucservice|xmppcomponent|zimlet|config|global).
     * @param  array  $grantee Grantee selector.
     * @param  array  $right   Right selector.
     * @return mix
     */
    public function revokeRight($target, $type, array $grantee, array $right)
    {
        $params = array(
            'target' => array(
                'type' => in_array($type, $this->_resourceTypes) ? $type : 'account',
                'by' => 'name',
                '_' => $target,
            ),
            'grantee' => array(),
            'right' => array(),
        );
        if(isset($grantee['_']))
            $params['grantee']['_'] = $grantee['_'];
        $gTypes = array('usr', 'grp', 'egp', 'all', 'dom', 'gst', 'key', 'pub', 'email');
        if(isset($grantee['type']) AND in_array($grantee['type'], $gTypes))
            $params['grantee']['type'] = $grantee['type'];
        if(isset($grantee['by']) AND in_array($grantee['by'], array('id', 'name')))
            $params['grantee']['by'] = $grantee['by'];
        if(isset($grantee['secret']))
            $params['grantee']['secret'] = $grantee['secret'];
        if(isset($grantee['all']))
            $params['grantee']['all'] = ((int) $grantee['all'] > 0) ? 1 : 0;

        if(isset($right['_']))
            $params['right']['_'] = $right['_'];
        if(isset($grantee['deny']))
            $params['grantee']['deny'] = ((int) $grantee['deny'] > 0) ? 1 : 0;
        if(isset($grantee['canDelegate']))
            $params['grantee']['canDelegate'] = ((int) $grantee['canDelegate'] > 0) ? 1 : 0;
        if(isset($grantee['disinheritSubGroups']))
            $params['grantee']['disinheritSubGroups'] = ((int) $grantee['disinheritSubGroups'] > 0) ? 1 : 0;
        if(isset($grantee['subDomain']))
            $params['grantee']['subDomain'] = ((int) $grantee['subDomain'] > 0) ? 1 : 0;

        return $this->_client->revokeRight($params);
    }

    /**
     * Runs the server-side unit test suite.
     * If <test>'s are specified, then run the requested tests (instead of the standard test suite).
     * Otherwise the standard test suite is run.
     * 
     * @param  string $tests Array test name.
     * @return mix
     */
    public function runUnitTests(array $tests = array())
    {
        $params = array();
        if(count($tests))
        {
            $params['test'] = array();
            foreach ($tests as $test)
            {
                $params['test'][] = $test;
            }
        }
        return $this->_client->runUnitTests($params);
    }

    /**
     * Search Accounts.
     * Access: domain admin sufficient (a domain admin can't specify "domains" as a type).
     * 
     * @param  string  $query  Query string - should be an LDAP-style filter string (RFC 2254).
     * @param  string  $domain The domain name to limit the search to.
     * @param  bool    $apply  Flag whether or not to apply the COS policy to account. Specify 0 (false) if only requesting attrs that aren't inherited from COS.
     * @param  array   $types  Array of types to return. Legal values are: accounts|resources (default is accounts).
     * @param  array   $attrs  Array of attrs to return ("displayName", "zimbraId", "zimbraAccountStatus").
     * @param  string  $sort   Name of attribute to sort on. Default is the account name.
     * @param  bool    $asc    Whether to sort in ascending order. Default is 1 (true).
     * @param  integer $limit  The maximum number of accounts to return (0 is default and means all).
     * @param  integer $offset The starting offset (0, 25, etc).
     * @return mix
     */
    public function searchAccounts($query, $domain = '', $apply = TRUE, array $types = array('accounts'), array $attrs = array(), $sortBy = '', $asc = TRUE, $limit = 0, $offset = 0)
    {
        $options = array(
            'query' => $query,
            'limit' => (int) $limit,
            'offset' => (int) $offset,
            'applyCos' => ((bool) $apply) ? 1 : 0,
            'sortAscending' => ((bool) $asc) ? 1 : 0,
        );
        if(!empty($domain)) $options['domain'] = $domain;
        if(!empty($sortBy)) $options['sortBy'] = $sortBy;
        $typesStr = $this->_commaAttributes($types, array('accounts', 'resources'));
        if(!empty($typesStr)) $options['types'] = $typesStr;
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr)) $options['attrs'] = $attrsStr;
        return $this->_client->searchAccounts(array(), $options);
    }

    /**
     * Search Auto Prov Directory.
     * Only one of <name> or <query> can be provided.
     * If neither is provided, the configured search filter for auto provision will be used.
     * 
     * @param  string  $key     Name of attribute for the key.
     * @param  string  $domain  Domain name to limit the search to (do not use if searching for domains).
     * @param  string  $query   Query string - should be an LDAP-style filter string (RFC 2254).
     * @param  string  $name    Name to fill the auto provisioning search template configured on the domain.
     * @param  integer $max     Maximum results that the backend will attempt to fetch from the directory before returning an account.TOO_MANY_SEARCH_RESULTS error.
     * @param  bool    $refresh Refresh - whether to always re-search in LDAP even when cached entries are available. 0 (false) is the default.
     * @param  array   $attrs   Array of attributes.
     * @param  integer $limit   The number of accounts to return per page (0 is default and means all).
     * @param  integer $offset  The starting offset (0, 25, etc).
     * @return mix
     */
    public function searchAutoProvDirectory($key, $domain, $query = '', $name = '', $max = 0, $refresh = FALSE, array $attrs = array(), $limit = 0, $offset = 0)
    {
        $options = array(
            'keyAttr' => $key,
            'limit' => (int) $limit,
            'offset' => (int) $offset,
            'maxResults' => (int) $max,
            'refresh' => ((bool) $refresh) ? 1 : 0,
            'sortAscending' => ((bool) $asc) ? 1 : 0,
        );
        if(!empty($query)) $options['query'] = $query;
        if(!empty($name)) $options['name'] = $name;
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr)) $options['attrs'] = $attrsStr;

        $params = array(
            'domain' => array(
                'by' => 'name',
                '_' => $domain,
            ),
        );
        return $this->_client->searchAutoProvDirectory($params, $options);
    }

    /**
     * Search for Calendar Resources.
     * Access: domain admin sufficient.
     * 
     * @param  string  $search Search filter condition.
     * @param  string  $domain The domain name to limit the search to.
     * @param  bool    $apply  Flag whether or not to apply the COS policy to calendar resource. Specify 0 (false) if only requesting attrs that aren't inherited from COS.
     * @param  array   $attrs  Array of attributes.
     * @param  string  $sort   Name of attribute to sort on. default is the calendar resource name.
     * @param  bool    $asc    Whether to sort in ascending order. Default is 1 (true).
     * @param  integer $limit  The maximum number of calendar resources to return (0 is default and means all).
     * @param  integer $offset The starting offset (0, 25, etc).
     * @return mix
     */
    public function searchCalendarResources(array $conds = array(), $domain = '', $apply = TRUE, array $attrs = array(), $sort = '', $asc = TRUE, $limit = 0, $offset = 0)
    {
        $options = array(
            'limit' => (int) $limit,
            'offset' => (int) $offset,
            'applyCos' => ((bool) $apply) ? 1 : 0,
            'sortAscending' => ((bool) $asc) ? 1 : 0,
        );
        if(!empty($domain)) $options['domain'] = $domain;
        if(!empty($sort)) $options['sortBy'] = $sort;
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr)) $options['attrs'] = $attrsStr;

        $params['searchFilter'] = array();
        if(isset($conds['cond']) AND is_array($conds['cond']))
        {
            $params['searchFilter']['cond'] = $this->_processCondFilter($conds['cond']);
        }
        elseif(isset($conds['conds']) AND is_array($conds['conds']))
        {
            $params['searchFilter']['conds'] = $this->_processCondsFilter($conds['conds']);
        }
        return $this->_client->searchCalendarResources($params, $options);
    }

    /**
     * Search directory.
     * Access: domain admin sufficient (though a domain admin can't specify "domains" as a type).
     * 
     * @param  string  $query       Query string - should be an LDAP-style filter string (RFC 2254).
     * @param  string  $domain      The domain name to limit the search to.
     * @param  bool    $applyCos    Flag whether or not to apply the COS policy to account. Specify 0 (false) if only requesting attrs that aren't inherited from COS.
     * @param  bool    $applyConfig Whether or not to apply the global config attrs to account. specify 0 (false) if only requesting attrs that aren't inherited from global config.
     * @param  bool    $countOnly   Whether response should be count only. Default is 0 (false).
     * @param  array   $attrs       Array of attributes.
     * @param  array   $types       Array of types to return. Legal values are: accounts|distributionlists|aliases|resources|domains|coses. (default is accounts)
     * @param  integer $max         Maximum results that the backend will attempt to fetch from the directory before returning an account.TOO_MANY_SEARCH_RESULTS error.
     * @param  string  $sort        Name of attribute to sort on. Default is the account name.
     * @param  bool    $asc         Whether to sort in ascending order. Default is 1 (true).
     * @param  integer $limit       The maximum number of accounts to return (0 is default and means all).
     * @param  integer $offset      The starting offset (0, 25, etc).
     * @return mix
     */
    public function searchDirectory($query = '', $domain = '', $applyCos = TRUE, $applyConfig = TRUE, $countOnly = FALSE, array $attrs = array(), array $types = array('accounts'), $max = 0, $sort = '', $asc = TRUE, $limit = 0, $offset = 0)
    {
        $options = array(
            'limit' => (int) $limit,
            'offset' => (int) $offset,
            'maxResults' => (int) $max,
            'applyCos' => ((bool) $applyCos) ? 1 : 0,
            'applyConfig' => ((bool) $applyConfig) ? 1 : 0,
            'sortAscending' => ((bool) $asc) ? 1 : 0,
            'countOnly' => ((bool) $countOnly) ? 1 : 0,
        );
        if(!empty($query)) $options['query'] = $query;
        if(!empty($domain)) $options['domain'] = $domain;
        if(!empty($sort)) $options['sortBy'] = $sort;
        $attrsStr = $this->_commaAttributes($attrs);
        if(!empty($attrsStr)) $options['attrs'] = $attrsStr;
        $validTypes = array('accounts', 'distributionlists', 'aliases', 'resources', 'domains', 'coses');
        $typesStr = $this->_commaAttributes($types, $validTypes);
        if(!empty($typesStr)) $options['types'] = $typesStr;

        return $this->_client->searchDirectory(array(), $options);
    }

    /**
     * Search Global Address Book (GAL).
     * Notes: admin verison of mail equiv. Used for testing via zmprov.
     * 
     * @param  string  $domain    Domain name.
     * @param  string  $type      Type of addresses to search. Valid values: all|account|resource|group.
     * @param  string  $name      Name.
     * @param  string  $galAcctId GAL account ID.
     * @param  integer $limit     The maximum number of entries to return (0 is default and means all).
     * @return mix
     */
    public function searchGal($domain, $type = 'account', $name = '', $galAcctId = '', $limit = 0)
    {
        $type = in_array($type, array('all', 'account', 'resource', 'group')) ? $type : 'account';
        $options = array(
            'domain' => $domain,
            'limit' => (int) $limit,
            'type' => $type,
        );
        if(!empty($name)) $options['name'] = $name;
        if(!empty($galAcctId)) $options['galAcctId'] = $galAcctId;
        if(!empty($sort)) $options['sortBy'] = $sort;

        return $this->_client->searchGal(array(), $options);
    }

    /**
     * Set current volume.
     * Notes: Each SetCurrentVolumeRequest can set only one current volume type.
     * 
     * @param  integer $id   ID.
     * @param  integer $type Volume type: 1 (primary message), 2 (secondary message) or 10 (index).
     * @return mix
     */
    public function setCurrentVolume($id, $type)
    {
        $options = array(
            'id' => (int) $id,
            'type' => in_array((int) $type, array(1, 2, 10)) ? $type : 1,
        );
        return $this->_client->setCurrentVolume(array(), $options);
    }

    /**
     * Set Password.
     * Access: domain admin sufficient.
     * Note: this request is by default proxied to the account's home server.
     * 
     * @param  string $id       Zimbra ID.
     * @param  string $newPassword New password.
     * @return mix
     */
    public function setPassword($id, $newPassword)
    {
        $options = array(
            'id' => $id,
            'newPassword' => $newPassword,
        );
        return $this->_client->setPassword(array(), $options);
    }

    /**
     * Suspend a device or all devices attached to an account from further sync actions.
     * 
     * @param  string $account The name used to identify the account.
     * @param  string $device  Device ID.
     * @return mix
     */
    public function suspendDevice($account, $device = '')
    {
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
        );
        if(!empty($device))
        {
            $params['device'] = array('id' => $device);
        }
        return $this->_client->suspendDevice($params);
    }

    /**
     * Sync GalAccount.
     * Notes:
     *   1. If fullSync is set to false (or unset) the default behavior
     *      is trickle sync which will pull in any new contacts or modified contacts since last sync.
     *   2. If fullSync is set to true, then the server will go through all the contacts
     *      that appear in GAL, and resolve deleted contacts in addition to new or modified ones.
     *   3. If reset attribute is set, then all the contacts will be populated again,
     *      regardless of the status since last sync.
     *      Reset needs to be done when there is a significant change in the configuration,
     *      such as filter, attribute map, or search base.
     * 
     * @param  array  $datasource SyncGalAccount data source specifications.
     * @return mix
     */
    public function syncGalAccount(array $galAccounts = array())
    {
        $params = array();
        if(count($galAccounts))
        {
            $params['account'] = array();
            foreach ($galAccounts as $id => $datasources)
            {
                $params['account']['id'] = $id;
                if(count($datasources))
                {
                    $params['account']['datasource'] = array();
                    foreach ($datasources as $key => $datasource)
                    {
                        $arr = array(
                            'by' => 'name',
                            'fullSync' => (isset($datasource['fullSync']) AND (int) $datasource['fullSync'] > 0 ) ? 1 : 0,
                            'reset' => (isset($datasource['reset']) AND (int) $datasource['reset'] > 0 ) ? 1 : 0,
                            '_' => isset($datasource['_']) ? $datasource['_'] : NULL,
                        );
                        $params['account']['datasource'][] = $arr;
                    }
                }
            }
        }
        return $this->_client->syncGalAccount($params);
    }

    /**
     * Undeploy Zimlet.
     * 
     * @param  string $name   Zimlet name.
     * @param  string $action Action.
     * @return mix
     */
    public function undeployZimlet($name, $action = '')
    {
        $options = array(
            'name' => $name,
        );
        if(!empty($action)) $options['action'] = $action;
        return $this->_client->undeployZimlet(array(), $options);
    }

    /**
     * Update device status.
     * 
     * @param  string $account The name used to identify the account.
     * @param  string $device  Device ID.
     * @param  string $status  Device status - enabled|disabled|locked|wiped.
     * @return mix
     */
    public function updateDeviceStatus($account, $device, $status = 'enabled')
    {
        $status = in_array($status, array('enabled', 'disabled', 'locked', 'wiped')) ? $status : 'enabled';
        $params = array(
            'account' => array(
                'by' => 'name',
                '_' => $account,
            ),
            'device' => array(
                'id' => $device,
                'status' => $status,
            ),
        );
        return $this->_client->updateDeviceStatus($params);
    }

    /**
     * Generate a new Cisco Presence server session ID and persist the newly generated session id
     * in zimbraUCCiscoPresenceSessionId attribute for the specified UC service..
     * 
     * @param  string $ucservice The UC service name.
     * @param  string $username  App username.
     * @param  string $password  App password.
     * @param  array  $attrs     Attributes.
     * @return mix
     */
    public function updatePresenceSessionId($ucservice, $username, $password, array $attrs = array())
    {
        $params = array(
            'ucservice' => array(
                'by' => 'name',
                '_' => $ucservice,
            ),
            'username' => $username,
            'password' => $password,
        );
        $attributes = $this->_attributes($attrs);
        if(count($attributes)) $params['a'] = $attributes;
        return $this->_client->updatePresenceSessionId($params);
    }

    /**
     * Upload domain certificate.
     * 
     * @param  string $certAid      Certificate attach ID.
     * @param  string $certFilename Certificate name.
     * @param  string $keyAid       Key attach ID.
     * @param  string $keyFilename  Key name.
     * @return mix
     */
    public function uploadDomCert($certAid, $certFilename, $keyAid, $keyFilename)
    {
        $options = array(
            'cert.aid' => $certAid,
            'cert.filename' => $certFilename,
            'key.aid' => $keyAid,
            'key.filename' => $keyFilename,
        );
        return $this->_client->uploadDomCert(array(), $options);
    }

    /**
     * Upload proxy CA.
     * 
     * @param  string $aid      Certificate attach ID.
     * @param  string $filename Certificate name.
     * @return mix
     */
    public function uploadProxyCA($aid, $filename)
    {
        $options = array(
            'cert.aid' => $certAid,
            'cert.filename' => $certFilename,
        );
        return $this->_client->uploadProxyCA(array(), $options);
    }

    /**
     * Verify Certificate Key.
     * 
     * @param  string $cert    Certificate.
     * @param  string $privkey Private key.
     * @return mix
     */
    public function verifyCertKey($cert = '', $privkey = '')
    {
        $options = array(
            'cert' => $cert,
            'privkey' => $privkey,
        );
        return $this->_client->verifyCertKey(array(), $options);
    }

    /**
     * Verify index.
     * 
     * @param  string $id Account ID.
     * @return mix
     */
    public function verifyIndex($id)
    {
        $params['mbox'] = array(
            'id' => $id,
        );
        return $this->_client->verifyIndex($params);
    }

    /**
     * Verify Store Manager.
     * 
     * @param  integer $fileSize.
     * @param  integer $num.
     * @param  bool    $checkBlobs.
     * @return mix
     */
    public function verifyStoreManager($fileSize = 0, $num = 0, $checkBlobs = FALSE)
    {
        $options = array(
            'fileSize' => (int) $fileSize,
            'num' => (int) $num,
            'checkBlobs' => (bool) $checkBlobs,
        );
        return $this->_client->verifyStoreManager(array(), $options);
    }

    /**
     * Version Check.
     * 
     * @param  integer $action Action. Either check or status.
     * @return mix
     */
    public function versionCheck($action = 'check')
    {
        $action = in_array($action, array('check', 'status')) ? $action : 'check';
        $options = array(
            'action' => $action,
        );
        return $this->_client->versionCheck(array(), $options);
    }

    /**
     * Process attributes params.
     *
     * @param  array $attrs Array of attribute.
     * @return mix
     */
    protected function _attributes(array $attrs, $name = 'n', array $inArray = array())
    {
        $params = array();
        if(count($attrs))
        {
            foreach ($attrs as $key => $value)
            {
                $params[] = array(
                    $name => $key,
                    '_' => $value,
                );
            }
        }
        return $params;
    }

    protected function _commaAttributes(array $attrs = array(), array $inArray = array())
    {
        $commaStr = '';
        foreach ($attrs as $attr)
        {
            if(count($inArray) AND !in_array($attr, $inArray))
            {
                continue;
            }
            if(empty($commaStr))
                $commaStr = $attr;
            else
                $commaStr .= ','.$attr;
        }
        return $commaStr;
    }

    protected function _processCondFilter(array $cond)
    {
        $result = array();
        $condKeys = array('not', 'attr', 'op', 'value');
        foreach ($cond as $key => $value)
        {
            if(in_array($key, $condKeys))
            {
                $filter[$key] = $value;
            }
        }
        return $result;
    }

    protected function _processCondsFilter(array $conds)
    {
        $result = array();
        if(isset($conds['not']))
        {
            $not = (int)$conds['not'];
            $result['not'] = ($not > 0) ? 1 : 0;
            unset($conds['not']);
        }
        if(isset($conds['or']))
        {
            $or = (int)$conds['or'];
            $result['or'] = ($or > 0) ? 1 : 0;
            unset($conds['or']);
        }
        if(isset($conds['cond']) AND is_array($conds['cond']))
        {
            $result['cond'] = $this->_processCondFilter($conds['cond']);
            unset($conds['cond']);
        }
        if(isset($conds['conds']) AND is_array($conds['conds']))
        {
            $result['conds'] = $this->_processCondsFilter($conds['conds']);
            unset($conds['conds']);
        }
        return $result;
    }

    /**
     * Process interest types.
     *
     * @param  array $types Array of interest type.
     * @return mix
     */
    protected function _defTypes(array $types = array())
    {
        $validTypes = array('f', 'm', 'c', 'a', 't', 'd', 'all');
        $defTypes = '';
        foreach ($types as $type)
        {
            if(empty($defTypes))
            {
                $defTypes = $type;
            }
            else
            {
                $defTypes .= ','.$type;
            }
        }
        return $defTypes;
    }

    /**
     * Process WaitSet.
     *
     * @param  array $set Array of WaitSet.
     * @return mix
     */
    protected function _waitSets(array $set = array())
    {
        $waitSets = array();
        foreach ($set as $param)
        {
            $arr = array();
            if(isset($param['name'])) $arr['name'] = $param['name'];
            if(isset($param['id'])) $arr['id'] = $param['id'];
            if(isset($param['token'])) $arr['token'] = $param['token'];
            if(isset($param['types'])) $arr['types'] = $this->_defTypes($param['types']);
            if(count($arr))
            {
                $waitSets[] = $arr;
            }
        }
        return $waitSets;
    }
}