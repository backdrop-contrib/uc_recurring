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
    'renew callback' => 'uc_recurring_authorizenet_cim_renew',
    'process callback' => 'uc_recurring_authorizenet_cim_process',
    'menu' => array(
      'charge' => UC_RECURRING_MENU_DEFAULT,
      'edit'   => UC_RECURRING_MENU_DEFAULT,
      'cancel' => UC_RECURRING_MENU_DEFAULT,
    ), // Use the default user operation defined in uc_recurring.
  );
  $items['authorizenet_arb'] = array(
    'name' => t('Authorize.net (ARB)'),
    'payment method' => 'credit',
    'module' => 'uc_recurring',
    'fee handler' => 'authorizenet_arb',
    'process' => 'uc_recurring_authorizenet_arb_process',
    'own handler' => TRUE,
  );

  if (variable_get('uc_authnet_cim_mode', 'disabled') != 'disabled') {
    $items['authorizenet'] = $items['authorizenet_cim'];
  }
  elseif (variable_get('uc_authnet_arb_mode', 'disabled') != 'disabled') {
    // ARB not currently supported, uncomment to test
    // $items['authorizenet'] = $items['authorizenet_arb'];
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
 * Process a renewal using the CIM profile.
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

/**
 * Set up the recurring fee using the ARB api.
 *
 * @param $order
 *   The order object.
 * @param $fee
 *   The fee object.
 * @return
 *   TRUE if recurring fee setup
 */
function uc_recurring_authorizenet_arb_process($order, &$fee) {
  // We can't just call the existing arb function in ubercart as that would try
  // and setup thing that uc_recurring does now for all recurring fees.
  // return uc_authorizenet_arb_create($order, $fee);
  $server = variable_get('uc_authnet_arb_mode', 'disabled');

  // Setup variables for the payment schedule.
  list($length, $unit) = explode(' ', $fee->regular_interval);
  list($trial_length, $trial_unit) = explode(' ', $fee->initial_charge);

  // Convert weeks and years to days.
  if ($unit == 'weeks') {
    $length *= 7;
    $unit = 'days';
  }
  elseif ($unit == 'years') {
    $length *= 365;
    $unit = 'days';
  }

  // Make sure we have valid values for Authorize.Net.
  if ($length <= 0 || $unit == 'days' && $length > 365 || $unit == 'months' && $length > 12) {
    watchdog('uc_authorizenet', 'Product @sku has invalid interval settings for Authorize.Net - @length @unit', array('@sku' => $fee->title, '@length' => $length, '@unit' => $unit), WATCHDOG_ERROR);
    return FALSE;
  }

  // Get the country data for the billing and shipping information.
  $billing_country = uc_get_country_data(array('country_id' => $order->billing_country));
  $delivery_country = uc_get_country_data(array('country_id' => $order->delivery_country));

  // Build the data array for the request.
  $data = array(
    'refId' => substr($order->order_id .'-'. time(), 0, 20),
    'subscription' => array(
      'name' => substr(t('Order @order_id', array('@order_id' => $order->order_id)), 0, 50),
      'paymentSchedule' => array(
        'interval' => array(
          'length' => $length,
          'unit' => $unit,
        ),
        'startDate' => date('Y-m-d', strtotime('+ '. $fee->initial_charge)),
        'totalOccurrences' => $fee->number_intervals,
        'trialOccurrences' => '0',
      ),
      'amount' => round($fee->fee_amount, 2),
      'trialAmount' => 0,
      'payment' => array(), // Data inserted below based on payment method.
      'order' => array(
        'invoiceNumber' => substr($order->order_id, 0, 20),
        'description' => substr(t('Order @order_id - @sku', array('@order_id' => $order->order_id, '@sku' => $fee->model)), 0, 255),
      ),
      'customer' => array(
        'id' => substr($order->uid, 0, 20),
        'email' => substr($order->primary_email, 0, 255),
        'phoneNumber' => substr($order->billing_phone, 0, 25),
        // 'faxNumber' => '',
      ),
      'billTo' => array(
        'firstName' => substr($order->billing_first_name, 0, 50),
        'lastName' => substr($order->billing_last_name, 0, 50),
        'company' => substr($order->billing_company, 0, 50),
        'address' => substr($order->billing_street1, 0, 60),
        'city' => substr($order->billing_city, 0, 40),
        'state' => substr(uc_get_zone_code($order->billing_zone), 0, 2),
        'zip' => substr($order->billing_postal_code, 0, 20),
        'country' => !$billing_country ? '' : $billing_country[0]['country_iso_code_2'],
      ),
      'shipTo' => array(
        'firstName' => substr($order->delivery_first_name, 0, 50),
        'lastName' => substr($order->delivery_last_name, 0, 50),
        'company' => substr($order->delivery_company, 0, 50),
        'address' => substr($order->delivery_street1, 0, 60),
        'city' => substr($order->delivery_city, 0, 40),
        'state' => substr(uc_get_zone_code($order->delivery_zone), 0, 2),
        'zip' => substr($order->delivery_postal_code, 0, 20),
        'country' => !$delivery_country ? '' : $delivery_country[0]['country_iso_code_2'],
      ),
    ),
  );

  // Strip out the shipping info if it isn't necessary.
  if (empty($data['subscription']['shipTo']['firstName'])) {
    unset($data['subscription']['shipTo']);
  }

  // Add the payment information to the data array based on the payment method.
  if ($order->payment_method == 'credit') {
    if ($order->payment_details['cc_exp_month'] < 10) {
      $order->payment_details['cc_exp_month'] = '0'. $order->payment_details['cc_exp_month'];
    }

    $data['subscription']['payment'] = array(
      'creditCard' => array(
        'cardNumber' => $order->payment_details['cc_number'],
        'expirationDate' => $order->payment_details['cc_exp_year'] .'-'. $order->payment_details['cc_exp_month'],
      ),
    );
  }
 
  // Build the XML string.
  $xml = _uc_authorizenet_xml_api_wrapper('ARBCreateSubscriptionRequest', _uc_authorizenet_array_to_xml($data));

  // Send the request off to the server and get the response.
  $response = uc_authorizenet_xml_api($server, $xml);

  // Fail if the response is empty or FALSE.
  if (!$response) {
    return FALSE;
  }

  // Parse the response into a data array.
  $data = _uc_authorizenet_arb_parse_response($response);

  if ($data['resultCode'] == 'Error') {
    uc_order_comment_save($order->order_id, 0, t('Authorize.Net: Recurring fee for @model failed.<br />@error - @text', array('@model' => $fee->model, '@error' => $data['code'], '@text' => $data['text'])), 'admin');
    return FALSE;
  }

  return TRUE;
}

