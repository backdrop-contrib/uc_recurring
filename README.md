uc_recurring
============

uc_recurring is a module that provides recurring billing for Ubercart.

Installation
------------

- Install this module using the [official Backdrop CMS instructions](https://backdropcms.org/guide/modules).

Usage
-----

This module allows you to add handle recurring payments in ubercart.

Step 1: Enable module on your drupal site.

Step 2: Setup Recurring Payments:
(This step can be skipped if you do not accept payments on site and you do not
have the uc_payment module enabled)
  - Requirement: Installed and setup payment gateways in ubercart.
  - Go to:
    "Store administration" -> "Configuration" -> "Payment Settings" -> "Edit" -> "Recurring payments"
  - Select payment methods that should be allows to process recurring payments,
    only the methods selected will be shown on the checkout page when a order
    includes a recurring product.

Step 3: Enable a module that triggers recurring payments on certain events.
  - Recurring Products (uc_recurring_product) - product specific recurring fees (e.g. subscriptions)
  - Recurring Order (uc_recurring_order) - entire order is recurring.

Your site should now be ready to accept orders with recurring payments.

Testing
-------

Ubercart includes a test payment gateway called test_gateway. This gateway
emulates a credit card payment gateway and uc_recurring supports this gateway.

If you are attempting to test if uc_recurring is setup correctly this is a good
gateway to initally test against before setting up your own live gateway.

If need to take snapshots of live databases with recurring fees setup ensure
that cron is not running on your test site or recurring payments may be
triggered from your test and live installs.

A simple way to ensure recurring payments are not triggered on cron runs is to
add the following php to your test sites settings.php

<?php
// disable ubercart recurring payments
$conf['uc_recurring_trigger_renewals'] = FALSE;
?>

Developers
----------

This modules includes the file uc_recurring.api.php which is an attempt to
define all the hooks this module exposes to developers.

To integrate with a new payment gateway you should first look at the
hook_recurring_info() function as this defines all the details uc_recurring
needs to work with a new gateway.

Current Maintainers
-------------------

- [Hosef](https://github.com/hosef)
- [Alanmels](https://github.com/alanmels)
- Seeking additional maintainers.

Credits
-------

- Ported to Backdrop CMS by [Hosef](https://github.com/hosef/).
- Originally written for Drupal by [Ryan Szrama](https://www.drupal.org/u/rszrama)
- Currently maintained for Drupal by [Chris Hood](https://www.drupal.org/u/univate).

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
