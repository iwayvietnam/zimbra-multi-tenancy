; $Id: README.txt,v 1.0 2013/03/02 07:32:18 miglius Exp $

Implements Zimbra multi-tenacy of Zimbra mail.


FEATURES
________

1. Tenant management
  * Create a new tenant (create tenant-account and set permissions at the same time)
  * Update an existing tenant
  * Delete an existing tenant

2. Domain management
  * Create a new domain (create domain-admin-account and set permissions at the same time)
  * Update an existing domain
  * Delete an existing domain

3. Group/Distribution List management
  * Create a new distribution list
  * Update an existing distribution list
  * Delete an existing distribution list
  
4. Mailbox/Account management
  * Create a new mailbox
  * Update an existing mailbox
  * Delete an existing mailbox
  * Reset mailbox password

5. Alias management
  * Create a new alias
  * Update an existing alias
  * Delete an existing alias

6. Integrated with Zimbra API (libraries) in PHP.


REQUIREMENTS
____________

- Drupal 7.x;
- Zimbra server
- References module (https://drupal.org/project/references)
- Field Hidden module (https://drupal.org/project/field_hidden)

INSTALLATION
____________

Add this module to the modules folder (sites/all/modules).
Enable it.
Go to Configuration->System->Zimbra settings ('admin/config/system/zimbra-tenancy') and configure to meet your needs.

AUTHOR
______

- Lê Anh Quyến
Email: leanhquyen@gmail.com

Integrated with Zimbra API (libraries) in PHP by Nguyen Van Nguyen - nguyennv1981@gmail.com
