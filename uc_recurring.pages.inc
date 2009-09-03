<?php
// $Id$

/**
 * @file
 * Recurring payments menu items.
 */

/**
 * Displays the confirmation form for cancelling a recurring fee.
 */
function uc_recurring_user_cancel_form($form_state, $user, $rfid) {
  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $user->uid,
  );

  $form['rfid'] = array(
    '#type' => 'value',
    '#value' => $rfid,
  );

  return confirm_form($form, t('Are you sure you want to cancel your recurring fee?'), 'user/'. $user->uid, t('This action cannot be undone and may result in the termination of subscription services.'), t('Yes, I want to cancel the subscription'), t('No, I want to continue with the subscription'));
}

/**
 * Implements hook_submit() for the cancel recurring fee form
 */
function uc_recurring_user_cancel_form_submit($form, &$form_state) {
  uc_recurring_fee_cancel($form_state['values']['rfid']);

  drupal_set_message(t('The recurring fee has been cancelled.'));

  $form_state['redirect'] = 'user/'. $form_state['values']['uid'];
}

