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
  if (variable_get('uc_authnet_cim_mode', 'disabled') != 'disabled') {
    $items['authorizenet'] = array(
      'name' => t('Authorize.net (CIM)'),
      'payment method' => 'credit',
      'module' => 'uc_recurring',
      'fee handler' => 'authorizenet',
      'renew callback' => 'uc_recurring_authorizenet_cim_renew',
      'process callback' => 'uc_recurring_authorizenet_cim_process',
      'menu' => array(
        'charge' => UC_RECURRING_MENU_DEFAULT,
        'edit'   => UC_RECURRING_MENU_DEFAULT,
        'cancel' => UC_RECURRING_MENU_DEFAULT,
      ), // Use the default user operation defined in uc_recurring.
    );
  }
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
  if (!variable_get('uc_authnet_cim_profile', FALSE)) {
    $data = array(
      'txn_type' => UC_CREDIT_REFERENCE_SET,
    );
    return uc_authorizenet_charge($order->order_id, $order->total_amount, $data);
  }
  return TRUE;
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
