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
/*
  $items['paypal_wpp'] = array(
    'name' => t('PayPal website payments pro'),
    'payment method' => 'paypal_wpp',
    'fee handler' => 'paypal_wpp',
    'module' => 'UC recurring',
    'renew' => 'uc_recurring_paypal_wpp_renew',
    'process' => 'uc_recurring_paypal_wpp_process',
  );
*/
  return $items;
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


