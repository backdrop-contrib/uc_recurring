<?php
// $Id$

/**
 * @file
 * Uc recurring implementation for the Paypal module.
 */

/**
 * Implementation of hook_recurring_info().
 */
function uc_recurring_uc_paypal_recurring_info() {
  $items['paypal_wps'] = array(
    'name' => t('Paypal website payments standard'),
    'payment method' => 'paypal_wps',
    'fee handler' => 'paypal_wps',
    'module' => 'UC recurring',
    'renew' => 'uc_recurring_paypal_wps_renew',
    'process' => 'uc_recurring_paypal_wps_process',
  );

  $items['paypal_wpp'] = array(
    'name' => t('PayPal website payments pro'),
    'payment method' => 'paypal_wpp',
    'fee handler' => 'paypal_wpp',
    'module' => 'UC recurring',
    'renew' => 'uc_recurring_paypal_wpp_renew',
    'process' => 'uc_recurring_paypal_wpp_process',
  );
  return $items;
}

/**
 * Paypal website payments standard process.
 */
function uc_recurring_paypal_wps_process($order, $fee) {
  return TRUE;
}

/**
 * Paypal website payments standard renew.
 */
function uc_recurring_paypal_wps_renew($order, $fee) {
  return TRUE;
}

/**
 * PayPal website payments pro process.
 */
function uc_recurring_paypal_wpp_process($order, $fee) {
  return TRUE;
}

/**
 * PayPal website payments pro renew.
 */
function uc_recurring_paypal_wpp_renew($order, $fee) {
  return TRUE;
}


