<?php
// $Id$

/**
 * @file
 * Uc recurring implementation for the test gateway module.
 */

/**
 * Implementation of hook_recurring_info().
 */
function uc_recurring_test_gateway_recurring_info() {
  $items['test_gateway'] = array(
    'name' => t('Test Gateway'),
    'payment method' => 'credit',
    'fee handler' => 'test_gateway',
    'module' => 'UC recurring',
    'renew' => 'uc_recurring_test_gateway_renew',
    'process' => 'uc_recurring_test_gateway_process',
  );
  return $items;
}

function uc_recurring_test_gateway_process($order, &$fee) {
  $data = array(
    'billing' => array(
      'first_name' => $order->billing_first_name,
      'last_name' => $order->billing_last_name,
      'phone' => $order->billing_phone,
      'company' => $order->billing_company,
      'street1' => $order->billing_street1,
      'street2' => $order->billing_street2,
      'city' => $order->billing_city,
      'zone' => $order->billing_zone,
      'postal_code' => $order->billing_postal_code,
      'country' => $order->billing_country,
    ),
    'payment_details' => $order->payment_details,
  );

  if ($key = uc_credit_encryption_key()) {
    $crypt = new uc_encryption_class;
    $data['payment_details']['cc_number'] = $crypt->encrypt($key, $data['payment_details']['cc_number'], 32);
    if (variable_get('uc_credit_debug', FALSE)) {
      $data['payment_details']['cc_cvv'] = $crypt->encrypt($key, $data['payment_details']['cc_cvv'], 32);
    }
    $data['payment_details']['cc_exp_month'] = $crypt->encrypt($key, $data['payment_details']['cc_exp_month'], 32);
    $data['payment_details']['cc_exp_year'] = $crypt->encrypt($key, $data['payment_details']['cc_exp_year'], 32);
    uc_store_encryption_errors($crypt, 'uc_recurring');

    // Set gateway specific fee information for renewals.
    $fee->data['billing'] = $data['billing'];
    $fee->data['payment_details'] = $data['payment_details'];
  }

  return TRUE;
}


function uc_recurring_test_gateway_renew($order, &$fee) {
  if ($key = uc_credit_encryption_key()) {
    $crypt = new uc_encryption_class;
    $fee->data['payment_details']['cc_number'] = $crypt->decrypt($key, $fee->data['payment_details']['cc_number']);
    if (variable_get('uc_credit_debug', FALSE)) {
      $fee->data['payment_details']['cc_cvv'] = $crypt->decrypt($key, $fee->data['payment_details']['cc_cvv']);
    }
    $fee->data['payment_details']['cc_exp_month'] = $crypt->decrypt($key, $fee->data['payment_details']['cc_exp_month']);
    $fee->data['payment_details']['cc_exp_year'] = $crypt->decrypt($key, $fee->data['payment_details']['cc_exp_year']);
    uc_store_encryption_errors($crypt, 'uc_recurring');
  }

  // Cache the CC details stored by the handler.
  uc_credit_cache('save', $fee->data['payment_details'], FALSE);

  // Run the charge.
  $result = test_gateway_charge($fee->order_id, $fee->fee_amount, NULL);

  // Handle the result.
  if ($result['success'] === TRUE) {
    uc_payment_enter($fee->order_id, 'credit', $fee->fee_amount, 0, $result['data'], t('Recurring fee payment.') .'<br />'. $result['comment']);
  }

  return $result['success'];
}