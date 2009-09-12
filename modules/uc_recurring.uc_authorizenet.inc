<?php
// $Id$

/**
 * @file
 * Uc recurring implementation for the test gateway module.
 */

/**
 * Implementation of hook_recurring_info().
 */
function uc_recurring_uc_authorizenet_recurring_info() {
  $items['authorizenet_cim'] = array(
    'name' => t('Authorize.net (CIM)'),
    'payment method' => 'credit',
    'module' => 'uc_recurring',
    'fee handler' => 'authorizenet_cim',
    'renew' => 'uc_recurring_authorizenet_cim_renew',
    'process' => 'uc_recurring_authorizenet_cim_process',
  );
  return $items;
}

/**
 * Set up the recurring fee by creating a CIM profile for future payments
 *
 * @param $order
 *   The order object.
 * @param $fee
 *   The fee object.
 * @return
 *   TRUE if recurring fee setup
 */
function uc_recurring_authorizenet_cim_process($order, &$fee) {
  $data = array(
    'txn_type' => UC_CREDIT_REFERENCE_SET,
  );
  return uc_authorizenet_charge($order->order_id, $order->total_amount, $data);
}

/**
 * Process a renewal using the CIM profile
 *
 * @param $order
 *   The order object.
 * @param $fee
 *   The fee object.
 * @return
 *   TRUE if renewal succeeded
 */
function uc_recurring_authorizenet_cim_renew($order, &$fee) {
  if (!empty($order->data['cc_txns']['references'])) {
    $data = array(
      'txn_type' => UC_CREDIT_REFERENCE_TXN,
      'ref_id' => end(array_keys($order->data['cc_txns']['references'])),
    );
    return uc_authorizenet_charge($order->order_id, $order->total_amount, $data);
  }
  return FALSE;
}
