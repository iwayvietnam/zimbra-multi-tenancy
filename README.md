# Zimbra Multi-tenancy

**Add Multi-tenancy and Domain Administration & Role-based Delegate features to Zimbra OSE.**

Copyright (C) 2013 [iWay Vietnam] (http://www.iwayvietnam.com/)

##Licensing
[GNU Affero General Public License] (http://www.gnu.org/licenses/agpl-3.0.html)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

##Backgrounds

###Community Needs background
  * As mentioned in http://www.zimbra.com/products/compare_products.html, Multi-tenancy and Domain Administration & Role-based Delegate features is just available in Zimbra Network Edition.
  * Since a lot of sites deployed Zimbra Open Source Edition, especially for providing cloud-based services, still need these features.
  * This project is contributed to fill this gap. All contributions are welcome.

###Technical background
  * Based technologies: LAMP, using Drupal framework
  * Programming language: PHP

##Functionality Details

###For supervisors

####Tenant management
  * Create a new tenant (create tenant-account and set permissions at the same time)
  * Update an existing tenant
  * Delete an existing tenant

###For tenants
  
####Domain management
  * Create a new domain (create domain-admin-account and set permissions at the same time)
  * Update an existing domain
  * Delete an existing domain


###For domain admins

####Group/Distribution List management
  * Create a new distribution list
  * Update an existing distribution list
  * Delete an existing distribution list
  
####Mailbox/Account management
  * Create a new mailbox
  * Update an existing mailbox
  * Delete an existing mailbox
  * Reset mailbox password

####Alias management
  * Create a new alias
  * Update an existing alias
  * Delete an existing alias

**Many other functionalities would be added later.**

##Credits
Special thanks to: iWay Development team
