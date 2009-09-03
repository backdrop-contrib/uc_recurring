<?php
// $Id$

/**
 * @file
 * Uc recurring implementation for the credit card module.
 */

/**
 * Implementation of hook_recurring_info().
 */
function uc_recurring_uc_credit_recurring_info() {
  $items['credit'] = array(
    'name' => t('Credit card'),
    'payment method' => 'credit',
    'fee handler' => 'credit',
    'module' => 'UC recurring',
    'process' => 'uc_recurring_uc_credit',
    'renew' => 'uc_recurring_uc_credit',
  );
  return $items;
}

/**
 * Credit card process and renew.
 */
function uc_recurring_uc_credit($order, &$fee, $op) {
  // Get the default gateway that should be used to process the credit card.
  $gateway = uc_credit_default_gateway();
  // Invoke the correct payment method.
  return uc_recurring_invoke($gateway, $op, array($order, &$fee));
}
