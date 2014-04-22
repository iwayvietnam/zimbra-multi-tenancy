
## System Requirements
* LAMP Stack
* Zimbra Mail Server
* Drupal >= 7.22
* [References module](https://drupal.org/project/references)
* [Field Hidden module](https://drupal.org/project/field_hidden)
* [Zimbra API](https://github.com/iwayvietnam/zimbra-api-php) (libraries) in PHP


## Installation
* Add 'zimbra-multi-tenancy' module to Drupal's module folder
* Change the user and group ownership of 'zimbra-multi-tenancy' folder to 'apache'
* Go to the module folder ('sites/all/modules/zimbra-multi-tenancy')
* Switch to the branch 'UsingWebServiceAPI'
* Add [Zimbra API](https://github.com/iwayvietnam/zimbra-api-php) module to Drupal's module folder
* Go to the module folder ('sites/all/modules/zimbra-api-php')
* Copy all content in 'zimbra-api-php' folder to the 'sites/all/modules/zimbra-multi-tenancy/vendor/zap' folder
* Change the user and group ownership of 'zimbra-multi-tenancy/vendor' folder to 'apache'
* Enable 'zimbra-multi-tenancy' module
* Click on the 'Zimbra multi tenancy' item
* Enjoy


## Authors
* Lê Anh Quyến (leanhquyen@gmail.com)
* Integrated with Zimbra API (libraries) in PHP by Nguyen Van Nguyen - nguyennv1981@gmail.com
