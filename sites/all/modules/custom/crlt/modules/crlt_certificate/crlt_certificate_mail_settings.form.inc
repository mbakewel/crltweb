<?php

function crlt_certificate_mail_settings(&$form_state) {
  $form = array();
  $form['tokens'] = array(
    '#value' => theme('token_tree', array('all'), TRUE, TRUE),
  );
  foreach(crlt_certificate_mail_settings_keys() as $key => $label) {
    $form[$key . "_header"] = array(
      '#value' => '<h2>' . $label . '</h2>' 
    );
    $subject_key = $key . "_subject";
    $form[$subject_key] = array(
      '#type' => 'textfield',
      '#title' => 'Subject',
      '#default_value' => variable_get($subject_key, $subject_key)
    );
    $form[$key] = array(
      '#type' => 'textarea',
      '#title' => 'Body',
      '#default_value' => variable_get($key, $key),
      '#wysiwyg' => FALSE,
      '#rows' => 20 
    );
  }
  // Submit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save'
  );
  return $form;
}

function crlt_certificate_mail_settings_submit($form, &$form_state) {
  foreach(crlt_certificate_mail_settings_keys() as $key => $label) {
    variable_set($key, $form_state['values'][$key]);
    variable_set($key . "_subject", $form_state['values'][$key . "_subject"]);
  }
  drupal_set_message('Mail settings saved!');
}

function crlt_certificate_mail_settings_keys() {
  return array(
    'crlt_certificate_revision_requested' => 'Revision Requested',
    'crlt_certificate_needs_verification_C1' => 'C1 Needs Verification',
    'crlt_certificate_needs_verification_C2' => 'C2 Needs Verification',
    'crlt_certificate_needs_verification_D_internal' => 'D Needs Verification (@umich.edu)',
    'crlt_certificate_needs_verification_D_external' => 'D Needs Verification (external)',
    'crlt_certificate_needs_verification_user' => 'Needs Verification (user)'
  );
}

