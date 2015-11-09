<?php

/**
 * CRLT Grant Report Mail Settings form
 *
 * Handles cron scheduling and email content
 */
function crlt_grant_report_mail_settings( &$form_state ) {
  $lastrun = variable_get( 'crlt_grant_report_mail_sent', time() );
  $form = array();
  $form['crlt_grant_report_mail_timer'] = array(
    '#type' => 'select',
    '#title' => t( 'Process grant report workflow and reminders every' ),
    '#default_value' => variable_get( 'crlt_grant_report_mail_timer', 604800 ),
    '#options' => drupal_map_assoc( array( 300, 3600, 10800, 21600, 32400, 43200, 86400, 172800, 259200, 604800, 1209600, 2419200, 4838400, 9676800 ), 'format_interval' ),
    '#description' => t( 'Grant report workflow will be processed and reminder emails automatically sent. (Requires a correctly configured <a href="@cron">cron maintenance task</a>.)<br/>Schedule interval based on the last run time: <strong>@lastrun</strong>', array( '@cron' => url( 'http://drupal.org/cron' ), '@lastrun' => format_date($lastrun) ) ),
  );
  $form['buttons']['crlt_grant_report_mail_settings_refresh'] = array(
    '#type' => 'submit',
    '#value' => t( 'Send report reminders' ),
    '#submit' => array( 'crlt_grant_report_mail_settings_submit_send' ),
    '#weight' => 10,
  );
  $form['tokens'] = array(
    '#value' => theme( 'token_tree', array( 'all' ), TRUE, TRUE ),
  );
  foreach ( crlt_grant_report_mail_settings_keys() as $key => $label ) {
    $form[$key . "_header"] = array(
      '#value' => '<h2>' . $label . '</h2>'
    );
    $subject_key = $key . "_subject";
    $form[$subject_key] = array(
      '#type' => 'textfield',
      '#title' => 'Subject',
      '#default_value' => variable_get( $subject_key, $subject_key )
    );
    $form[$key] = array(
      '#type' => 'textarea',
      '#title' => 'Body',
      '#default_value' => variable_get( $key, $key ),
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

/**
 * Settings submission. Set variables to update settings.
 */
function crlt_grant_report_mail_settings_submit( $form, &$form_state ) {
  variable_set( 'crlt_grant_report_mail_timer', $form_state['values']['crlt_grant_report_mail_timer'] );
  foreach ( crlt_grant_report_mail_settings_keys() as $key => $label ) {
    variable_set( $key, $form_state['values'][$key] );
    variable_set( $key . "_subject", $form_state['values'][$key . "_subject"] );
  }
  drupal_set_message( 'Mail settings saved!' );
}
/**
 * Submit form and send report reminder emails
 */
function crlt_grant_report_mail_settings_submit_send( $form, &$form_state ) {
  // Process form submit normally
  crlt_grant_report_mail_settings_submit( $form, $form_state );

  // Check the current update timer
  $conf_delta = variable_get( 'crlt_grant_report_mail_timer', 604800 );

  // Check when the last update occurred
  $last_time = variable_get( 'crlt_grant_report_mail_sent', 0 );

  // Capture current run time
  $run_time = time();

  // Send report reminder emails if the amount of time specified by the update
  // timer has passed
  $actual_delta = $run_time - $last_time;

  // Watchdog log
  watchdog(
    'crlt_grant_app',
    'Manual Cron: At @run_time. Previous execution @last_time. @actual_delta seconds between executions. @conf_delta seconds configured between executions.',
    array(
      '@run_time'=>format_date($run_time),
      '@last_time'=>format_date($last_time),
      '@actual_delta'=>$actual_delta,
      '@conf_delta'=>$conf_delta,        
    ), 
    WATCHDOG_WARNING
  );

  // Execute cron function manually
  _crlt_grant_application_report_reminders( FALSE );

  // Record when the action was completed
  variable_set( 'crlt_grant_report_mail_sent', $run_time );
}

function crlt_grant_report_mail_settings_keys() {
  $keys = array();
  $node_type = "grant";
  $grants_results = db_query( "SELECT nid, title FROM {node} WHERE type = '%s' ", $node_type );
  while ( $grant = db_fetch_object ( $grants_results ) ) {
    $keys['crlt_grant_report_reminder_'.$grant->nid.'_01'] = $grant->title . ': Report reminder 1';
    $keys['crlt_grant_report_reminder_'.$grant->nid.'_02'] = $grant->title . ': Report reminder 2';
    $keys['crlt_grant_report_reminder_'.$grant->nid.'_03'] = $grant->title . ': Report reminder 3';
  }
  return $keys;
}
